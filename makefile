#compile the program without the UCS files
all: Schedule.hpp main.cpp
	g++ Schedule.hpp main.cpp

#compile the program and run it
#ucs files will be added later when they're fixed
#pretty confident that the program will run pretty fast so just stop it after 1 seconds
#some intput will cause an infinite loop anyways
test: Schedule.hpp main.cpp
	g++ Schedule.hpp main.cpp
	timeout 1s ./a.out
	more solution.xml

#clean the gch file, a.out file and the solution
clean:
	rm *.gch a.out solution.xml