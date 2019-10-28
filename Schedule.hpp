#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <utility>
#include <algorithm>
// #include "EvalUCS.hpp"
using namespace std;

class Schedule
{
private:
	//solution of the problem
	vector<vector<int>> solution;
	
	//score of fit
	int fit;					//the total preference score of values that isn't 1 (i.e. 2 and 5)
	// double testFit;				//the result from the goodness of fit function from EvalUCS

	//hold the preference of each slot filled in: pair<preference, cell in solution>
	vector< pair<int, int> > preferenceScore;

	//fill up the room of the week
	vector<int> roomCount;

	//this vector will tell in which day a course is taught
	//used for constant access when moving the course around, using set.count() function
	vector< unordered_set<int> > courseDay;
	


	//print the solution into an xml file
	void output()
	{
		ofstream xml;
		xml.open("solution.xml");
		for (int i=0; i<solution.size(); i++) {
			for (int m=0; m<solution[i].size() - 1; m++) {
				if (solution[i][m] != -1) {
					xml << " ";				//add a space for alignemnt
				}

				//add individual numbers in, followed by a comma
				xml << to_string( solution[i][m] ) << ",";	
				if (m % 8 == 7) {			//add 4 spaces between each day
					xml << "    ";
				}					
			}

			//last value will not have the comma
			xml << to_string( solution[i][solution[i].size() - 1] ) << "\n";	
		}
		xml.close();
		return;
	}



	//create an iniial solution
	/* the function in a nutshell:

		loop through 5 days of the week
			loop through every lecturer
				loop through 8 hours a day
					if the lecturer can teach at that session and there are room left
						loop through every course and see which course the lecturer can teach at that session
						(the course mustn't been taught that day and still have time left to teach)
							modify solution vector
							push_back the preferenceScore vector
							add this day into courseDay so the course cannot be taught again
							skip 1 hour ahead
								see if the next hour can be taught or not, if yes, do pretty much like the last hour
	*/
	void initialSolution(int rooms, int courses, vector<int> hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference)
	{
		//the conditions
		//loop through every day of the week
		for (int i=0; i<5; i++) {
			//loop through every lecturer
			for (int m=0; m<lecturers.size(); m++) {
				//loop through every hour of the day
				for (int n=0; n<8; n++) {

					//if the lecturer is free at that session and there are still rooms avaiable
					if ( preference[m][i*8 + n] != 0 && roomCount[i*8 + n] > 0 ) {
						//loop through every course
						for (int x=0; x<courses; x++) {
							//if the lecturer teaches that course, there is time left to teach and the course has not been taught that day
							if (binaryMapping[x][m] == 1 && hours[x] > 0 && courseDay[x].count(i) == 0) {
								//number of the course
								solution[m][i*8 + n] = x;	

								//the cell number in the solution vector and it's preference score		
								preferenceScore.push_back( {preference[m][i*8 + n], m*40 + i*8 + n} );

								//set the course to have been taught today so it can't be taught again
								courseDay[x].insert(i);

								//decrease the total room count
								roomCount[i*8 + n]--;

								//decrease the hour count
								hours[x]--;

								//move on to the next hour (end of the day will exit the loop)
								n++;

								//see if the lecturer can teach the next hour of not
								if (n < 8 && preference[m][i*8 + n] != 0 && roomCount[i*8 + n] > 0 && hours[x] > 0 ) {
									//pretty much like the one above but without setting courseDay (since it's already set)
									solution[m][i*8 + n] = x;
									preferenceScore.push_back( {preference[m][i*8 + n], m*40 + i*8 + n} );
									roomCount[i*8 + n]--;
									hours[x]--;
									n++;
								}

								break;
							}
						}
					}

				}
			}
		}

		//validation, in case with a specific test input i can't generate any valid output from this
		// for (int i=0; i<hours.size(); i++) {
		// 	if (hours[i] > 0) {
		// 		cout << "generation failed: course " << i << " has " << hours[i] << " hours remaining" << endl;
		// 	}
		// }

		return ;
	}




