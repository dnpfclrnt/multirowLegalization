all: compile

clean:
	rm ./main

compile:
	g++ -O3 ./src/db/*.cpp ./src/lg/*.cpp ./src/*.cpp ./main.cpp  -o main
