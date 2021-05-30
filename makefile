all:
	g++ -Wall -c emisor.cpp -lwiringPi
	g++ -Wall -c receptor.cpp -lwiringPi
	g++ -Wall -c protocol/protocol.cpp
	g++ -Wall -c helpers/helpers.cpp
	g++ -Wall -o emisor emisor.o protocol.o helpers.o -lwiringPi
	g++ -Wall -o receptor receptor.o protocol.o helpers.o -lwiringPi
test:
	g++ -Wall -c main.cpp
	g++ -Wall -c protocol/protocol.cpp
	g++ -Wall -c helpers/helpers.cpp
	g++ -Wall -c menu/menu.cpp
	g++ -Wall -o test main.o protocol.o helpers.o menu.o
clean:
	rm *.o
