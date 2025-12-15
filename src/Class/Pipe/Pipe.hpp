#pragma once


class Pipe {
	private:
		enum RedirectionMode {
			READ,
			WRITE
		};

		int _fd[2];

		Pipe(const Pipe&);
		Pipe& operator=(const Pipe&);

	public:
		Pipe(void);
		~Pipe(void);

		int		getReadPipe(void) const;
		int		getWritePipe(void) const;

		void	closeReadPipe(void);
		void	closeWritePipe(void);

		void	fdRedirection(int src, RedirectionMode mode);
};