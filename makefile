default: shell

shell: shell.c
	gcc -o shell shell.c -Wall

run: shell
	./shell

clean:
	-rm *~
	-rm shell
	-rm a.out
