all: Schedule.cpp main.cpp
	g++ -std=c++11 Schedule.cpp main.cpp

clean:
	rm *.gch a.out