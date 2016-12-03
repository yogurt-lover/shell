#ifndef _PIPESH_
#define _PIPESH_
/*======== void set_pipe_stdin() ==========
 * Inputs: int command_num
           int pipes[][]
 * Returns: none
 *
 * Dynamically allocates a buffer for use throughout the shell
 * If Ctrl-D is pressed, the program exits
 * ====================*/
void set_pipe_stdin(int command_num, int pipes[2][2]);

/*======== void set_pipe_stdout() ==========
 * Inputs: int command_num
           int pipes[][]
 * Returns: none
 *
 * Dynamically allocates a buffer for use throughout the shell
 * If Ctrl-D is pressed, the program exits
 * ====================*/
void set_pipe_stdout(int command_num, int pipes[2][2]);

/*======== void pipe_handler() ==========
 * Inputs: char **args
           int command_num
           int pipe_num
           int pipes[][]
 * Returns: none
 *
 * Dynamically allocates a buffer for use throughout the shell
 * If Ctrl-D is pressed, the program exits
 * ====================*/
void pipe_handler(char **args, int command_num, int pipe_num, int pipes[2][2]);

/*======== int exec_pipe() ==========
 * Inputs: char **args
           int num_args
           int command_num
           int pipe_num
           int pipes[][]
 * Returns: int
 *
 * Dynamically allocates a buffer for use throughout the shell
 * If Ctrl-D is pressed, the program exits
 * ====================*/
int exec_pipe(char **args, int num_args, int command_num, int pipe_num, int pipes[2][2]);
#endif
