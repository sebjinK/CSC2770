all: server

server:
	cd fserver && g++ -g -Wall -c -o fserver.o fserver.cpp
	cd fserver && g++ -g -Wall fserver.o -o fserver
clean:
	rm -f *.o fserver

run:
	./fserver 8007