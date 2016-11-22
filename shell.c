#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define PROMPT "$ "


void process() {

  char raw_input[100];
  printf(PROMPT);
  fgets(raw_input, sizeof(raw_input), stdin);
  raw_input[strlen(raw_input)-1] = 0; //remove newline

  char *s1 = raw_input;
  while (s1) {
    char *single_input = strsep(&s1, ";");

    char *command_args[100];
    char *s2 = single_input;

    int i = 0;
    while (s2) {
      command_args[i] = strsep(&s2, " ");
      i++;
    }
    command_args[i] = 0;

    //remove command_args elements with "\0" 
    
    //
    
    int f = fork();
    if (f == 0) {
      if (execvp(command_args[0], command_args) == -1) {
	printf("%s: command not found\n", single_input);
      }
    }
    else {
      wait(NULL);
    }
    
  }
}

int main() {
  while (1) {
    process();
  }
  return 0;
}
  
