all:
	g++ -Wall -c emisor.cpp -lwiringPi
	g++ -Wall -c receptor.cpp -lwiringPi
	g++ -Wall -c protocol/protocol.cpp
	g++ -Wall -c helpers/helpers.cpp
	g++ -Wall -c menu/menu.cpp
	g++ -Wall -o emisor emisor.o menu.o protocol.o helpers.o -lwiringPi
	g++ -Wall -o receptor receptor.o menu.o protocol.o helpers.o -lwiringPi
clean:
	rm *.o
