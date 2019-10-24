#include "EvalUCS.hpp"

// static variables.
// ***********************************
int ProblemUCS::rooms;                  // Teaching rooms
int ProblemUCS::mC;                     // Number of courses
vector<int> ProblemUCS::cHours;         // hours per course
vector<string> ProblemUCS::cNames;      // names of courses
int ProblemUCS::nL;                     // number of lecturers
vector<string> ProblemUCS::lNames;      // names of lecturers
IntMatrix ProblemUCS::TL;
IntMatrix ProblemUCS::LP;
IntMatrix Solution::Timetable;

// simple driver program 
int main(int argc, char* argv[]) {

  if(argc !=3){
    cerr << "usage: ./Eval ucs_problem_filename   ucs_solution filename" 
	 << endl;
    exit(EXIT_FAILURE);
  }
  
  string ucsFileName=string(argv[1]);
  string ucsSolnFileName=string(argv[2]);
    
  ProblemUCS::readUCSInstance(ucsFileName);;
  double res ;   // place holder for result


    cout << "calling readSolution " << endl;
  // if a feasible solution can be read
   if ( Solution::readSolution(ucsSolnFileName)) {
 
    cout << "calling constrains " << endl;
    int constrains = Solution::checkConstraints(Solution::Timetable, ProblemUCS::rooms, ProblemUCS::cHours, ProblemUCS::LP, ProblemUCS::cNames, ProblemUCS::lNames);
    if (constrains == 0)
         cout << "your solution is feasible" << endl;
     else 
         cout << "your solution is not feasible" << endl;
    
      res =  Solution::getFitnessValue(Solution::Timetable, ProblemUCS::mC,ProblemUCS::LP,ProblemUCS::cHours,constrains);
      cout << "Fitness value " <<  res << endl;
    }
    else { 
          cout << "Solution file is not readable " << endl;
    }

}


