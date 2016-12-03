# SASH (The Seiji-Anton shell)

## Features

A lovely shell capable of executing commands in one's PATH.  
Can be used very similarly to your standard Bash shell.  
* Presents your host name and current working directory in a beautiful light blue.
* Will fork and execute commands.
* Support for multiple commands separated by ";"
* Capable of of redirection, including "<", ">", ">>", "&>", "&>>", "2>", "2>>", "1>", and "1>>". Also supports pipes! "|"
* You can put as many spaces between flags and commands as you like!

## Attempted
* Did create a colorized "ls" originally, using default colors per operating system, however was dropped in final version due to reorganization of methods and lack to time to re-implement.
* ~/[nextdir] Does not work. However, ~ itself does work as home.

## Bugs

## Files & Function Headers
### prompt.c
### parse.c
### exec.c
### redirect.c
### pipes.c
