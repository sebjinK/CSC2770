# all: fserver fclient

# SERVER=fserver
# CLIENT=fclient

# SERVERCPP=$(SERVER).cpp
# SOBJS=$(SERVERCPP:.cpp=.o)

# CLIENTCPP=$(CLIENT).cpp
# COBJS=$(CLIENTCPP:.cpp=.o)

# CXX=g++
# CC=g++

# CXXFLAGS = -g -Wall
# LDFLAGS=-g

# $(SERVER) : $(SOBJS)

# $(CLIENT) : $(COBJS)


# PHONY: clean run
# clean:
# 	rm $(SERVER) $(CLIENT) $(SOBJS) $(COBJS)


client: fclient.cpp
	g++ -g -Wall -c -o ./fclient/fclient.o ./fclient/fclient.cpp
	g++ -g -Wall ./fclient/fclient.o -o fclient/fclient

server: fserver.cpp
	g++ -g -Wall -c -o ./fserver/fserver.o ./fserver/fserver.cpp
	g++ -g -Wall ./fserver/fserver.o ./fserver/fserver

clean:
	rm -f fserver/fserver
	rm -f fserver/fserver.o
	rm -f fclient/fclient
	rm -f fclient/fclient.o

run:
	./fserver/fserver 8007
	./fclient/fclient localhost 8007 hello.txt

