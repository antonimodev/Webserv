#include <iostream>
#include <sstream>

#include "Parser.hpp"
#include "../../../include/webserv.h"

#include "../../Exceptions/HttpCodeException/HttpCodeException.hpp"
#include "../../Exceptions/PendingRequestException.hpp"


Parser::Parser(void) {}


Parser::Parser(const Parser& other) {
	(void)other;
}


Parser& Parser::operator=(const Parser& other) {
	(void)other;
	return *this;
}


Parser::~Parser(void) {}


// PRIVATE - REQUEST LINE VALIDATION


void	Parser::validMethod(const std::string& method) {
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw HttpCodeException(BAD_REQUEST, "Error: " + method + " is not a valid method");
}


void	Parser::validRoute(const std::string& route) {
	if (route.empty() || route[0] != '/')
		throw HttpCodeException(BAD_REQUEST, "Error: route must start with '/'");

	if (route.find("..") != std::string::npos)
		throw HttpCodeException(BAD_REQUEST, "Error: path traversal not allowed");

	for (size_t i = 0; i < route.size(); ++i) {
		if (!std::isgraph(static_cast<unsigned char>(route[i])))
			throw HttpCodeException(BAD_REQUEST, "Error: invalid character in route");
	}
}


void	Parser::validVersion(const std::string& version) {
	if (version != "HTTP/1.1")
		throw HttpCodeException(BAD_REQUEST, "Error: " + version + " not valid");
}


// PRIVATE - PARSING


std::string	Parser::extractRoute(const std::string& full_route) {
	size_t pos = full_route.find('?');

	if (pos != std::string::npos)
		return full_route.substr(0, pos);

	return full_route;
}


std::string	Parser::extractQuery(const std::string& full_route) {
	size_t pos = full_route.find('?');

	if (pos != std::string::npos)
		return full_route.substr(pos + 1);

	return "";
}


void	Parser::parseRequestLine(const std::string& request, HttpRequest& http_struct, size_t& pos) {
	size_t endl = request.find("\r\n", pos);

	if (endl == std::string::npos)
		throw HttpCodeException(BAD_REQUEST, "Error: malformed request line");

	std::string request_line = request.substr(pos, endl - pos);
	std::istringstream iss(request_line);
	std::string extra;
	std::string full_route;

	if (!(iss >> http_struct.method >> full_route >> http_struct.version))
		throw HttpCodeException(BAD_REQUEST, "Error: malformed request line");

	if (iss >> extra)
		throw HttpCodeException(BAD_REQUEST, "Error: malformed request line");

	http_struct.route = extractRoute(full_route);
	http_struct.query = extractQuery(full_route);

	validMethod(http_struct.method);
	validRoute(http_struct.route);
	validVersion(http_struct.version);

	pos = endl + 2;
}


void	Parser::parseHeaders(const std::string& request, HttpRequest& http_struct, size_t& pos) {
	while (pos < request.size()) {
		size_t endl = request.find("\r\n", pos);
		if (endl == std::string::npos)
			break;

		if (endl == pos) {
			pos += 2;  // Skip "\r\n"
			return;
		}

		std::string header_line = request.substr(pos, endl - pos);
		size_t colon_pos = header_line.find(':');

		if (colon_pos == std::string::npos)
			throw HttpCodeException(BAD_REQUEST, "Error: malformed header line");

		std::string key = trim_space(header_line.substr(0, colon_pos));
		std::string value = trim_space(header_line.substr(colon_pos + 1));

		if (key.empty())
			throw HttpCodeException(BAD_REQUEST, "Error: empty header key");

		if (key.find(' ') != std::string::npos)
			throw HttpCodeException(BAD_REQUEST, "Error: header name cannot contain spaces");

		http_struct.headers[key] = value;
		pos = endl + 2;
	}
}


void	Parser::parseBody(const std::string& request, HttpRequest& http_struct, size_t pos) {
	std::map<std::string, std::string>::const_iterator it = http_struct.headers.find("Content-Length");

	if (it == http_struct.headers.end()) {
		return; // No body expected
	}

	std::istringstream iss(it->second);
	size_t content_length = 0;
	iss >> content_length;

	if (iss.fail())
		throw HttpCodeException(BAD_REQUEST, "Error: invalid Content-Length");

	if (pos + content_length > request.size())
		throw PendingRequestException("Info: waiting to complete request body reading");

	http_struct.body = request.substr(pos, content_length);
}


// PUBLIC


HttpRequest Parser::parseHttpRequest(const std::string& request) {
	HttpRequest http_struct;
	size_t pos = 0;

	parseRequestLine(request, http_struct, pos);
	parseHeaders(request, http_struct, pos);

	// Extraer y guardar boundary si es multipart/form-data
	http_struct.multipart_boundary = extractMultipartBoundary(http_struct);

	parseBody(request, http_struct, pos);


	return http_struct;
}


