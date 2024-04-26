all: client

client:
	g++ -g -Wall -c -o fclient.o fclient.cpp
	g++ -g -Wall fclient.o -o fclient

clean:
	rm -f *.o fclient *.txt

run:
	./fclient localhost 8007 hello.txt 
	