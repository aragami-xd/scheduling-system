#compile the program without the UCS files
all: Schedule.hpp main.cpp
	g++ Schedule.hpp main.cpp

#compile the program and run it
#ucs files will be added later when they're fixed
#the program will timeout in 59s and the solution will be printed out onto the console
test: Schedule.hpp main.cpp
	g++ Schedule.hpp main.cpp
	./a.out
	more solution.xml

#clean the gch file, a.out file and the solution
clean:
	rm *.gch a.out solution.xml