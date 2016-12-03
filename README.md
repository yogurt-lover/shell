# SASH (The Seiji-Anton shell)

## Features
A lovely shell capable of executing commands in one's PATH.  
Can be used very similarly to your standard Bash shell.  
* Presents your host name and current working directory in a beautiful light blue.
* Will fork and execute commands.
* Support for multiple commands separated by ";"
* Capable of of redirection, including "<", ">", ">>", "&>", "&>>", "2>", "2>>", "1>", and "1>>". Also supports multiple pipes! "|"
* You can put as many spaces between flags and commands as you like!

## Attempted
* Did create a colorized "ls" originally, using default colors per operating system, however was dropped in final version due to reorganization of methods and lack to time to re-implement.
* ~/[nextdir] Does not work. However, ~ itself does work as home.
* Piping through a built-in doesn't work (kept looping infinitely for some reason). So if this is attempted, SASH will return an error
* The '$' character in the prompt prints in a new line because when it was printed on the same line as the rest of the prompt, tabbing and deleting would have unexpected results (i.e. characters that were deleted would be displayed)

## Bugs
* A more complicated command like `ls < filename | grep e | wc > output` will not work (In this case, the parser will set `filename` as the STDIN file and will ignore everything afterwards. But `ls | grep e | wc > output` will work.

## Files & Function Headers
### prompt.c
Handles prompt printing

```
/*======== char *get_home_dir() ==========
 * Inputs: none
 * Returns: Home directory string
 *
 * Get environment variable "HOME"
 * ====================*/

/*======== char *get_username() =========
 * Inputs: none
 * Returns: User string
 * 
 * Get environment variable "USER"
 * ====================*/

/*======== char *get_hostname() =========
 * Inputs: none
 * Returns: Hostname string
 * 
 * Dynamically allocates buffer with gethostname()
 * ====================*/

/*======== char *get_cwd() =========
 * Inputs: none
 * Returns: CWD string
 * 
 * Dynamically allocates buffer with getcwd()
 * ====================*/

/*======== void print_prompt() =========
 * Inputs: none
 * Returns: none
 * 
 * Prints shell prompt and frees cwd and hostname memory
 * ====================*/
```
### parse.c
Handles parsing of a command's arguments

```
/*======== char *read_raw() ==========
 * Inputs: none
 * Returns: Dynamically allocated string from terminal input
 *
 * Dynamically allocates a buffer for use throughout the shell
 * If Ctrl-D is pressed, the program exits
 * ====================*/

/*======== char **get_args() ==========
 * Inputs: char *input
 *         int *num_args
 * Returns: Array of strings, where each entry is a whitespace separated token of the user input
 *
 * Dynamically allocates a buffer for use throughout the shell
 * If an argument is "~", it gets replaced with the home directory.
 * ====================*/
```
### exec.c
Defines shell-builtins (cd and exit) and handles execution of programs in $PATH

```
/*======== void cd_def() ==========
 * Inputs: char **args
 * Returns: none
 * 
 * Changes directory of current process
 * If unsuccessful, print error message
 * If no arguments after "cd", then goes to home directory
 * ====================*/

/*======== void exec_coreutil() ==========
 * Inputs: char **args
 * Returns: none
 * 
 * Executes command in $PATH
 * If execution fails, print error message
 * ====================*/

/*======== void exec_coreutil() ==========
 * Inputs: char **args, int *status
 * Returns: none
 * 
 * Tests for special commands such as "cd" or "exit" and runs cd_def
 * and sets exit status respectively (0 if exit is run, 1 otherwise).
 * Forks, and runs exec_coreutil for all else.
 * ====================*/
```
### redirect.c
Handles redirection to and from files

```
/*======== void redirect_stdout() ==========
 * Inputs: int *redirect
           int *dup_stdout
 * Returns: none
 *
 * Duplicates and closes file descriptor 1 (STDOUT)
 * Sets dup_stdout to the duplicate of STDOUT
 * Sets the stdout flag in redirect to 1
 * ====================*/

/*======== void redirect_stderrt() ==========
 * Inputs: int *redirect
           int *dup_stderr
 * Returns: none
 *
 * Duplicates and closes file descriptor 2 (STDERR)
 * Sets dup_stderr to the duplicate of STDERR
 * Sets the stderr flag in redirect to 1
 * ====================*/

/*======== void redirect_stdin() ==========
 * Inputs: int *redirect
           int *dup_stdin
 * Returns: none
 *
 * Duplicates and closes file descriptor 0 (STDIN)
 * Sets dup_stdin to the duplicate of STDIN
 * Sets the stdin flag in redirect to 1
 * ====================*/

/*======== void change_stdout() ==========
 * Inputs: char *input
           int *redirect
           int *dup_stdout
           int *dup_stderr
 * Returns: none
 *
 * Redirects stdout and/or stderr to the file specified in the argument after the '>' character in the input string
 * Doesn't modify the input
 * Checks for the occurrence of ">>" in the input string so an append flag can be added
 * If an argument isn't found after the '>' characters, then the failed flag in redirect is set to 1
 * Otherwise, stdout and stderr will be changed using redirect_stdout and redirect_stderr depending on which
 * redirect characters were used (&>, 2>, >)
 * ====================*/

/*======== void change_stdin() ==========
 * Inputs: char *input
           int *redirect
           int *dup_stdin
 * Returns: none
 *
 * Redirects stdin to the file specified in the argument after the '<' character in the input string
 * Doesn't modify the input
 * If an argument isn't found after the '<' character, then the failed flag in redirect is set to 1
 * Otherwise, stdin will be changed using redirect_stdin

/*======== void restore_stdin() ==========
 * Inputs: int dup_stdin
 * Returns: none
 *
 * Closes the current stdin file descriptor and replaces it with the duplicate file descriptor created in
 * redirect_stdin
 * It will only be called if the stdin flag in the redirect variable is one
 * ====================*/

/*======== void restore_stdout() ==========
 * Inputs: int dup_stdout
 * Returns: none
 *
 * Closes the current stdout file descriptor and replaces it with the duplicate file descriptor created in
 * redirect_stdout
 * It will only be called if the stdout flag in the redirect variable is one
 * ====================*/

/*======== void restore_stderr() ==========
 * Inputs: int dup_stderr
 * Returns: none
 *
 * Closes the current stderr file descriptor and replaces it with the duplicate file descriptor created in
 * redirect_stderr
 * It will only be called if the stderr flag in the redirect variable is one
 * ====================*/
```
### pipes.c
Handles redirection to file descriptors created by the pipe() system call and execution of commands in pipes

```
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
```
### shell.c
Main shell file

```
/*======== int print_args() ==========
 * Inputs: char **args
 		   int num_args
 * Returns: none
 *
 * Debug method. Prints all parsed arguments placed in args array.
 * ====================*/

/*======== void process() ==========
 * Inputs: none
 * Returns: none
 *
 * Main processing method. Begins by printing the Shell prompt.
   awaits user input, and runs read_raw() on it.
 * The sequence of parsing and execution goes:
 * * Separation of raw_input using ';' as a delimiter (each string being a single_input)
 * * Set file redirection ('<', '>') and remove the arguments that come along them from single_input
 * * Calculating number of pipes in the single_input
 * * If there aren't any pipes, then exec_single (in exec.c) will be called
 * * Otherwise, single_input will be separated using '|' as a delimiter
 * * exec_pipe (in pipes.c) will be called on each substring
 * * If file redirection has occurred (checked in the redirect variable), then they will be restored to the 
 *   normal stdin, stdout, and/or sterr file descriptors
 * * If exit was called without any pipes, status will have been sest to zero, and the program will exit
 * * Otherwise, the process() loop called in the main function will keep going.
 * ====================*/

/*======== int main() ==========
 * Inputs: none
 * Returns: 0.
 *
 * Runs main processing method, process().
 * ====================*/
```
### debug.h
Defines the DEBUG variable for debugging purposes

### shell_name.h
Defines the name of the SHELL (i.e. SASH)
