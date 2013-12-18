###########################
# Makefile - RedBot     
# Tým F#ntom
# Pavel Grunt (FIT VUT, Brno), Tomáš Pastirčák (FI MUNI, Brno)

FILE = "F\#ntom"
ZIPNAME = $(FILE)

# Makefile pravidla
.PHONY= all clean pack run

all: $(FILE)

$(FILE): Program.fs
	fsharpc -O Program.fs
	cp $(FILE).sh $(FILE)
	chmod +x $(FILE)

# smazani
clean:
	rm -f *.exe $(FILE)

run: $(FILE)
	./$(FILE)

# zabaleni
pack: clean
	zip $(ZIPNAME).zip Makefile *.fs $(FILE).sh

