#pragma once


class Pipe {
	private:
		int _fd[2];

		Pipe(const Pipe&);
		Pipe& operator=(const Pipe&);

	public:
		enum PipeMode {READ, WRITE};

		/**
		 * @brief Construct a new Pipe object and set it non-blocking.
		 * @throw PipeException if pipe() or fcntl() fails.
		 */
		Pipe(void);

		/**
		 * @brief Destroy the Pipe object and close both ends of the pipe.
		 */
		~Pipe(void);

		int		getReadPipe(void) const;
		int		getWritePipe(void) const;

		void	closeReadPipe(void);
		void	closeWritePipe(void);

		/**
		 * @brief Redirects file descriptor src to the pipe end specified by mode.
		 * @param src Source file descriptor.
		 * @param mode Pipe end to redirect to (READ or WRITE).
		 * @throw PipeException if dup2() fails.
		 */
		void	fdRedirection(int src, PipeMode mode);

		/**
		 * @brief Releases the file descriptor of the specified pipe end.
		 * @param mode Pipe end to release (READ or WRITE).
		 * @return int The released file descriptor.
		 */
		int		fdRelease(PipeMode mode);
};