	//try to extend all of the 1 hour 1 score solution
	//while this may cause room overflow, it opens up more room for the worse solutions to move around
	void extend(int rooms, int courses, vector<int> &hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference)
	{
		//loop through the entire preferenceScore list to extend every 1 score solutions (if the course has more than 1 hour a week)
		for (int i=0; i<preferenceScore.size(); i++) {
			if (preferenceScore[i].first == 1) {

				//extract the data of that slot: lecturerNo, slot (i.e. which session of the week) and courseNumber
				int slot = preferenceScore[i].second;
				int lecturerNo = div(slot, 40).quot;
				slot = div(slot, 40).rem;
				int courseNo = solution[lecturerNo][slot];

				//if there is no occupied slot before and after this one in the solution and the course has more than 1 hour to teach per week
				if ( (slot % 8 == 0 || solution[lecturerNo][slot-1] == -1) && slot % 8 < 7 && solution[lecturerNo][slot+1] == -1 && hours[courseNo] > 1) {
					slot++;

					//check and see if the next session has the score of 1 or not, regardless of room constraint
					//if we don't have enough room then we'll try to move the other session away to make room for it
					if (preference[lecturerNo][slot] == 1) {
						//if this slot is suitable for an extension, then swap out with another slot
						bool betterSolution = swap(binaryMapping, preference, i, courseNo);

						//if you can swap out for a better solution
						if (betterSolution) {
							solution[lecturerNo][slot] = courseNo;

							//after the extension is done, validate it
							validate(rooms, courses, hours, lecturers, binaryMapping, preference, slot, lecturerNo);
						} 
					}

				}
			}
		}

		return ;
	}



	//this function will swap out another session for the extended one
	//it looks a bit like the relocate function, but the other way around
	bool swap(vector<vector<int>> &binaryMapping, vector<vector<int>> &preference, int index, int currentCourse)
	{
		//loop from the worst solution to the best solution (skip itself)
		for (int i=preferenceScore.size() - 1; i>0; i--) {
			if (i != index) {
				//extract the data of that current slot: lecturerNo, slot (i.e. which session of the week) and courseNumber
				int slot = preferenceScore[i].second;
				int lecturerNo = div(slot, 40).quot;
				slot = div(slot, 40).rem;
				int courseNo = solution[lecturerNo][slot];

				//if this course is the current course then delete this course, change the room data and move on
				if (currentCourse == courseNo) {
					preferenceScore.erase(preferenceScore.begin() + i);
					solution[lecturerNo][slot] = -1;
					roomCount[slot]++;

					//erase the value in the set (i.e. the course is not taught in that day anymore)
					courseDay[currentCourse].erase( div(slot, 8).quot );
					return true;
				}
			}

		}

		return false;
	}



	//this function will test and see if the improvement introduces any clashing or not, if not call a function that relocate the cells to fix it
	void validate(int rooms, int courses, vector<int> &hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference, int index, int lecturerNo)
	{
		//see if there is sufficient room for that time of the day
		//score vector hold the preference value and the lecturer that teach that course
		int classes = 0;
		vector< pair<int,int> > score(courses);

		//worst hold the lecturer that worst course, worstScore hold the preferenceScore of that slot
		int worst = 0;
		int worstScore = 0;

		//loop through and see how many courses are taught at a certain time
		for (int i=0; i<lecturers.size(); i++) {
			if (solution[i][index] != -1) {
				classes++;
				score.push_back( {preference[i][index], i} );
				
				//see which slot is the worst one, off all the ones taught in that hour
				if (score.back().first >= worstScore) {
					worstScore = score.back().first;
					worst = score.back().second;
				}
			}
		}

		//test to see if it's compatible or not (have less course than total room count)
		if (classes > rooms) {			
			//courseNo hold the course that needs to be relocated
			int courseNo = score[worst].second * 8 + index;

			//relocate that course
			relocate(rooms, courses, hours, lecturers, binaryMapping, preference, div(worst, 40).quot, div(worst, 40).rem, courseNo);
		}

		//see if the next session is a teaching break or not
		if (solution[lecturerNo][index + 1] != -1) {
			relocate(rooms, courses, hours, lecturers, binaryMapping, preference, lecturerNo, index + 1, solution[lecturerNo][index + 1]);
		}
	}



	//this function will move all of the worst solution away from it's slot
	void badCell(int rooms, int courses, vector<int> hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference)
	{
		//loop from the back to the front of the preferenceScore vector and relocate every 5 or 2 score cells possible
		for (int i=preferenceScore.size() - 1; i>=0; i--) {
			if (preferenceScore[i].first != 1) {
				//extract the data of that current slot: lecturerNo, slot (i.e. which session of the week) and courseNumber
				int slot = preferenceScore[i].second;
				int lecturerNo = div(slot, 40).quot;
				slot = div(slot, 40).rem;
				int courseNo = solution[lecturerNo][slot];

				//relocate that cell
				relocate(rooms, courses, hours, lecturers, binaryMapping, preference, lecturerNo, slot, courseNo);
			}
		}
	}



