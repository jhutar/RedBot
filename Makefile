CFLAGS=-Wall
LDFLAGS=
SOURCES=OptionParser.cpp PlayField.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=redbot

all: $(SOURCES) $(EXECUTABLE) gui
	
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(OBJECTS) -pthread -o $@

OptionParser.o: OptionParser.cpp
	$(CXX) $(CFLAGS) -c OptionParser.cpp

PlayField.o: PlayField.cpp
	$(CXX) $(CFLAGS) -c PlayField.cpp

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp -pthread

gui: gui.c
	$(CC) `pkg-config --cflags gtk+-2.0` -o $@ gui.c `pkg-config --libs gtk+-2.0`

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) gui
	
