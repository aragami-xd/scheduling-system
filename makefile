all: Schedule.hpp EvalUCS.hpp EvalUCS.cpp main.cpp
	g++ Schedule.hpp EvalUCS.hpp EvalUCS.cpp main.cpp

clean:
	rm *.gch a.out