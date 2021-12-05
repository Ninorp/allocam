all: mymalloc.o
	gcc -o main main.c mymalloc.o

mymalloc.o:
	gcc -o mymalloc.o -c mymalloc.c

clean:
	rm -rf *.o main