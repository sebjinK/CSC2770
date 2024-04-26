all: client

client:
	g++ -g -Wall -c -o fclient.o fclient.cpp
	g++ -g -Wall -c -o fclient.o fclient.cpp

clean:
	rm -f *.o fclient hello.txt

run:
	./fclient localhost 8007 hello.txt
	