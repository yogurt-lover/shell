default: shell

shell: shell.o parse.o prompt.o redirect.o pipes.o exec.o
	gcc -o shell shell.o parse.o prompt.o redirect.o pipes.o exec.o -Wall

shell.o: shell.c
	gcc -o shell.o -c shell.c

parse.o: parse.c
	gcc -o parse.o -c parse.c

prompt.o: prompt.c
	gcc -o prompt.o -c prompt.c

redirect.o: redirect.c
	gcc -o redirect.o -c redirect.c

pipes.o: pipes.c
	gcc -o pipes.o -c pipes.c

exec.o: exec.c
	gcc -o exec.o -c exec.c

run: shell
	./shell

clean:
	-rm *~ shell a.out *.o
