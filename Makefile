all: compile

clean:
	rm ./main

compile:
	g++ ./src/db/*.cpp ./src/*.cpp ./main.cpp  -o main
