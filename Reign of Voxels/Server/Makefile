CC=gcc
CFLAGS=-I.

DEPS = json.hpp
LIBS= -lsfml-network

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: server.o json.o
	g++ -o server server.o json.o -I $(LIBS)
clean:
	rm *.o server