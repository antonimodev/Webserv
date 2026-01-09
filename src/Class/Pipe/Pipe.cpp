#include <unistd.h>

#include "Pipe.hpp"
#include "Socket.hpp"

#include "PipeException.hpp"


/*****************************************************************************
*                               CONSTRUCTORS                                 *
*****************************************************************************/

Pipe::Pipe(void) {
	_fd[0] = -1;
	_fd[1] = -1;

	if (pipe(_fd) == -1) {
		throw PipeException("Error: pipe() failed");
	}

	Socket::setNonBlocking(getReadPipe());
	Socket::setNonBlocking(getWritePipe());
}


Pipe::~Pipe(void) {
	closeReadPipe();
	closeWritePipe();
}


/*****************************************************************************
*                                FUNCTIONS                                   *
*****************************************************************************/

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


void Pipe::fdRedirection(int src, PipeMode mode) {
	if (mode == READ) {
		if (dup2(getReadPipe(), src) == -1)
			throw PipeException("Error: dup2() failed for READ");
	} else if (mode == WRITE) {
		if (dup2(getWritePipe(), src) == -1)
			throw PipeException("Error: dup2() failed for WRITE");
	}

	closeWritePipe();
	closeReadPipe();
}


int Pipe::fdRelease(PipeMode mode) {
	int	out = -1;

	switch (mode) {
		case Pipe::READ:
			out = getReadPipe();
			_fd[0] = -1;
			return out;

		case Pipe::WRITE:
			out = getWritePipe();
			_fd[1] = -1;
			return out;
	}
	return -1;
}
