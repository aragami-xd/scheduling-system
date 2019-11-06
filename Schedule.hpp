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
	//solution vector is based on the course: who and when will teach that course
	vector<vector<int>> solution;
	
	//debug solution (because the solution vector is so confusing)
	//debug vector is based on the lecturer: which and when to teach a course
	vector<vector<int>> debug;

	//roomCount vector: keeping track of how many rooms are available at a certain time of the week
	vector<int> roomCount;

	//courseDay vector: keeping track of which day a course is taught
	//true means the course is taught that day, else it's false
	vector<vector<bool>> courseDay;

	//twoHour vector: apparently a lecturer cannot teach 2 hour session once they have taught something that day
	vector<bool> twoHour;

	//preferenceScore vector: keeping track of the preference at that slot
	//tuple <preference, courseNo, hour of the week, lecturerNo>
	vector< tuple<int,int,int,int> > preferenceScore;

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
							preferenceScore.push_back( {LP[i][m],n,m,i} );
							solution[n][m] = i;
							debug[i][m] = n;

							//if this is not the last hour of the day then try to extend it to 2 hours
							if ( m%8 < 7 ) {
								m++;
								//if the next hour is not busy, still have time in the course, room to teach and the lecturer has not taught that day
								if ( LP[i][m] != 0 && cHoursCopy[n] > 0 && roomCount[m] > 0 /*&& !twoHour[i]*/ ) {
									cHoursCopy[n]--;
									roomCount[m]--;
									preferenceScore.push_back( {LP[i][m],n,m,i} );
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

		//see how many hours have not bee alocated for each course
		cout << endl;
		for (int i=0; i<mC; i++) {
			if (cHoursCopy[i] > 0) cout << "course \e[32m" << i << "\e[0m has \e[32m" << cHoursCopy[i] << "\e[0m left" << endl; 
		}

		return ;
	}


	
	//this function will fill in the remaining hours that intial solution cannot fill in, if any
	void validateInitial(vector<int> remaining)
	{
		for (int i=0; i<remaining.size(); i++) {

		}
	}



	//this function will move all of the bad cell away to a better location
	void improve()
	{
		for (int i=0; i<preferenceScore.size(); i++) {
			if ( get<0>(preferenceScore[i]) != 1 ) relocate(preferenceScore[i], i);
		}
	}



	//this function will move a cell to the best place it can find, then relocate every cells that would cause an error in that session
	//tuple <preferenceScore, courseNo, location in debug, lecturerNo>
	//index stores the location of that tuple in the preferenceScore vector
	void relocate(tuple<int,int,int,int> cell, int index)
	{
		//cout << "call relocate function" << endl;
		//best hold the location of the best cell in the solution (the place it will move to) and bestScore hold the preference at that location
		tuple<int,int,int,int> best = {-1,-1,-1,-1};
		int bestScore = 6;

		//loop through every lecturer and see if they teach that course or not
		for (int i=0; i<nL; i++) {
			if ( TL[ get<1>(cell) ][i] == 1 ) {
				//loop from the start to the end and see which is the best tie to teach that course for that lecturer
				for (int m=0; m<40; m++) {
					//if that cell is better than the previous cell then change the best cell and the cell must be empty
					//"hard constraints"
					if ( debug[get<3>(cell)][m] == -1 && LP[i][m] < bestScore && LP[i][m] > 0 ) {
						//"soft constraints"
						//no course is taught before and after this course
						//there are enough rooms (this one might be deleted later)
						//the new cell is either in the same day as the current cell (argument)
						if ( (m%8==0 || debug[i][m-1] == -1) && (m%8==7 || debug[i][m+1] == -1) && roomCount[m] > 0 && courseDay[get<1>(cell)][div(m,8).quot] == false ) {
							bestScore = LP[i][m];
							best = {bestScore, get<1>(cell), m,i};
						}

					}
				}
				
				//stop the loop if found the best score of 1
				if (bestScore == 1) break;
			}
		}

		//if you can't move anywhere
		if (bestScore == 6) {
			return ;
		}

		//delete the previous cell
		solution[get<1>(cell)][get<2>(cell)] = -1;
		debug[get<3>(cell)][get<2>(cell)] = -1;
		courseDay[get<1>(cell)][div(get<2>(cell), 8).quot] = false;
		roomCount[get<2>(cell)]--;

		//add the best cell in
		solution[get<1>(best)][get<2>(best)] = get<3>(best);
		debug[get<3>(best)][get<2>(best)] = get<1>(best);
		courseDay[get<1>(best)][div(get<2>(best), 8).quot] = true;
		roomCount[get<2>(best)]++;

		//modify the preferenceScore
		preferenceScore[index] = best;

		//in case room count constraint is not checked, this function will responsible for it
		//note: not working so well at the moment
		//validateRoom(best);
	}



	//this function will check and see if there is enough room or not, if not, move the worst cell away to a better place
	//again, tuple <prefenceScore, courseNo, location in debug, lecturerNo>
	// void validateRoom(tuple<int,int,int,int> cell)
	// {
	// 	//this vector will hold the preference at each 
	// 	vector<int> occupied(nL);

	// 	//see how many rooms are occupied
	// 	for (int i=0; i<LP.size(); i++) {
	// 		occupied.push_back( LP[i][get<2>(cell)] );
	// 	}

	// 	//if there are still enough rooms
	// 	if (occupied.size() <= rooms) {
	// 		return ;
	// 	}

	// 	//else, call the relocate function
	// 	int bestCell = *max_element(occupied.begin(), occupied.end());
	// 	tuple<int,int,int,int> best = {occupied[bestCell], debug[bestCell][get<2>(cell)], get<2>(cell), bestCell};

	// 	//see which cell is that cell in the preferenceList
	// 	for (int i=0; i<preferenceScore.size(); i++) {
	// 		if (best == preferenceScore[i]) {
	// 			relocate(best, i);
	// 			return;
	// 		}
	// 	}

	// }




public:
	void generate()
	{
		//get the data
		ProblemUCS data;
		if ( !data.readUCSInstance("medium1.ucs") ) {
			cout << "oof" << endl;	//cannot read the file
			return ;
		}
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
	
		//setup the two hour vector
		for (int i=0; i<nL; i++) {
			twoHour.push_back(false);
		}


		//the initial solution
		initialSolution();

		//improve the solution (5 times)
		for (int i=0; i<5; i++) improve();


		//print the solution vector
		cout << endl;
		cout << "solution" << endl;
		for (int i=0; i<solution.size(); i++) {
			for (int m=0; m<40; m++) {
				if (solution[i][m] != -1) cout << "\e[32m ";
				cout << solution[i][m] << " \e[0m";
				if (m%8==7) cout << "  ";
			}
			cout << endl;
		}

		//print out the preference
		cout << endl;
		cout << "preference" << endl;
		for (int i=0; i<LP.size(); i++) {
			for (int m=0; m<40; m++) {
				if (debug[i][m] != -1) cout << "\e[32m";
				cout << " " << LP[i][m] << " \e[0m";
				if (m%8==7) cout << "  ";
			}
			cout << endl;
		}

		//print the debug vector
		cout << endl;
		cout << "debug" << endl;
		for (int i=0; i<debug.size(); i++) {
			for (int m=0; m<40; m++) {
				if (debug[i][m] != -1) cout << "\e[32m ";
				cout << debug[i][m] << " \e[0m";
				if (m%8==7) cout << "  ";
			}
			cout << endl;
		}

		//test the data
		cout << endl;
		Solution::printTimetable(solution, cNames, lNames);
		double fit = Solution::getFitnessValue(solution, mC, LP, cHours, Solution::checkConstraints(solution, rooms, cHours, LP, cNames, lNames) );
		cout << endl;
		cout << "fit: " << fit << endl;
		return ;
	}
	
	
};