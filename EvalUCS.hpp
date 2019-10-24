#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <bitset>
#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <sstream>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <climits>
 
using namespace std;
 
const int maxsize=100000+5;
vector<string> split(const string& s, char delim);
vector<int> splitInt(const string& s, char delim);
template <typename T> bool parseField(string const& match, string input, T& output);

// template from Mike Seymour 
// (http://stackoverflow.com/questions/14590410/stoi-and-stoll-in-c)
template <typename T> T ston(string const & s) {
    stringstream ss(s);
    T result;
    ss >> result;  
    return result;
}


typedef vector<vector<int> > IntMatrix;

class ProblemUCS{
public:
  static int rooms;		  // number of teaching rooms
  static int mC;             // total number of courses to schedule
  static vector<int> cHours  ;     // hours per course
  static vector<string> cNames;    // Course names  
  static int  nL;                   // number of lecturers 
  static vector<string> lNames;    // Course names  
  static IntMatrix   TL;
  static IntMatrix   LP;
  
  
  // function to read in UCS problem specification from a file
  static  bool readUCSInstance(string fileName){
    string input;
    ifstream sr(fileName.c_str());
    if (! sr.is_open()) return false;

    bool status=getline(sr,input);

     // match on keyword read in number, assign to var and continue;
     if(parseField<int>("Rooms",input,rooms)){
	cout << "r: " << rooms << endl;
      }else{
        // there was a problem reading... report and return
        cerr << "inconsistent file - first line should have Rooms" << endl;
        return false;
      }
	
     // read second line
      status=getline(sr,input);

      if(parseField<int>("Courses",input,mC)){
	cout << "mCourses: " << mC << endl;
	;
      }else{
        // there was a problem reading... report and return
        cerr << "inconsistent file - second line should have course number" << endl;
        return false;
      }

    
    // now we get the line with the course hours
    status=getline(sr,input);  // read the line
    
    string hours, next; 
    hours = replaceGeneric(input, "Hours", "");
 
    // used for breaking words 
    stringstream ss;
    ss << hours;

    while (getline(ss, next, ',' )) { 
       cHours.push_back(stoi(next)); 
    }

    cout << "hours read" << endl;
    for (int i=0;i<mC;i++) 
            cout << cHours[i] << " ";
    cout << endl;

    // now we get the line with the course names
    status=getline(sr,input);  // read the line
   
    ss.clear(); 
    ss.str(input);
    ss >> next;
    
    while (getline(ss, next, ',' )) { 
       cNames.push_back(removeDoubleSpace(next)); 
    }

    cout << "names read" << endl;
    for (int i=0;i<mC;i++) 
	cout << cNames[i] <<  endl;

     // read lecture numbers 
     status=getline(sr,input);

     if(parseField<int>("Lecturers",input,nL)){
	cout << "lecturers: " << nL << endl;
	;
      }else{
        // there was a problem reading... report and return
        cerr << "inconsistent file - second line should have course number" << endl;
        return false;
      }

    // now we get the line with lecturers names
    status=getline(sr,input);  // read the line
   
    ss.clear(); 
    ss.str(input);
    
    while (getline(ss, next, ',' )) { 
       lNames.push_back(removeDoubleSpace(next)); 
    }

    cout << "lecture names read" << endl;
    for (int i=0;i<nL;i++) 
	cout << lNames[i] <<  endl;


    status=getline(sr,input);  // read the  comment line for TL 
    //first, resize TL to be mC x nL 
    TL.resize(mC, vector<int>(nL, 0)); 
    
    // read the TL matrix, one row at at time 
    for (int i=0;i<mC;i++)  {
         status=getline(sr,input);  // read the line
         ss.clear();  
         ss.str(input);
         int j = 0;
         while (getline(ss, next, ',' )) { 
             TL[i][j] = stoi(next);
             j++;          
         }
    }

    status=getline(sr,input);  // read the  comment line for LP  
    //first, resize LP to be  nL x 40  
    LP.resize(nL, vector<int>(40, 0)); 

    // read the LP matrix, one row at at time 
    for (int i=0;i<nL;i++)  {
         status=getline(sr,input);  // read the line
         ss.clear();  
         ss.str(input);
         int j = 0;
         while (getline(ss, next, ',' )) { 
             LP[i][j] = stoi(next);
             j++;
         }
     }

    cout << "LP  read" << endl;
    for (int i=0;i<nL;i++) {
       for (int j=0;j<40;j++) 
	cout << LP[i][j] <<  " ";
    cout << endl;
    }

    return true;
  }

    

