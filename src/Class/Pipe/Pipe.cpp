#include <unistd.h>

#include "Pipe.hpp"
#include "PipeException.hpp"


// CONSTRUCTORS
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


// FUNCTIONS
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


void Pipe::fdRedirection(int src, RedirectionMode mode) {
	if (mode == READ) {
		if (dup2(getReadPipe(), src) == -1)
			throw PipeException("Error: dup2() failed for READ");
		closeWritePipe();
		closeReadPipe();
	} else if (mode == WRITE) {
		if (dup2(getWritePipe(), src) == -1)
			throw PipeException("Error: dup2() failed for WRITE");
		closeReadPipe();
		closeWritePipe(); 
	}
}