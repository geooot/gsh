build:
	mkdir -p build
gsh: build build/functions.o build/main.o
	g++ -g build/functions.o build/main.o -o build/gsh
test: build/functions.o build/test.o
	g++ -g build/functions.o build/test.o -o build/test

build/test.o: build test.cpp
	g++ -g -c test.cpp -o build/test.o
build/functions.o: build functions.cpp functions.h
	g++ -g -c functions.cpp -o build/functions.o
build/main.o: build main.cpp
	g++ -g -c main.cpp -o build/main.o

all:
	make gsh
	make test
clean: 
	rm build/*
