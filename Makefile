G=gcc
CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=OptionParser.cpp PlayField.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=redbot gui
GTKLIBS=-lgtk-3 -lgdk-3 -lgtk-3.0 -lgdk3.0
GTKCFLAGS=-pthread -I/usr/include/gtk-3.0 -I/usr/include/atk-1.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/pango-1.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/pixman-1 -I/usr/include/libpng15 -I/usr/include/libdr

all: $(SOURCES) $(EXECUTABLE) gui.c
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -pthread -o $@

OptionParser.o: OptionParser.cpp
	$(CC) $(CFLAGS) -c OptionParser.cpp

PlayField.o: PlayField.cpp
	$(CC) $(CFLAGS) -c PlayField.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -pthread

gui: gui.c
	$(G) `pkg-config --cflags gtk+-3.0` -o $@ gui.c `pkg-config --libs gtk+-3.0`

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) gui
