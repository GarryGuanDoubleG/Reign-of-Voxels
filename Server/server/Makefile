C=clang++
CFLAGS=-g -std=c++11 -Wall -pedantic

LIBS= -lenet

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: server.o 
	g++ -o server server.o $(CLFAGS) $(LIBS)


server.o: server.cpp json.hpp
	g++ -c server.cpp json.hpp $(CFLAGS)
	
clean:
	rm *.o server
