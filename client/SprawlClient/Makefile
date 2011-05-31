OBJECTS = main.o SprawlNet.o

SprawlClient.bin: $(OBJECTS)
	g++ $(OBJECTS) -o ../SprawlClient.bin

main.o: main.cpp
	g++ -c main.cpp

SprawlNet.o: SprawlNet.cpp
	g++ -c SprawlNet.cpp