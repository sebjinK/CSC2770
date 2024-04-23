all: fserver fclient

SERVER=fserver
CLIENT=fclient

SERVERCPP=$(SERVER).cpp
SOBJS=$(SERVERCPP:.cpp=.o)

CLIENTCPP=$(CLIENT).cpp
COBJS=$(CLIENTCPP:.cpp=.o)

CXX=g++
CC=g++

CXXFLAGS = -g -Wall
LDFLAGS=-g

$(SERVER) : $(SOBJS)

$(CLIENT) : $(COBJS)
PHONY: clean run
clean:
	rm $(SERVER) $(CLIENT) $(SOBJS) $(COBJS)


