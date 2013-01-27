CC=gcc
CFLAGS=-Wall
LDFLAGS=
SOURCES=farmari.c  utocnici40_rand.c  utocnici50_rand.c  utocnici_o1.c  utocnici_rand.c spioni.c lupici_rand.c
EXECUTABLE=farmari utocnici40_rand utocnici50_rand  utocnici_o1  utocnici_rand spioni lupici_rand

all: $(EXECUTABLE)

spioni: spioni.c
	$(CC) $(CFLAGS) spioni.c -o $@

farmari: farmari.c
	$(CC) $(CFLAGS) farmari.c -o $@

utocnici40_rand: utocnici40_rand.c
	$(CC) $(CFLAGS) utocnici40_rand.c -o $@

utocnici50_rand: utocnici50_rand.c
	$(CC) $(CFLAGS) utocnici50_rand.c -o $@

utocnici_rand: utocnici_rand.c
	$(CC) $(CFLAGS) utocnici_rand.c -o $@

utocnici_o1: utocnici_o1.c
	$(CC) $(CFLAGS) utocnici_o1.c -o $@

lupici_rand: lupici_rand.c
	$(CC) $(CFLAGS) lupici_rand.c -o $@

clean:
	rm -rf  $(EXECUTABLE)
