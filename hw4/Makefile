all: server client

client:
	cd fclient && g++ -g -Wall -c -o fclient.o fclient.cpp
	cd fclient && g++ -g -Wall fclient.o -o fclient

server: 
	cd fserver && g++ -g -Wall -c -o fserver.o fserver.cpp
	cd fserver && g++ -g -Wall fserver.o -o fserver

clean:
	cd fclient && rm -f *.o fclient
	cd fserver && rm -f *.o fserver

run:
	cd fserver && ./fserver 8007
	cd fclient && ./fclient localhost 8007 hello.txt

