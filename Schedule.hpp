#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <tuple>
#include "EvalUCS.h"
#include "Solution.h"
using namespace std;

class Schedule
{
private:
	//solution vector
	vector<vector<int>> solution;
	
	//debug solution (because the solution vector is so confusing)
	vector<vector<int>> debug;

	//roomCount vector: keeping track of how many rooms are available at a certain time of the week
	vector<int> roomCount;

	//courseDay vector: keeping track of which day a course is taught
	//true means the course is taught that day, else it's false
	vector<vector<bool>> courseDay;

	//twoHour vector: apparently a lecturer cannot teach 2 hour session once they have taught something that day
	vector<bool> twoHour;

	//preferenceScore vector: keeping track of the preference at that slot
	//tuple <preference, lecturer, hour of the week>
	vector< tuple<int,int,int> > preferenceScore;

	//all the other data
	int rooms;
	int mC;
	vector<int> cHours;
	vector<string> cNames;
	int nL;
	vector<string> lNames;
	vector<vector<int>> TL;
	vector<vector<int>> LP;


	//generate an initial solution
	void initialSolution()
	{
		//make a copy of the cHours
		vector<int> cHoursCopy = cHours;

		//loop through all the lecturers and every hour of the week
		for (int i=0; i<nL; i++) {
			for (int m=0; m<40; m++) {
				//if the lecturer can teach that session then check and see which course can that lecturer teach
				if ( LP[i][m] != 0 ) {
					for (int n=0; n<mC; n++) {
						//if the lecturer teaches that course, the course has not been taught that day
						//the course still has hours left to be taught and there are rooms available
						if ( TL[n][i] == 1 && !courseDay[n][div(m,8).quot] && cHoursCopy[n] > 0 && roomCount[m] > 0 ) {
							//change the data
							cHoursCopy[n]--;
							roomCount[m]--;
							courseDay[n][div(m,8).quot] = true;
							preferenceScore.push_back( {LP[i][m],i,m} );
							solution[n][m] = i;
							debug[i][m] = n;

							//if this is not the last hour of the day then try to extend it to 2 hours
							if ( m%8 < 7 ) {
								m++;
								//if the next hour is not busy, still have time in the course, room to teach and the lecturer has not taught that day
								if ( LP[i][m] != 0 && cHoursCopy[n] > 0 && roomCount[m] > 0 && !twoHour[i] ) {
									cHoursCopy[n]--;
									roomCount[m]--;
									preferenceScore.push_back( {LP[i][m],i,m} );
									solution[n][m] = i;
									debug[i][m] = n;
									
									//jump to the next hour
									if ( m%8 < 7 ) m++;
								}
							}
							
							//setting status that the lecturer has already taught that day
							twoHour[i] = true;
							break;
						}
					}
				}
				
				//reset the state for the new day
				if ( m%8==7 ) twoHour[i] = false;
			}
		}

		//initial solution validation
		for (int i=0; i<cHoursCopy.size(); i++) {
			if (cHoursCopy[i] > 0) {
				cout << "course " << i << " is not full: " << cHoursCopy[i] << endl;
			}
		}
		return;
	}

public:
	void generate()
	{
		//get the data
		ProblemUCS data;
		data.readUCSInstance("simple1.ucs");
		rooms = data.rooms;
		mC = data.mC;
		cHours = data.cHours;
		cNames = data.cNames;
		nL = data.nL;
		lNames = data.lNames;
		TL = data.TL;
		LP = data.LP;

		//set lunch break to 0
		for (int i=0; i<nL; i++) {
			for (int m=3; m<40; m+=8) {
				LP[i][m] = 0;
			}
		}

		//setup the room
		for (int i=0; i<40; i++) {
			roomCount.push_back(rooms);
		}

		//setup the solution vector
		for (int i=0; i<mC; i++) {
			solution.push_back({});
			for (int m=0; m<40; m++) {
				solution[i].push_back(-1);
			}
		}

		//setup the courseDay
		for (int i=0; i<mC; i++) {
			courseDay.push_back({});
			for (int m=0; m<5; m++) {
				courseDay[i].push_back(false);
			}
		}

		//setup the debug vector
		for (int i=0; i<nL; i++) {
			debug.push_back({});
			for (int m=0; m<40; m++) {
				debug[i].push_back(-1);
			}
		}
	
		for (int i=0; i<nL; i++) {
			twoHour.push_back(false);
		}

		initialSolution();

		//print the output
		for (int i=0; i<debug.size(); i++) {
			for (int m=0; m<40; m++) {
				if (debug[i][m] != -1) cout << "\e[32m ";
				cout << debug[i][m] << " \e[0m";
				if (m%8==7) cout << "  ";
			}
			cout << endl;
		}

		//test the data
		Solution test;
		int constraints = test.checkConstraints(solution, rooms, cHours, LP, cNames, lNames);
		double fit = test.getFitnessValue(solution, mC, LP, cHours, constraints);
		cout << "fit: " << fit << endl;
		return ;
	}
	
	
};