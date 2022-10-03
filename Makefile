all:
	compile

clean:
	rm ./main

compile:
	g++ ./src/db/*.cpp -o main
