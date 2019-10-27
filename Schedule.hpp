#include <iostream>
#include <string>
#include <vector>
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
	double testFit;				//the result from the goodness of fit function from EvalUCS

	//hold the preference of each slot filled in: pair<preference, cell in solution>
	vector< pair<int, int> > preferenceScore;

	//fill up the room of the week
	vector<int> roomCount;
	


	//print the solution into an xml file
	void output()
	{
		ofstream xml;
		xml.open("solution.xml");
		for (int i=0; i<solution.size(); i++) {
			for (int m=0; m<solution[i].size() - 1; m++) {
				if (m % 8 == 0) {			//add 4 spaces between each day
					xml << "    ";
				}
				if (solution[i][m] != -1) {
					xml << " ";				//add a space for alignemnt
				}

				//add individual numbers in, followed by a comma
				xml << to_string( solution[i][m] ) << ",";						
			}

			//last value will not have the comma
			xml << to_string( solution[i][solution[i].size() - 1] ) << "\n";	
		}
		xml.close();
		return;
	}


	//create an iniial solution
	/* the function in a nutshell:
		fill up the room for each day
		fill up a vector that check what is the earliest day that a course can be taught. by default is the first day

		loop through 5 days of the week
			loop through every lecturer
				loop through 8 hours a day
					if the lecturer can teach at that session and there are room left
						loop through every course and see which course the lecturer can teach at that session
						(the course mustn't been taught that day and still have time left to teach)
							modify solution vector
							push_back the preferenceScore vector
							set courseDay to the next day and decrease room count
							skip 1 hour ahead if this is not the last hour of the day
	*/
	void initialSolution(int rooms, int courses, vector<int> hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference)
	{
		//the earliest day that the course can be taught
		vector<int> courseDay(courses);
		fill(courseDay.begin(), courseDay.end(), 0);

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
							if (binaryMapping[x][m] == 1 && hours[x] > 0 && i >= courseDay[x]) {
								//number of the course
								solution[m][i*8 + n] = x;	

								//the cell number in the solution vector and it's preference score		
								preferenceScore.push_back( {preference[m][i*8 + n], m*40 + i*8 + n} );

								//set the course to the next day so it can't be taught again
								courseDay[x] = i+1;

								//decrease the total room count
								roomCount[i*8 + n]--;

								//decrease the hour count
								hours[x]--;

								//move on to the next hour (end of the day will exit the loop)
								n++;

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

		//sort the vector of preference desendingly by it's score
		sort(preferenceScore.begin(), preferenceScore.end());

		return ;
	}




//try to extend all of the 1 hour 1 score solution
//while this may cause room overflow, it opens up more room for the worse solutions to move around
void extend(int rooms, int courses, vector<int> &hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference)
{
	//loop through the entire preferenceScore list to extend every 1 score solutions (if the course has more than 1 hour a week)
	for (int i=0; i<preferenceScore.size(); i++) {
		if (preferenceScore[i].first == 1 && hours[i] > 1) {

			//extract the data of that slot: lecturerNo, slot (i.e. which session of the week) and courseNumber
			int slot = preferenceScore[i].second + 1;
			int lecturerNo = div(slot, 40).quot;
			slot = div(slot, 40).rem;
			int courseNo = solution[lecturerNo][slot - 1];

			//check and see if the next session has the score of 1 or not, regardless of room constraint
			//if we don't have enough room then we'll try to move the other session away to make room for it
			if (preference[lecturerNo][slot] == 1) {
				//if this slot is suitable for an extension, then swap out with another slot
				bool betterSolution = swap(binaryMapping, preference, i, courseNo);
				//if you can swap out for a better solution
				if (betterSolution) {
					solution[lecturerNo][slot] = courseNo;
				} 
			}

		}
	}
	return ;
}



//this function will swap out another session for the extended one
bool swap(vector<vector<int>> &binaryMapping, vector<vector<int>> &preference, int index, int currentCourse)
{
	//loop from the worst solution to the best solution
	for (int i=preferenceScore.size() - 1; i>index; i--) {
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
			return true;
		}
	}
	return false;
}



//this function will test and see if the improvement introduces any clashing or not, if not call a function that relocate the cells to fix it
void validate(int rooms, int courses, vector<int> &hours, vector<string> &lecturers, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference, int index, int lecturerNo)
{
	//see if there is sufficient room for that time of the day
	int classes = 0;
	vector<int> score(courses);						//get the preference of every cell with the same time so you can relocate the worst one
	for (int i=0; i<lecturers.size(); i++) {
		if (solution[i][index] != -1) {
			classes++;
			score.push_back(preference[i][index]);
		}
	}
	if (classes > rooms) {
		int worst = *min_element(score.begin(), score.end());
		// relocate();
	}

	//see if the next session is a teaching break or not
	if (solution[i][index + 1] != -1) {
		// relocate();
	}
}



//this funciton will relocate the "invalid" cell another location



public:
	void generate(int rooms, int courses, vector<int> hours, vector<string> names, vector<string> lecturers, vector<vector<int>> binaryMapping, vector<vector<int>> preference)
	{
		//setup the class variables
		solution.clear();
		preferenceScore.clear();
		roomCount.clear();

		//setup the room
		for (int i=0; i<40; i++) {
			roomCount.push_back(rooms);
		}
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
				if (m % 8 == 3) {						//lunch break = 4th slot of the day (index 3)
					preference[i][m] = 0;
				}
			}
		}



		//generate an initial solution of the problem
		initialSolution(rooms, courses, hours, lecturers, binaryMapping, preference);

		//extend the function
		extend(rooms, courses, hours, lecturers, binaryMapping, preference);

		//output the xml file
		output();


		
		for (int i=0; i<preferenceScore.size(); i++) {
			cout << preferenceScore[i].first << " " << preferenceScore[i].second << endl;
			if (preferenceScore[i].first != 1) {
				fit += preferenceScore[i].first;
			}
			
		}
		cout << "total score: " << fit << endl;

		return ;
	}
	
};