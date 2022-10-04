all: compile

clean:
	rm ./main

compile:
	g++ ./src/db/*.cpp ./main.cpp  -o main