// ANOTHER FUNCTIONS


const std::string	get_mime_type(const std::string& extension) {
	if (extension == "html") return "text/html";
	if (extension == "css")  return "text/css";
	if (extension == "png")  return "image/png";
	if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
	if (extension == "ico")  return "image/x-icon";
	if (extension == "txt")  return "text/plain";

	return "text/html";
}

// Detecta si Content-Type es multipart/form-data y extrae el boundary
std::string Parser::extractMultipartBoundary(const HttpRequest& http_struct) {
    // Busca el header "Content-Type" en el mapa de headers.
    std::map<std::string, std::string>::const_iterator it = http_struct.headers.find("Content-Type");
    if (it == http_struct.headers.end())
        return ""; // Si no existe, no es multipart.

    // Obtiene el valor del header Content-Type.
    const std::string& content_type = it->second;

    // Define el prefijo que buscamos: "multipart/form-data;"
    std::string prefix = "multipart/form-data;";
    size_t pos = content_type.find(prefix);
    if (pos == std::string::npos)
        return ""; // Si no está el prefijo, no es multipart.

    // Busca la subcadena "boundary=" después del prefijo.
    size_t boundary_pos = content_type.find("boundary=", pos + prefix.length());
    if (boundary_pos == std::string::npos)
        return ""; // Si no está boundary=, no es válido.

    boundary_pos += 9; // Avanza 9 caracteres para saltar "boundary=".

    // Busca el final del boundary (puede estar seguido de ';' o ser el final del string).
    size_t end = content_type.find(';', boundary_pos);

    // Extrae el boundary desde boundary_pos hasta end.
    std::string boundary = content_type.substr(boundary_pos, end - boundary_pos);

    // Elimina espacios o comillas al principio.
    while (!boundary.empty() && (boundary[0] == ' ' || boundary[0] == '\"'))
        boundary.erase(0, 1);

    // Elimina espacios o comillas al final.
    while (!boundary.empty() && (boundary[boundary.size()-1] == ' ' || boundary[boundary.size()-1] == '\"'))
        boundary.erase(boundary.size()-1, 1);

    // Devuelve el boundary limpio.
    return boundary;
}

// Parsea el body multipart/form-data y extrae archivos subidos
std::vector<Parser::UploadedFile> Parser::parseMultipartFormData(const std::string& body, const std::string& boundary) {
    std::vector<Parser::UploadedFile> files;
	if (boundary.empty())
		return files;

	std::string delim = "--" + boundary;
	std::string end_delim = delim + "--";
	size_t pos = 0;

	while (true) {
		// Buscar el inicio de la siguiente parte
		size_t part_start = body.find(delim, pos);

		if (part_start == std::string::npos)
			break;
		
		part_start += delim.length();
		if (body.compare(part_start, 2, "--") == 0)
			break; // Es el final
		
		// Saltar CRLF
		if (body.compare(part_start, 2, "\r\n") == 0)
			part_start += 2;

		// Buscar el final de los headers de la parte
		size_t headers_end = body.find("\r\n\r\n", part_start);
		if (headers_end == std::string::npos)
			break;
		
		std::string part_headers = body.substr(part_start, headers_end - part_start);

		// Buscar Content-Disposition y filename
		size_t disp_pos = part_headers.find("Content-Disposition:");

		if (disp_pos == std::string::npos) {
			pos = headers_end + 4;
			continue;
		}

		size_t filename_pos = part_headers.find("filename=", disp_pos);
		
		if (filename_pos == std::string::npos) {
			pos = headers_end + 4;
			continue; // No es archivo
		}

		filename_pos += 9; // Saltar 'filename='
		char quote = part_headers[filename_pos];
		size_t filename_end = part_headers.find(quote, filename_pos + 1);
		std::string filename = part_headers.substr(filename_pos + 1, filename_end - filename_pos - 1);

		// El contenido empieza después de los headers y CRLFCRLF
		size_t content_start = headers_end + 4;
		// Buscar el siguiente boundary
		size_t content_end = body.find(delim, content_start);
		if (content_end == std::string::npos)
			break;
		// Eliminar el CRLF final si existe
		if (content_end > 1 && body[content_end - 2] == '\r' && body[content_end - 1] == '\n')
			content_end -= 2;
		std::string file_content = body.substr(content_start, content_end - content_start);

		UploadedFile file;
		file.filename = filename;
		file.content = file_content;
		files.push_back(file);

		pos = content_end;
	}
	return files;
}
