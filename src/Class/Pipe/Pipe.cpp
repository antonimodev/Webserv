#include <unistd.h>

#include "Pipe.hpp"
#include "PipeException.hpp"

Pipe::Pipe(void) {
    _fd[0] = -1;
    _fd[1] = -1;

	if (pipe(_fd) == -1) {
		throw PipeException("Error: pipe() failed");
	}
}

Pipe::~Pipe(void) {
	closeReadPipe();
	closeWritePipe();
}

int Pipe::getReadPipe(void) const {
	return _fd[0];
}

int Pipe::getWritePipe(void) const {
	return _fd[1];
}

void Pipe::closeReadPipe(void) {
	if (_fd[0] != -1) {
		close(_fd[0]);
		_fd[0] = -1;
	}
}

void Pipe::closeWritePipe(void) {
	if (_fd[1] != -1) {
		close(_fd[1]);
		_fd[1] = -1;
	}
}