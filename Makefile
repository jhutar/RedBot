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

OptionParser.o: OptionParser.cpp OptionParser.h
	$(CC) $(CFLAGS) -c OptionParser.cpp

PlayField.o: PlayField.cpp PlayField.h
	$(CC) $(CFLAGS) -c PlayField.cpp

main.o: main.cpp PlayField.o OptionParser.o
	$(CC) $(CFLAGS) -c main.cpp -pthread

gui: gui.c
	$(G) `pkg-config --cflags gtk+-2.0` -o $@ gui.c `pkg-config --libs gtk+-2.0`

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) gui
	
