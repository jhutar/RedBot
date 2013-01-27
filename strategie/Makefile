CC=gcc
CFLAGS=-Wall
LDFLAGS=
SOURCES=farmari.c  lupici.c  utocnici.c 
EXECUTABLE=farmari lupici utocnici

all: $(EXECUTABLE)

lupici: lupici.c
	$(CC) $(CFLAGS) lupici.c -o $@

farmari: farmari.c
	$(CC) $(CFLAGS) farmari.c -o $@

utocnici: utocnici.c
	$(CC) $(CFLAGS) utocnici.c -o $@

clean:
	rm -rf  $(EXECUTABLE)
