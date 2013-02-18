G=gcc
CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=OptionParser.cpp PlayField.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=redbot

all: $(SOURCES) $(EXECUTABLE) gui
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -pthread -o $@

OptionParser.o: OptionParser.cpp
	$(CC) $(CFLAGS) -c OptionParser.cpp

PlayField.o: PlayField.cpp
	$(CC) $(CFLAGS) -c PlayField.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -pthread

gui: gui.c
	$(G) `pkg-config --cflags --libs gtk+-2.0` -o $@ gui.c

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) gui
	
