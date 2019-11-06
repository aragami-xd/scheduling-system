#compile the program
all: Schedule.hpp EvalUCS.o Solution.o main.cpp
	g++ Schedule.hpp EvalUCS.o Solution.o main.cpp

#compile the program and run it
test: Schedule.hpp EvalUCS.o Solution.o main.cpp
	g++ Schedule.hpp EvalUCS.o Solution.o main.cpp
	./a.out

#compile the EvalUCS.o file
EvalUCS.o: EvalUCS.h EvalUCS.cpp
	g++ -c EvalUCS.h EvalUCS.cpp

#compile the Solution.o file
Solution.o: Solution.h Solution.cpp
	g++ -c Solution.h Solution.cpp

#clean the gch file, a.out file and the solution
clean:
	rm *.gch *.o *.hpp.gch a.out 