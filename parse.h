#ifndef _PARSEH_
#define _PARSEH_
/*======== char *read_raw() ==========
 * Inputs: none
 * Returns: Dynamically allocated string from terminal input
 *
 * Dynamically allocates a buffer for use throughout the shell
 * If Ctrl-D is pressed, the program exits
 * ====================*/
char *read_raw(); 
/*======== char **get_args() ==========
 * Inputs: char *input
 *         int *num_args
 * Returns: Array of strings, where each entry is a whitespace separated token of the user input
 *
 * Dynamically allocates a buffer for use throughout the shell
 * If an argument is "~", it gets replaced with the home directory.
 * ====================*/
char **get_args(char *input, int *num_args);
#endif
