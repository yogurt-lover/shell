#ifndef _PIPESH_
#define _PIPESH_
void set_pipe_stdin(int command_num, int pipes[2][2]);
void set_pipe_stdout(int command_num, int pipes[2][2]);
void pipe_handler(char **args, int command_num, int pipe_num, int pipes[2][2]);
int exec_pipe(char **args, int num_args, int command_num, int pipe_num, int pipes[2][2]);
#endif
