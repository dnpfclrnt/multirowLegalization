all: linux 

clean:
	rm ./main

linux:
	g++ -O3 ./src/db/*.cpp ./src/lg/*.cpp ./src/*.cpp ./main.cpp  -o main
