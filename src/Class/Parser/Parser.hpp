#pragma once

#include <string>
#include <map>

/**
 * @struct HttpRequest
 * @brief Holds parsed components of an HTTP request.
 */
struct HttpRequest {
	// START_LINE, EXAMPLE: GET /index.html HTTP/1.1
	std::string method;
	std::string route; 		// e.g	/script.php
	std::string	query;		// e.g	user=antonimo&id=42
	std::string version;

	// HEADERS
	std::map<std::string, std::string> headers; // e.g headers["Content-Length"] = "500";

	// BODY (Content to POST (images, text...))
	std::string body;

	// Boundary para multipart/form-data (si aplica)
	std::string multipart_boundary;
};


class Parser {
   public:
	   /**
		* @struct UploadedFile
		* @brief Representa un archivo subido por multipart/form-data.
		*/
	   struct UploadedFile {
		   std::string filename;
		   std::string content;
	   };

	   /**
		* @brief Parsea el body multipart/form-data y extrae archivos subidos.
		* @param body El body completo de la petición.
		* @param boundary El boundary extraído del header.
		* @return Vector de archivos subidos.
		*/
	   static std::vector<UploadedFile> parseMultipartFormData(const std::string& body, const std::string& boundary);

   private:
		Parser(void);
		Parser(const Parser&);
		Parser& operator=(const Parser&);
		~Parser(void);

		// Request Line validation
		static void	validMethod(const std::string& method);
		static void	validRoute(const std::string& route);
		static void	validVersion(const std::string& version);

		static std::string	extractRoute(const std::string& full_route);
		static std::string	extractQuery(const std::string& full_route);

		/**
		 * @brief Parses the first line: "METHOD ROUTE HTTP/1.1\r\n"
		 * @param request Full request string.
		 * @param http_struct Output struct to populate.
		 * @param pos In/Out position, updated to after "\r\n".
		 * @complexity O(n) where n is length of request line.
		 */
		static void	parseRequestLine(const std::string& request, HttpRequest& http_struct, size_t& pos);

		/**
		 * @brief Parses headers until empty line "\r\n\r\n".
		 * @param request Full request string.
		 * @param http_struct Output struct to populate headers map.
		 * @param pos In/Out position, updated to start of body.
		 * @complexity O(n) where n is total length of headers.
		 */
		static void	parseHeaders(const std::string& request, HttpRequest& http_struct, size_t& pos);

		/**
		 * @brief Extracts body based on Content-Length header.
		 * @param request Full request string.
		 * @param http_struct Struct with headers already parsed.
		 * @param pos Starting position of body.
		 * @complexity O(n) where n is body length.
		 */
		static void	parseBody(const std::string& request, HttpRequest& http_struct, size_t pos);

	public:
	/**
	 * @brief Detecta si Content-Type es multipart/form-data y extrae el boundary.
	 * @param http_struct Estructura HttpRequest con headers ya parseados.
	 * @return boundary si existe, string vacío si no es multipart.
	 */
	static std::string extractMultipartBoundary(const HttpRequest& http_struct);
		/**
		 * @brief Parses a raw HTTP request string into HttpRequest struct.
		 * @param request Raw HTTP request as received from client.
		 * @return Populated HttpRequest structure.
		 * @throws HttpCodeException on malformed request.
		 */
		static HttpRequest parseHttpRequest(const std::string& request);
};
