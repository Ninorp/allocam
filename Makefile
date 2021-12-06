all: 
	gcc -o lib/mymalloc.o -c lib/mymalloc.c
	gcc -o test/test.o -c test/test.c
	gcc -o main main.c lib/mymalloc.o test/test.o

clean:
	rm -rf main */*.o