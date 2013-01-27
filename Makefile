CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=OptionParser.cpp main.cpp PlayField.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=redbot

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -pthread -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -pthread

PlayField.o: PlayField.cpp
	$(CC) $(CFLAGS) -c PlayField.cpp

OptionParser.o: OptionParser.cpp
	$(CC) $(CFLAGS) -c OptionParser.cpp

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
