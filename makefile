
all: main.exe

main.exe: main.o
	g++ -std=c++17 -o main.exe main.o -pthread

#main.o: main.cpp
#	g++ -std=c++17 -c main.cpp -pthread

clean: rm main.o main.exe

