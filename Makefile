all: main

CFLAGS=-O2 -Wall -Wconversion

main: main.cpp
	clang++ $(CFLAGS) main.cpp -o main

clean:
	rm -rf main
