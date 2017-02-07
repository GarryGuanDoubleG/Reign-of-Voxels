CXX=clang++
CXXFLAGS=-g -std=c++11 -Wall -pedantic

LIBS= -lsfml-network

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: server.o json.o
	g++ -o server server.o json.o $(LIBS)

json.o: json.hpp
	g++ -c json.hpp
	
server.o: server.cpp
	g++ -c server.cpp
	
clean:
	rm *.o server