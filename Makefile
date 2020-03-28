main: main.cpp cache.hpp nameof.hpp replace.hpp trace.hpp utility.hpp Makefile
	g++ -O2 -std=c++17 -o main main.cpp

run: main
	./main

clean:
	rm -rf *.o main

clean_log:
	rm -rf log/*