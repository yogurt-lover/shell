#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>

#define NRML "\x1B[0m"
#define CYAN "\x1B[36m"
#define BOLD "\x1B[1m"
#define DEBUG 0

// signals (Ctrl-C)
// resizing buffers
// redirection < | >

char *get_home_dir() {
  struct passwd *pw = getpwuid(getuid());
  return pw->pw_dir;
}

char *get_username() {
  struct passwd *pw = getpwuid(getuid());
  return pw->pw_name;  
}

char *get_hostname() {
  char *hostname = (char *)malloc(256*sizeof(char));;
  gethostname(hostname, 256);
  return hostname;
}

char *get_cwd() {
  char *cwd = (char *)malloc(256*sizeof(char));
  getcwd(cwd, 256);
  return cwd;
}

void print_prompt() {
  char *cwd = get_cwd();
  char *username = get_username();
  char *hostname = get_hostname();
  char *home = get_home_dir();
  if (strstr(cwd, home)) {
    char *s = cwd;
    printf("%s%s%s@%s:~%s%s\n$ ", CYAN, BOLD, username, hostname, s+strlen(home), NRML );
  }
  else {
    printf("%s%s%s@%s:%s%s\n$ ", CYAN, BOLD, username, hostname, cwd, NRML);
  }
  free(hostname);
  free(cwd);
}

char *read_raw() {
  char *buff = (char *)malloc(256*sizeof(char));

  
  char c;
  int counter = 0;

  c = getchar();
  for (; c != '\n' && c != EOF; counter++) {
    buff[counter] = c;
    c = getchar();
  }
  buff[counter] = '\0';
  if (c == EOF) printf("\n");
  return buff;
  
  
  //fgets(buff, 256, stdin);
  //return buff;
}

char **get_args(char *input, int *num_args) {
  char **args = (char **)malloc(256*sizeof(char*));
  char *arg = strtok(input, " \n\r\t");
  int counter;
  for (counter = 0; arg; counter++) {
    args[counter] = arg;
    arg = strtok(NULL, " \n\r\t");
  }
  args[counter] = arg;
  *num_args = counter;

  return args;
}

int execute(char **args, int *num_args) {
  // whitespace or no characters at all
  if (!args[0]) return 1;
  
  // cd (might put this someplace else)
  if (!strcmp(args[0], "cd")) {
    if (args[1]) {
      if (chdir(args[1])) {
	printf("%s\n", strerror(errno));
      }
    }
    else {
      char *home = get_home_dir();
      chdir(home);
    }
    return 1;
  }
  
  // exit (this too)
  if (!strcmp(args[0], "exit")) {
    return 0;
  }
  
  // system binaries
  int f = fork();
  if (f == 0) {

    // colorized ls (Doesn't work in BSD)
    /*
    if (!strcmp(args[0], "ls")) {
      args[*num_args] = "--color=auto";
      *num_args += 1;
      args[*num_args] = NULL;
    }
    */

    if (execvp(args[0], args) == -1) {
      printf("%s: command not found\n", args[0]);
    }
  }
  else {
    wait(NULL);
  }
  return 1;
}

int process() {
  int status;
  char *raw_input;
  
  print_prompt(); 
  raw_input = read_raw();
  if (DEBUG) printf("raw_input: ~~~%s~~~\n", raw_input);
  
  char *s = raw_input;
  while (s) {
    int num_args;
    char *single_input;
    char **args;

    single_input = strsep(&s, ";");
    if (DEBUG) printf("single_input:~~~%s~~~\n", single_input);
    args = get_args(single_input, &num_args);

    if (DEBUG) {
      printf("----------------------------\n");
      printf("ARGUMENTS\n");
      int k = 0;
      for (; k < num_args; k++) {
	printf("arg: ~~~%s~~~\n", args[k]);
      }
      printf("----------------------------\n");
    }
    
    status = execute(args, &num_args);
    free(args);
  }
  free(raw_input);
  return status;
}

int main() {
  int status = 1;
  while (status) {
    status = process();
    if (DEBUG) printf("status: %d\n", status);
  }
  return 0;
}
  
