default: shell

shell: shell.o parse.o prompt.o
	gcc -o shell shell.o parse.o prompt.o  -Wall

shell.o: shell.c
	gcc -o shell.o -c shell_one_pipe.c

parse.o: parse.c
	gcc -o parse.o -c parse.c

prompt.o: prompt.c
	gcc -o prompt.o -c prompt.c

run: shell
	./shell

clean:
	-rm *~
	-rm shell
	-rm a.out
	-rm *.o