	//this funciton will relocate the "invalid" cell another location (we only need the course, so everything else doesn't matter)
	void relocate(int rooms, int courses, vector<int> &hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference, int lecturerNo, int index, int courseNo)
	{
		//these 3 values determine the first valid slot with that preference score
		//idealy, there will be a valid slot with score of 1, but if there is none then we'll have to relocate the value to slot with score of 2 or 5
		//it will hold the value of which lecture will teach it, at which day and which hour of that day
		int one = -1, two = -1, five = -1;

		//loop through 5 days to see which in which day the course has not been taught in so it can move there
		for (int i=0; i<5; i++) {
			if (courseDay[courseNo].count(i) == 0) {

				//loop through every lecturer
				for (int m=0; m<lecturers.size(); m++) {
					//if lecturer teaches that course
					if (binaryMapping[courseNo][m] == 1) {

						//loop through 8 hours of the day
						for (int n=0; n<8; n++) {
							int slot = i*8 + n;
							//get the score and see if it can be set as the slot to relocate to or not
							if (preference[m][slot] == 1 && one == -1 && solution[m][slot] == -1 && roomCount[slot] > 0) {
								one = m *40 + i*8 + n;
							} else if (preference[m][slot] == 2 && two == -1 && solution[m][slot] == -1 && roomCount[slot] > 0) {
								two = m*40 + i*8 + n;
							} else if (preference[m][slot] == 5 && five == -1 && solution[m][slot] == -1 && roomCount[slot] > 0) {
								five = m*40 + i*8 + n;
							}
						}

					}
				}

			}

			//if we have found a valid slot with score of 1
			if (one != -1) {
				break ;
			}
		}

		//get the index of the best solution to move to (if any)
		int moveIndex = -1;
		int finalPreference = 0;
		if (one != -1) {					//if there is an index stored in any of the three values then set the best one as the final index
			moveIndex = one;
			finalPreference = 1;
		} else if (two != -1) {
			moveIndex = two;
			finalPreference = 2;
		} else if (five != -1) {
			moveIndex = five;
			finalPreference = 5;
		} else {							//if no index can be found at all then just return it and pray to god that it would be improved later on
			return;
		}

		//if this cell can be moved then delete it
		solution[lecturerNo][index] = -1;
		roomCount[index]++;
		courseDay[courseNo].erase( div(index, 8).quot );
		preferenceScore.erase(preferenceScore.begin() + index);

		//move that cell into the right place
		int newLecturer = div(moveIndex, 40).quot;
		int slot = div(moveIndex, 40).rem;

		//insert the course into the right place
		solution[newLecturer][slot] = courseNo;
		roomCount[slot]--;
		courseDay[courseNo].insert({moveIndex, finalPreference});

		return ;
	}


	//for debugging purposes
	void printData()
	{
		fit = 0;
		for (int i=0; i<preferenceScore.size(); i++) {
			int lecturerNo = div(preferenceScore[i].second, 40).quot;
			int slot = div (preferenceScore[i].second, 40).rem;

			cout << preferenceScore[i].first << " " << lecturerNo << " " << slot << endl;
			if (preferenceScore[i].first != 1) {
				fit += preferenceScore[i].first;
			}
		}

		for (int i=0; i<solution.size(); i++) {
			for (int m=0; m<solution[0].size(); m++) {
				if (solution[i][m] != -1) {
					cout << " \e[92m";
				}
				cout << solution[i][m] << "\e[0m, ";
			}
			cout << endl;
		}
		cout << endl;
		cout << endl;
	}


public:
	void generate(int rooms, int courses, vector<int> hours, vector<string> names, vector<string> lecturers, vector<vector<int>> binaryMapping, vector<vector<int>> preference)
	{
		//setup the class variables
		//clean the vector
		solution.clear();
		preferenceScore.clear();
		roomCount.clear();
		courseDay.clear();
		//setup the room
		for (int i=0; i<40; i++) {
			roomCount.push_back(rooms);
		}
		//setup the courseDay
		for (int i=0; i<courses; i++) {
			courseDay.push_back({});
		}
		//setup the score
		fit = 0;
		// testFit = 0;


		//create a blank solution 
		for (int i=0; i<lecturers.size(); i++) {					
			solution.push_back({});
			for (int m=0; m<40; m++) {
				solution.back().push_back(-1);
			}
		}

		//change every lunch break to busy
		for (int i=0; i<lecturers.size(); i++) {
			for (int m=0; m<40; m++) {
				if (m % 8 == 3) {						//lunch break = 4th slot of the day (index 3)
					preference[i][m] = 0;
				}
			}
		}



		//generate an initial solution of the problem
		initialSolution(rooms, courses, hours, lecturers, binaryMapping, preference);

		while (1) {
			//extend the function
			extend(rooms, courses, hours, lecturers, binaryMapping, preference);
			printData();

			//mvoe the bad results
			badCell(rooms, courses, hours, lecturers, binaryMapping, preference);
			printData();

			//output the xml file
			output();

			printData();

			//continue the alogirthm
			int cont;
			cin >> cont;
			if (cont == 1) {
				break;
			}
		}






		return ;
	}
	
};