  static string replaceGeneric(string s, 
			       const string& search, 
			       const string& replace){
    int pos;
    int len;
    string result=s;
    len=search.length();
    pos=result.find(search);
    while(pos!=string::npos){
      result.replace(pos,len,replace);
      pos=result.find(search);
    }
    return result;
  }
    

  static string removeDoubleSpace(string s){
    return replaceGeneric(s,"  "," ");
  }

  
};

class Solution{
public:
  static IntMatrix  Timetable;
  
  static bool readSolution(string fileName){

    //  file format is Timetable.csv 
    string input;   // input string to hold line
    ifstream sr(fileName.c_str());   // open the file

    if (! sr.is_open()) return false;  // file can't open then exit
 
    int courses = ProblemUCS::mC;

    //first, resize Timetable to be  mC x 40  
    Timetable.resize(courses, vector<int>(40, -1)); 

    bool status;
    // read the allocation matrix, one row at at time 
    
    stringstream ss;
  try{
    for (int i=0;i< courses;i++)  {
       //cout << "line" << i << endl;
       status = getline(sr,input);     // file can open grab the first line
       ss.str(input);
       ss.clear();  
       int j = 0;
       string next;
       while (getline(ss, next, ',' )) { 
             Timetable[i][j] = stoi(next);
             j++;
         }
         if (j != 40)  
             cout << j << endl;
         
     }

    cout << "timetable"  << endl;
    for (int i=0;i<courses;i++)  {
      for (int j=0;j<40;j++)  
            cout <<  Timetable[i][j]  << " " ;
      cout << endl;
    }

  }catch(int e){
      // exception in processing return false
      printf(" exception caught \n" );
      return false;
       }

    cout << "read retruns true" << endl;
    return true;

    }


    
  static int checkConstraints(vector < vector <int>>  solution, int rooms, vector <int> hoursperCourse, vector < vector <int>>  LP, vector <string> cNames, vector <string>  lNames){
        //violated constraint 
        //1. No classes allocted at lunch break 12-1 (done)
        //2. A lecture can not have a separated session in the same day (done)
        //3. No more than two hours per lecture in the same day (done)
        //4. at the same time no more lectures than rooms (done)
        //5. Don't arrange a lecture when a lecturer is unavailabe (done)
        //6. NO lecture can teach more than two consecutive periods without break
        //
        int hoursWeek=40;
        int il,il2;//indice the lecturer
        int failedConstraints=0;

        //Based on the solution we fill busyRooms to verified if is not allocated at the 
        //same hour more than one course //always be the same 
       
        vector <int>  busyRooms(hoursWeek,0);

        int courses = cNames.size();
        int nl = lNames.size();
        int day=5;

        //To know how many hours per day a Lecturer works
         vector < vector <int>>   lectDay;
         lectDay.resize(nl, vector<int>(day,0));
        

        int hoursday=0;
        for (int i = 0; i < courses; i++) {
            day=0;
            for (int j = 0; j < hoursWeek; j++) {
                il=solution[i][j];//indice lecturer 
                if(il!=-1){
                    //Lunch constraint  positions 3,11,19,27,35
                    if(j%8 == 3) {
                        cout << "Constraint Violation: class  for lecturer" << lNames[il] <<  " is allocated at lunch break" << endl;
                        failedConstraints++;
                    }                    
                    
                    busyRooms[j]++;
                    //more classes allocated at the same time  than the number of rooms 
                    if(busyRooms[j]>rooms){
                        cout << "Constraint Violation: More classes than rooms at the same hour, day " << day <<  "hour " << j%8 << endl;
                        // printf("Constraint Violation: More classes than rooms at the same hour, day %d, hour %d \n", day, j%8 );
                        failedConstraints++;
                    }
                    
                    //sum the hour in the lecturers array 
                    //Check that the lecturer is available in his preferences 
                    if(LP[il][j]==0) { //means the lecturer isn't available    
                        cout << "Constraint Violation: lecturer" << lNames[il] <<  " was assigned to course " << cNames[i] <<  " in a busy slot "  << endl;
                        // printf("Constraint Violation:  lecturer %s was assigned to course %s in a busy slot \n", lNames[il], cNames[i]);
                        //failedConstraints++;                        
                    }
                    
                    lectDay[il][day]++;//increase the hour 
                        //cout << " lecturer" << lNames[il] <<  " teaching hour "  << j << " in day " << day << endl;
                        //cout << " lecturer hours " << lectDay[il][day] << endl;
                    //Verify if the lecturer took a rest at least one hour 
                    if(lectDay[il][day]>2){
                        il2=solution[i][j-1];
                        if(il2 !=-1 && il2==il)//the lecturer is teaching more than two hours without at least one hour break
                        {
                            cout << "Constraint Violation: lecturer" << lNames[il] <<  " is teaching more than two hours without break"  << endl;
                            // printf("Constraint Violation: lecturer %s is teaching more than two hours without break \n", lNames[i]);
                            failedConstraints++;
                        }
                    }
                        
                    //We added 1 hour per this course 
                    hoursday++;
                    if(hoursday>2){
                        cout << "Constraint Violation: More than two hours was assigned to course " << cNames[i] <<  " on day "   << day << endl;
                        //printf("Constraint Violation: More than two hours for course %s in  day %d \n", cNames[i], day);
                    failedConstraints++; 
                    }                
                    else if(hoursday>1){//No separated hours 
                        if(solution[i][j-1]==-1){
                            cout << "Constraint Violation: two  separate session for course " << cNames[i] <<  " on day "   << day << endl;
                            //printf("Constraint Violation: two separate session for course %s on day %d \n", cNames[i], day);
                            failedConstraints++; 
                        }
                    }
                    
                }// j != -1 
                
                //check when is a new day 
                if((j > 0) && (j%8==0)){
                    //a new day is starting 
                    day++;
                    //A lecture with more than two hours per day                          
                    hoursday=0;
                    //re-star hoursday 
                }
                    
            }//end for j-week
        }//end for i- courses 
        return failedConstraints;
    }
   
