CC = gcc
CFLAGS = -O2
RM = rm -f

all: hish

hish: main.o builtin_commands.o prompt.o
	${CC} -o hish main.o builtin_commands.o prompt.o

main.o: main.c hish.h
	${CC} ${CFLAGS} -c -o main.o main.c

builtin_commands.o: builtin_commands.h builtin_commands.c hish.h
	${CC} ${CFLAGS} -c -o builtin_commands.o builtin_commands.c

prompt.o: prompt.h prompt.c
	${CC} ${CFLAGS} -c -o prompt.o prompt.c

.PHONY: clean
clean:
	${RM} *.o
