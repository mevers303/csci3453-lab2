all: clean
	gcc -o a.out src/processes.c src/pcb.c src/main.c

clean:
	rm a.out
