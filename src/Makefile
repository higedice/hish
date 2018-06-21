CC = gcc
CFLAGS = -O2

all: hish

hish: main.o builtin_commands.o prompt.o
	${CC} -o hish main.o builtin_commands.o prompt.o

main.o: main.c
	${CC} ${CFLAGS} -c -o main.o main.c

builtin_commands.o: builtin_commands.h builtin_commands.c
	${CC} ${CFLAGS} -c -o builtin_commands.o builtin_commands.c

prompt.o: prompt.h prompt.c
	${CC} ${CFLAGS} -c -o prompt.o prompt.c

clearn:
	rm -f *.o
