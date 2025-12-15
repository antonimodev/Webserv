#pragma once


class Pipe {
	private:
		int _fd[2];

		Pipe(const Pipe&);
		Pipe& operator=(const Pipe&);

	public:
		enum RedirectionMode {READ, WRITE};

		Pipe(void);
		~Pipe(void);

		int		getReadPipe(void) const;
		int		getWritePipe(void) const;

		void	closeReadPipe(void);
		void	closeWritePipe(void);

		void	fdRedirection(int src, RedirectionMode mode);
};