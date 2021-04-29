CC = gcc
EXES = main
OBJS = main.o server.o client.o client_list.o task_queue.o utils.o
CFLAGS = -g -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wall -Werror -Iinclude 
LDFLAGS = -pthread -lm -lcurses

all: $(EXES) 

main: main.o utils.o server.o client.o client_list.o task_queue.o
		$(CC) $(CFLAGS) -o main main.o utils.o server.o client.o client_list.o task_queue.o $(LDFLAGS)

main.o: src/main.c
		$(CC) -c $(CFLAGS) $< -o $@

server.o: src/server.c
		$(CC) -c $(CFLAGS) $< -o $@

client.o: src/client.c
		$(CC) -c $(CFLAGS) $< -o $@

client_list.o: src/client_list.c
		$(CC) -c $(CFLAGS) $< -o $@

task_queue.o: src/task_queue.c
		$(CC) -c $(CFLAGS) $< -o $@

utils.o: src/utils.c
		$(CC) -c $(CFLAGS) $< -o $@

clean: 
		rm -rf $(OBJS) $(EXES) *.dSYM