    static double getFitnessValue( vector < vector <int>>  solution, int courses, vector < vector <int>> LP, vector <int> coursehours,
                                    int failedConstraints){
        
        int hoursWeek=40;
        int il;//indice the lecturer
        double penalization;
        //double
        //i is rows 
        int totalAllocateHours=0;

        double sum=0;
        for (int i = 0; i < courses; i++) {
            //j is cols 
            for (int j = 0; j < hoursWeek; j++) {
                if(solution[i][j]!=-1){
                    totalAllocateHours++;
                    il=solution[i][j];
                    //sum the preference
                    if (LP[il][j] == 0)
                       sum = sum+ 10;    //  busy slot is penalised by 10
                     else 
                       sum=sum+(double)LP[il][j];
                    
                }
                
            }
            
        }//end for courses 


        //we're penalise per each constraint that the solution violated and the number of times 
        penalization=failedConstraints*20;

        int totalhours = accumulate(coursehours.begin(), coursehours.end(), 0);  

        if(totalAllocateHours!=totalhours){
            printf("You didn't allocate all the hours or allocate more than the hours");
            //We penalize for any discrepancy 
            penalization = 15*(double)abs(totalAllocateHours-totalhours);
            sum = sum+ penalization;
            
        }
        //we get the average per allocated hours
        sum=sum+failedConstraints;
        return sum/(double)totalhours;
    }

};

// template to parse a input field (labelled with match)
// into an output reference type
// if there is no match then the output is left untouched.
// returns true if there is a match.
template <typename T> bool parseField(string const& match, 
				      string input, T& output){
  int pos;
  string stripped;
  bool res=false;
  if (pos=input.find(match)!=string::npos){
    res=true;
    input.replace(pos-1,match.length(),"");  // get rid of label
    stripped=ProblemUCS::removeDoubleSpace(input);     // strip to space
    output = ston<T>(stripped);               // convert to num
  }
  return res;
}

// derived from split function from Evan Teran
vector<string> split(const string& s, char delim) {
  vector<string> res;
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim)){
    res.push_back(item);
  }
  return res;
}

// derived from split function from Evan Teran
vector<int> splitInt(const string& s, char delim) {
  vector<int> res;
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim)){
    res.push_back(atoi(item.c_str()));
  }
  return res;
}

