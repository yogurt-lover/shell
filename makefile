default: shell

executor: shell.c
	gcc -o shell shell.c

run: shell
	./shell

clean:
	-rm *~
	-rm shell

