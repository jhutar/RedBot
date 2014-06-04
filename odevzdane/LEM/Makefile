# RedBot - jaro 2014
# Pavel Grunt
# xgrunt01@stud.fit.vutbr.cz


CXXFLAGS=-std=c++11 -O3 #-Wall -Wextra -pedantic 

all: LEM

LemSVM.o: Lem.h Lem.cpp SVM.h
	$(CXX) $(CXXFLAGS) -DSVMCLASSIFIER -c -o $@ Lem.cpp	

LEM: LemSVM.o main.o SVM.o
	$(CXX) -o $@ -O3 LemSVM.o main.o SVM.o

clean:
	rm -rf LEM *.o

pack:
	zip LEM.zip *.cpp *.h Makefile
