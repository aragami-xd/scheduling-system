#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <map>
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

	//hold the preference of each slot filled in: map<cell in solution, preference>
	map<int, int> preferenceScore;

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
					xml << "  ";
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
		//loop through every lecturer
		for (int m=0; m<lecturers.size(); m++) {

			//loop through 40 hours
			for (int i=0; i<40; i++) {

				//if the lecturer is free at that session and there are still rooms avaiable
				if ( preference[m][i] != 0 && roomCount[i] > 0 ) {
					//loop through every course
					for (int x=0; x<courses; x++) {

						//if the lecturer teaches that course, there is time left to teach and the course has not been taught that day
						if (binaryMapping[x][m] == 1 && hours[x] > 0 && courseDay[x].count( div(i,8).quot ) == 0) {
							//number of the course
							solution[m][i] = x;	

							//the cell number in the solution vector and it's preference score		
							preferenceScore.insert( {m*40 + i, preference[m][i]} );

							//set the course to have been taught today so it can't be taught again
							courseDay[x].insert( div(i,8).quot );

							//decrease the total room count
							roomCount[i]--;

							//decrease the hour count
							hours[x]--;

							//move on to the next hour
							i++;

							//see if the lecturer can teach the next hour of not
							if ( i%8 > 0 && preference[m][i] != 0 && roomCount[i] > 0 && hours[x] > 0) {
								//pretty much like the one above but without setting courseDay (since it's already set)
								solution[m][i] = x;
								preferenceScore.insert( {m*40 + i, preference[m][i]} );
								roomCount[i]--;
								hours[x]--;
								i++;
							}

							break;
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
		for (auto x : preferenceScore) {
			if (x.second == 1) {

				//extract the data of that slot: lecturerNo, slot (which session of the week) and courseNumber
				int slot = x.first;
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
						swap(rooms, courses, hours, lecturers, binaryMapping, preference, lecturerNo, slot, courseNo);

					}

				}
			}
		}

		return ;
	}


	//this function will swap a slot out with the slot right after a course as an extension
	//this function will somewhat be the contrary of relocate
	void swap(int rooms, int courses, vector<int> &hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference, int lecturerNo, int index, int courseNo)
	{
		//worstScore holds the preferenceScore of the worst cell, worst holds the lecturerNo and slot of the worst cell
		int worstScore = 0;
		pair<int,int> worst = {-1,-1};

		//loop through the preference list, after this loop you may get the worst cell to swap to
		for (auto x : preferenceScore) {
			int newLecturer = div(x.first, 40).quot;
			int slot = div(x.second, 40).rem;

			//if it's the worst cell and the same course as courseNo
			if (solution[newLecturer][slot] == courseNo && preference[newLecturer][slot] > worstScore) {
				worstScore = x.second;
				worst = {newLecturer, slot};
			}
		}

		//if you can't swap with anything, return
		if (worst.first == -1) {
			cout << "fatal error: can't find any place to swap" << endl;
			return ;
		}

		//delete the worst cell
		solution[worst.first][worst.second] = -1;
		courseDay[courseNo].erase(worst.second % 8);
		roomCount[worst.second]++;
		preferenceScore.erase(worst.first * 40 + worst.second);

		//add to the current cell
		solution[lecturerNo][index] = courseNo;
		roomCount[index]++;
		preferenceScore.insert( {lecturerNo * 40 + index, 1} );

		//validate the solution after the extension
		validate(rooms, courses, hours, lecturers, binaryMapping, preference, index, lecturerNo);
		
	}



	//this function will test and see if the improvement introduces any clashing or not, if not call a function that relocate the cells to fix it
	void validate(int rooms, int courses, vector<int> &hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference, int index, int lecturerNo)
	{
		//see if there is sufficient room for that time of the day
		int classes = 0;

		//worst hold the location of that worst course, worstScore hold the preferenceScore of that slot
		pair<int,int> worst = {-1,-1};
		int worstScore = 0;

		//loop through and see how many courses are taught at a certain time
		for (int i=0; i<lecturers.size(); i++) {
			if (solution[i][index] != -1) {
				classes++;
				
				//see which slot is the worst one, off all the ones taught in that hour
				if (preference[i][index] >= worstScore) {
					worstScore = preference[i][index];
					worst = {i, index};
				}
			}
		}

		//test to see if it's compatible or not (have less course than total room count)
		if (classes > rooms) {			
			//courseNo hold the course that needs to be relocated
			int courseNo = preference[worst.first][worst.second];

			//relocate that course to another place
			relocate(rooms, courses, hours, lecturers, binaryMapping, preference, worst.first, worst.second, courseNo);
		}
		

		//see if the next session is a teaching break or not
		index++;
		if (solution[lecturerNo][index] != -1) {
			//course to be relcated
			int courseNo = preference[worst.first][worst.second];

			//relocate that course to another place
			relocate(rooms, courses, hours, lecturers, binaryMapping, preference, lecturerNo, index, courseNo);
		}
	}



	//this function will move all of the worst solution away from it's slot
	void badCell(int rooms, int courses, vector<int> hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference)
	{
		//loop through preferenceScore and relocate every 5 or 2 score cells possible
		for (auto x : preferenceScore) {
			if (x.second != 1) {

				//extract the data of that current slot: lecturerNo, slot (i.e. which session of the week) and courseNumber
				int slot = x.first;
				int lecturerNo = div(slot, 40).quot;
				slot = div(slot, 40).rem;
				int courseNo = solution[lecturerNo][slot];

				//relocate that cell
				relocate(rooms, courses, hours, lecturers, binaryMapping, preference, lecturerNo, slot, courseNo);
			}
		}
	}



	//this function will relocate a slot to another place
	void relocate(int rooms, int courses, vector<int> &hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference, int lecturerNo, int index, int courseNo)
	{
		//these 3 values determine the first valid slot with that preference score
		//idealy, there will be a valid slot with score of 1, but if there is none then we'll have to relocate the value to slot with score of 2 or 5
		//it will hold the value of which lecture will teach it at which slot
		pair<int,int> best = {-1,-1};
		int bestScore = 6;

		//loop through all 40 hours and move on if the course is not taught that day
		for (int i=0; i<40; i++) {
			if (courseDay[courseNo].count(i) == 0) {
				//loop through every lecturer
				for (int m=0; m<lecturers.size(); m++) {

					//if lecturer teaches that course and there is room at that time
					//if there are no courses taught before and after this course and this cell is better than the current best cell
					if (binaryMapping[courseNo][m] == 1 && roomCount[i] > 0 && preference[m][i] < bestScore && solution[m][i] == -1 && (solution[m][i] == -1 || i%8==0) && (solution[m][i] == -1 || i%8==7) ) {
						best = {m,i};
						bestScore = preference[m][i];
					}

				}
			}

			//if we have found a valid slot with score of 1
			if (bestScore == 1) {
				break ;
			}
		}

		if (best.first == -1) {
			cout << "fatal error: cannot relocate to anywhere" << endl;
			return ;
		}


		//if this cell can be moved then delete it
		solution[lecturerNo][index] = -1;
		roomCount[index]++;
		courseDay[courseNo].erase( div(index, 8).quot );
		preferenceScore.erase(lecturerNo * 40 + index);

		//move that cell into the right place
		int newLecturer = best.first;
		int slot = best.second;

		//insert the course into the right place
		solution[newLecturer][slot] = courseNo;
		roomCount[slot]--;
		courseDay[courseNo].insert( slot % 8 );
		preferenceScore.insert( {newLecturer * 40 + slot, bestScore} );

		return ;
	}



	//for debugging purposes
	void printData()
	{
		fit = 0;
		for (auto x : preferenceScore) {
			int lecturerNo = div(x.first, 40).quot;
			int slot = div (x.first, 40).rem;

			cout << x.second << " " << lecturerNo << " " << slot << endl;
			if (x.second != 1) {
				fit += x.second;
			}
		}
		cout << "total score: " << fit << endl; 

		for (int i=0; i<solution.size(); i++) {
			for (int m=0; m<solution[0].size(); m++) {
				if (solution[i][m] != -1) {
					cout << " \e[92m";
				}
				cout << solution[i][m] << "\e[0m,";

				if (m % 8 == 7) {
					cout << "    ";
				}

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
		cout << "\e[32mINITIAL SOLUTION\e[0m" << endl;
		initialSolution(rooms, courses, hours, lecturers, binaryMapping, preference);
		printData();

		//keep improving the result until you get the best score or run out of time
		while (1) {
			//extend the function
			cout << "\e[31mEXTEND FUNCTION\e[0m" << endl; 
			extend(rooms, courses, hours, lecturers, binaryMapping, preference);
			printData();

			//stop the algorithm once you can't improve it anymore
			if (fit == 0) {
				break;
			}

			//mvoe the bad results
			cout << "\e[36mBAD_CELL FUNCTION\e[0m" << endl; 
			badCell(rooms, courses, hours, lecturers, binaryMapping, preference);
			printData();

			//stop the algorithm once you can't improve it anymore
			if (fit == 0) {
				break;
			}

			//output the xml file
			output();
		}


		return ;
	}
	
};