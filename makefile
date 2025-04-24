CC=gcc
CFLAGS=-Wall -Wextra -g
OBJ=main.o process.o

psinfo: $(OBJ)
	$(CC) $(CFLAGS) -o psinfo $(OBJ)

main.o: main.c process.h
	$(CC) $(CFLAGS) -c main.c

process.o: process.c process.h
	$(CC) $(CFLAGS) -c process.c

clean:
	rm -f *.o psinfo
