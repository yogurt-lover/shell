#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PROMPT "$ "

int main() {
      
  char input[100];
  printf(PROMPT);
  fgets(input, sizeof(input), stdin);
  input[strlen(input)-1] = 0; //remove newline

  char * command[100];
  char *s = input;
  char *p;

  int i = 0;
  while (s) {
    command[i] = strsep(&s, " ");
    i++;
  }
  command[i] = 0;
  
  execvp(command[0], command);
  return 0;
}
