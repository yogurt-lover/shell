#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PROMPT "$ "


void process() {
  char input[100];
  printf(PROMPT);
  fgets(input, sizeof(input), stdin);
  input[strlen(input)-1] = 0; //remove newline

  char *s = input;
  while (s) {
    char *single_input = strsep(&s, ";");
    
    char *command_args[100];
    char *s2 = single_input;
    
    int i = 0;
    while (s2) {
      command_args[i] = strsep(&s2, " ");
      i++;
    }
    command_args[i] = 0;
  
    int f = fork();
    if (f == 0) {
      printf("k\n");
      execvp(command_args[0], command_args);
    }
    else {
      wait();
    }
  }
}

int main() {
  while (1) {
    int f = fork();
    if (f == 0) {
      process();
      return 0;
    }
    else {
      wait();
    }
  }
  return 0;
}
