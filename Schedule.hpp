#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <utility>
#include <algorithm>
#include "EvalUCS.h"
#include "Solution.h"
using namespace std;

class Schedule
{
private:
	//solution of the problem
	vector<vector<int>> solution;

	//save the remaining hours of courses that cannot be filled up with the initial solution function
	//vector<courseNo, remaining hours>
	vector<pair<int,int>> remaining;
	
	//score of fit
	int fit;					//the total preference score of values that isn't 1 (i.e. 2 and 5)
	double testFit;				//the result from the goodness of fit function from EvalUCS

	//hold the preference of each slot filled in: map<cell in solution, preference>
	//use map for faster deletion when a course is moved away to another slot
	vector< pair<int, int> > preferenceScore;

	//fill up the room of the week
	vector<int> roomCount;

	//this vector will tell in which day a course is taught
	vector< vector<bool> > courseDay;
	


	//print the solution into an xml file
	void output()
	{
		ofstream xml;
		xml.open("solution.xml");
		for (int i=0; i<solution.size(); i++) {
			for (int m=0; m<solution[i].size() - 1; m++) {
				if (solution[i][m] != -1) xml << " ";				//add a space for alignemnt

				//add individual numbers in, followed by a comma
				xml << to_string( solution[i][m] ) << ",";				
			}

			//last value will not have the comma
			xml << to_string( solution[i][solution[i].back()] ) << "\n";	
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
						if (binaryMapping[m][x] == 1 && hours[x] > 0 && courseDay[x][div(i,8).quot] == false) {
							//number of the course
							solution[m][i] = x;	

							//the cell number in the solution vector and it's preference score		
							preferenceScore.push_back( {m*40 + i, preference[m][i]} );

							//set the course to have been taught today so it can't be taught again
							courseDay[x][div(i,8).quot] = true;

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
								preferenceScore.push_back( {m*40 + i, preference[m][i]} );
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
		for (int i=0; i<hours.size(); i++) {
			if (hours[i] > 0) {
				cout << "generation failed: course " << i << " has " << hours[i] << " hours remaining" << endl;
			}
		}

		return ;
	}




	//try to extend all of the 1 hour 1 score solution
	//while this may cause room overflow, it opens up more room for the worse solutions to move around
	void extend(int rooms, int courses, vector<int> &hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference)
	{
		//loop through the entire preferenceScore list to extend every 1 score solutions
		for (int i=0; i<preferenceScore.size(); i++) {
			if (preferenceScore[i].second == 1) {

				//extract the data of that slot: lecturerNo, slot (which session of the week) and courseNumber
				int slot = preferenceScore[i].first;
				int lecturerNo = div(slot, 40).quot;
				slot = div(slot, 40).rem;
				int courseNo = solution[lecturerNo][slot];

				//if there is no occupied slot before and after this one in the solution then we can extend it
				//else if it's already extended, there will be a cell before or after it
				if ( (slot % 8 == 0 || solution[lecturerNo][slot-1] == -1) && slot % 8 < 7 && solution[lecturerNo][slot+1] == -1 ) {
					// cout << "attempt to extend " << lecturerNo << " " << lecturerNo << endl;
					slot++;

					//check and see if the next session has the score less than 5 or not, regardless of room constraint
					//if we don't have enough room then we'll try to move the other session away to make room for it
					if (preference[lecturerNo][slot] < 5 && preference[lecturerNo][slot] > 0) {

						//if this slot is suitable for an extension, then swap out with another slot
						swap(rooms, courses, hours, lecturers, binaryMapping, preference, lecturerNo, slot, courseNo);
						// output();
					}

				}
			}
		}

		return ;
	}



	//this function will swap a slot out with the slot right after a course as an extension
	//it's somewhat like the reversed relocate function, but not quite
	void swap(int rooms, int courses, vector<int> &hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference, int lecturerNo, int index, int courseNo)
	{
		//worstScore holds the preferenceScore of the worst cell, worst holds the lecturerNo and slot of the worst cell
		int worstScore = 0;
		pair<int,int> worst = {-1,-1};
		int preferenceIndex = -1;

		//loop through the preference list, after this loop you may get the worst cell to swap to
		for (int i=0; i<preferenceScore.size(); i++) {
			int newLecturer = div(preferenceScore[i].first, 40).quot;
			int slot = div(preferenceScore[i].first, 40).rem;

			//if it's the worst cell and the same course as courseNo
			if (solution[newLecturer][slot] == courseNo && preference[newLecturer][slot] > worstScore) {
				worstScore = preferenceScore[i].second;
				worst = {newLecturer, slot};
				preferenceIndex = i;
			}
		}

		//if you can't swap with anything, return
		if (worst.first == -1) {
			return ;
		}

		//delete the worst cell
		solution[worst.first][worst.second] = -1;
		courseDay[courseNo][div(worst.second, 8).quot] = true;
		roomCount[worst.second]++;

		//add to the current cell
		solution[lecturerNo][index] = courseNo;
		roomCount[index]++;
		preferenceScore[preferenceIndex] = { lecturerNo * 40 + index, preference[lecturerNo][index] };

		//validate the solution after the extension
		validate(rooms, courses, hours, lecturers, binaryMapping, preference, index, lecturerNo);
		
	}



	//this function will test and see if the improvement introduces any clashing or not, if not call a function that relocate the cells to fix it
	void validate(int rooms, int courses, vector<int> &hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference, int index, int lecturerNo)
	{
		//see if there is sufficient room for that time of the day
		int classes = 0;

		//worst hold the location of that worst course, worstScore hold the preferenceScore of that slot
		int worst = -1;
		int worstScore = -1;

		//loop through and see how many courses are taught at a certain time
		for (int i=0; i<lecturers.size(); i++) {
			if (solution[i][index] != -1) {
				classes++;
				
				//see which slot is the worst one, off all the ones taught in that hour
				if (preference[i][index] >= worstScore) {
					worstScore = preference[i][index];
					worst = i;
				}
			}
		}

		//test to see if it's compatible or not (have less course than total room count)
		if (classes > rooms) {			
			//courseNo hold the course that needs to be relocated
			int courseNo = solution[worst][index];

			//find where that slot is in the preferenceScore vector
			int preferenceIndex = -1;
			int slot = worst * 40 + index;
			for (int i=0; i<preferenceScore.size(); i++) {
				if (preferenceScore[i].first == slot) {
					preferenceIndex = i;
					break;
				}
			}

			//relocate that course to another place
			relocate(rooms, courses, hours, lecturers, binaryMapping, preference, worst, index, courseNo, preferenceIndex);
		}
		

		//see if the next session is a teaching break or not
		index++;
		if (solution[lecturerNo][index] != -1) {
			//course to be relocated
			int courseNo = solution[lecturerNo][index];

			//find where that slot is in the preferenceScore vector
			int preferenceIndex = -1;
			int slot = lecturerNo * 40 + index;
			for (int i=0; i<preferenceScore.size(); i++) {
				if (preferenceScore[i].first == slot) {
					preferenceIndex = i;
					break;
				}
			}

			//relocate that course to another place
			relocate(rooms, courses, hours, lecturers, binaryMapping, preference, lecturerNo, index, courseNo, preferenceIndex);
		}
		return;
	}



	//this function will move all of the worst solution away from it's slot
	void badCell(int rooms, int courses, vector<int> hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference)
	{
		//loop through preferenceScore and relocate every 5 or 2 score cells possible
		for (int i=0; i<preferenceScore.size(); i++) {
			cout << preferenceScore[i].first << " " << preferenceScore[i].second << " " << i << endl;
			if (preferenceScore[i].second != 1) {

				//extract the data of that current slot: lecturerNo, slot (i.e. which session of the week) and courseNumber
				int slot = preferenceScore[i].first;
				int lecturerNo = div(slot, 40).quot;
				slot = div(slot, 40).rem;
				int courseNo = solution[lecturerNo][slot];
				cout << " " << courseNo << " " << slot << " " << lecturerNo << endl;
				//relocate that cell
				relocate(rooms, courses, hours, lecturers, binaryMapping, preference, lecturerNo, slot, courseNo, i);
			}
		}
	}



	//this function will relocate a slot to another place
	void relocate(int rooms, int courses, vector<int> &hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference, int lecturerNo, int index, int courseNo, int preferenceIndex)
	{
		//these 3 values determine the first valid slot with that preference score
		//idealy, there will be a valid slot with score of 1, but if there is none then we'll have to relocate the value to slot with score of 2 or 5
		//pair<lecturerNo, slot> best
		pair<int,int> best = {-1,-1};
		int bestScore = 6;

		//loop through all 40 hours and move on if the course is not taught that day
		for (int i=0; i<40; i++) {
			if (courseDay[courseNo][div(i,8).quot] == false) {
				//loop through every lecturer
				for (int m=0; m<lecturers.size(); m++) {
					//if lecturer teaches that course and there is room at that time
					//if there are no courses taught before and after this course and this cell is better than the current best cell
					if (binaryMapping[courseNo][m] == 1 && roomCount[i] > 0 && preference[m][i] < bestScore && preference[m][i] != 0 && solution[m][i] == -1 && (i%8==0 || solution[m][i-1] == -1) && (i%8==7 || solution[m][i+1] == -1) ) {
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
		courseDay[courseNo][div(index, 8).quot] = false;
		

		//make the variables look clearer
		int newLecturer = best.first;
		int slot = best.second;

		//insert the course into the right place
		solution[newLecturer][slot] = courseNo;
		roomCount[slot]--;
		courseDay[courseNo][div(slot, 8).quot] = true;
		preferenceScore[preferenceIndex] = {newLecturer * 40 + slot, bestScore};
		


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
				if (solution[i][m] != -1) cout << " \e[92m";
				cout << solution[i][m] << "\e[0m,";
				if (m % 8 == 7) cout << "    ";

			}
			cout << endl;
		}
		cout << endl;
		cout << endl;
	}


public:
	void generate()
	{
		ProblemUCS data;
		data.readUCSInstance("simple1.ucs");
		int rooms = data.rooms;
		int courses = data.mC;
		vector<int> hours = data.cHours;
		vector<string> names = data.cNames;
		vector<string> lecturers = data.lNames;
		vector<vector<int>> binaryMapping = data.TL;
		vector<vector<int>> preference = data.LP;

		//setup the class variables
		//clean the vector
		// solution.clear();
		// preferenceScore.clear();
		// roomCount.clear();
		// courseDay.clear();
		//setup the room
		for (int i=0; i<40; i++) {
			roomCount.push_back(rooms);
		}
		//setup the courseDay. all 5 days set to false
		for (int i=0; i<courses; i++) {
			courseDay.push_back({false, false, false, false, false});
		}
		//setup the score
		fit = 0;
		testFit = 0;


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
				//lunch break = 4th slot of the day (index 3)
				if (m % 8 == 3) preference[i][m] = 0;
			}
		}



		//generate an initial solution of the problem
		cout << "\e[32mINITIAL SOLUTION\e[0m" << endl;
		initialSolution(rooms, courses, hours, lecturers, binaryMapping, preference);
		printData();

		// //keep improving the result until you get the best score or run out of time
		// while (1) {
		// 	int copyFit = fit;

		// 	//extend the function
		// 	// cout << "\e[31mEXTEND FUNCTION\e[0m" << endl; 
		// 	// extend(rooms, courses, hours, lecturers, binaryMapping, preference);
		// 	// printData();

		// 	//output the xml file
		// 	//if (fit <= copyFit) output(); else break;

		// 	//move the bad results
		// 	// cout << "\e[36mBAD_CELL FUNCTION\e[0m" << endl; 
		// 	// badCell(rooms, courses, hours, lecturers, binaryMapping, preference);
		// 	// printData();

		// 	//output the xml file
		// 	//if (fit <= copyFit) output(); else break;

		// 	//exit the loop if the algorithm reached PEAK EFFICIENCY
		// 	if (fit == 0) break;		
		// }

		Solution test;
		test.readSolution("solution.xml");
		int constraints = test.checkConstraints(solution, rooms, hours, preference, names, lecturers);
		testFit = test.getFitnessValue(solution, courses, preference, hours, constraints);
		return ;
	}
	
	
};