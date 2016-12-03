#ifndef _PIPESH_
#define _PIPESH_
/*======== void set_pipe_stdin() ==========
 * Inputs: int command_num
           int pipes[2][2]
 * Returns: none
 *
 * Closes the stdin file descriptor
 * Depending on the parity of command_num, it will duplicate the "read" end of the pipe into
 * the stdin file descriptor
 * Then, it will close the file descriptors used up by the pipe (since the process doesn't need them anymore)
 * ====================*/
void set_pipe_stdin(int command_num, int pipes[2][2]);

/*======== void set_pipe_stdout() ==========
 * Inputs: int command_num
           int pipes[2][2]
 * Returns: none
 *
 * Closes the stdout file descriptor
 * Depending on the parity of command_num, it will duplicate the "write" end of the pipe into
 * the stdout file descriptor
 * Then, it will close the file descriptors used up by the pipe (since the process doesn't need them anymore)
 * ====================*/
void set_pipe_stdout(int command_num, int pipes[2][2]);

/*======== void pipe_handler() ==========
 * Inputs: char **args
           int command_num
           int pipe_num
           int pipes[2][2]
 * Returns: none
 *
 * Called by exec_pipe to set the stdin and stdout of piped commands dependent on their command_num
 * Entire function will run within a child process
 * If command_num is zero, the process's stdout file descriptor will be changed using set_pipe_stdout
 * Conversely, if command_num is at its last iteration, the process's stdin file descriptor will be changed 
 * using set_pipe_stdin
 * If the command_num is anything in between, both the stdout and stdin file descriptors will be changed.
 * The command will then be executed with exec_coreutil (in exec.c)
 * As the command_num counter increases (in shell.c), file descriptor associated with pipes that have 
 * been "used up" will be closed. 
 * ====================*/
void pipe_handler(char **args, int command_num, int pipe_num, int pipes[2][2]);

/*======== int exec_pipe() ==========
 * Inputs: char **args
           int num_args
           int command_num
           int pipe_num
           int pipes[2][2]
 * Returns: 1 if the command specified in args was completed successfuly, 0 otherwise
 *
 * Does a series of checks to see if a command is not a shell built in.
 * (This is a feature in bash that we weren't able to resolve - see BUGS)
 * If the command isn't a built-in, a pipe will be set up using the system call pipe(int[2])
 * A call to pipe_handler will then be made
 * ====================*/
int exec_pipe(char **args, int num_args, int command_num, int pipe_num, int pipes[2][2]);
#endif
