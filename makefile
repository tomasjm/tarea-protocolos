all:
	g++ -Wall -c main.cpp
	g++ -Wall -c protocol/protocol.cpp
	g++ -Wall -c helpers/helpers.cpp
	g++ -Wall -o program main.o protocol.o helpers.o
