all:
	g++ -Wall -c emisor.cpp -lwiringPi
	g++ -Wall -c receptor.cpp -lwiringPi
	g++ -Wall -c protocol/protocol.cpp
	g++ -Wall -c helpers/helpers.cpp
	g++ -Wall -o emisor emisor.o protocol.o helpers.o
	g++ -Wall -o receptor receptor.o protocol.o helpers.o
