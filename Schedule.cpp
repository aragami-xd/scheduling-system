#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Schedule
{
private:
	int counter = 0;
	int rooms = 0;
	int currentDay = 8;


	//setting up a timeTable with full of -1
	vector< vector<int> > generateGenericTimetable(int noLecturers)
	{
		vector< vector<int> > genericTimetable;

		for (int i=0; i<noLecturers; i++) {
			genericTimetable.push_back({});
			for (int m=0; m<40; m++) {
				genericTimetable[i].push_back(-1);
			}
		}

		return genericTimetable;
	}



	//create a vector of list of courses that a lecturer will teach
	vector< vector<int> > generateTeachingCourses(vector< vector<int> > &binaryMapping)
	{
		vector< vector<int> > teachingCourses;

		for (int i=0; i<binaryMapping[0].size(); i++) {			//all the lecturers
			teachingCourses.push_back({});

			for (int m=0; m<binaryMapping.size(); m++) {		//if the lecturer teaches that course, add to the list
				if (binaryMapping[m][i] == 1) {
					teachingCourses[i].push_back(m);
				}
			}
		}


		return teachingCourses;
	}









	//determining if the lecturer can teach at this session, and if can, decides to teach or not
	void generateLecturer(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, vector< vector<int> > timeTable, vector<bool> taught, vector<int> roomLayout)
	{
		//continue looping until hit the base case or a valid case
		while (1) {


			// all the different invalid cases and jump cases (end of the day jump)
			if (lecturerNo == timeTable.size() - 1 && session == 40) {
				// all the lecturers have been generated to the last day of the week, return

				//print out everything for testing purposes only
				for (int i=0; i<timeTable.size(); i++) {
					for (int m=0; m<40; m++) {
						if (timeTable[i][m] != -1) {
							cout << " \e[94m";
						}
						cout << timeTable[i][m] << " \e[0m";
					}
					cout << endl;
				}
				return;

			} else if (lecturerNo == timeTable.size() - 1 && session == currentDay) {
				// if one day for the last lecturer has been generated then go back to the first lecturer and start on the next day
				currentDay += 8;
				lecturerNo = 0;
				session++;

				fill(taught.begin(), taught.end(), false);						//reset the room and taught for the next day
				fill(roomLayout.begin(), roomLayout.end(), rooms);

			} else if (lecturerNo < timeTable.size() && session == currentDay) {
				// if one day for one lecturer has been generated then move on to the next lecturer
				lecturerNo++;
				session -= 8;

			} else {
				// if one day is not yet finished, generate the next session

				// if the lecturer is busy this session, there is no room left or it's currently lunch break
				// then the lecturer will not teach, move on to the next session
				if (preference[lecturerNo][session] == 0 || roomLayout[session] == 0 || session % 8 == 3) {
					session++;

				} else {
					// the lecturer can teach in this session\=
					/* all the options that the lecturer can do:
						- teach the first course he/she has to teach
						- teach the second course he/she has to teach
						- ...
						- teach the nth course he/she has to teach
						- not teach in this session and move on to the next one
					*/

					for (int i=0; i<teachingCourses[lecturerNo].size() + 1; i++) {
						// first option: the lecturer decides not to teach
						if (i == teachingCourses.size()) {
							generateLecturer(teachingCourses, hours, preference, session + 1, lecturerNo, timeTable, taught, roomLayout);

						//second option(s): the lecturer decides to teach
						} else {
							int courseNo = teachingCourses[lecturerNo][i];

							// the lecturer can only teach the course if there is still time left and the course
							// has not been taught that day
							if (hours[courseNo] > 0 || taught[courseNo] == false) {
								generateTeach(teachingCourses, hours, preference, session, lecturerNo, courseNo, timeTable, taught, roomLayout);
							}

						}
					}

					return;

				}

			}


		}
	}







void generateTeach(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, int courseNo, vector< vector<int> > timeTable, vector<bool> taught, vector<int> roomLayout)
{
	hours[courseNo]--;								// modifying the table. by default, consider that the lecturer will teach 1 hour session
	roomLayout[session]--;
	taught[courseNo] = true;
	timeTable[lecturerNo][session] = courseNo;

	if (session % 8 == 7) {
		// if this is the last session of the day or the next session is lunch brea then the lecturer can only teach 1 hour session
		generateLecturer(teachingCourses, hours, preference, session + 1, lecturerNo, timeTable, taught, roomLayout);
		return;

	} else {
		// else, the lecturer will have the choice of either teaching 2 hours or 1 hour
		for (int i=0; i<2; i++) {
			if (i == 0) {
				// teach 1 hour session, skip 1 hour ahead as teaching break
				generateLecturer(teachingCourses, hours, preference, session + 2, lecturerNo, timeTable, taught, roomLayout);

			} else if (i == 1 && hours[courseNo] > 0 && roomLayout[session+1] > 0) {
				//teach 2 hours session, skip 1 hour ahead as teaching break
				hours[courseNo]--;								// modifying the table. by default, consider that the lecturer will teach 1 hour session
				roomLayout[session+1]--;
				timeTable[lecturerNo][session+1] = courseNo;
				generateLecturer(teachingCourses, hours, preference, session + 3, lecturerNo, timeTable, taught, roomLayout);

			}
		}
		return;
	}
}




public:
	void generate(int rooms, int courses, vector<int> hours, vector<string> names, vector<string> lecturers, vector< vector<int> > binaryMapping, vector< vector<int> > preference)
	{
		vector< vector<int> > teachingCourses = generateTeachingCourses(binaryMapping);			//convert from binaryMapping to a typical vector for faster access
		vector< vector<int> > genericTimetable = generateGenericTimetable(lecturers.size());			//a generic blank timeTable

		vector<bool> taught(courses);					//see if the course has already been taught that day
		fill(taught.begin(), taught.end(), false);

		vector<int> roomLayout(40);						//see if there is any room left throughout the day
		fill(roomLayout.begin(), roomLayout.end(), rooms);

		rooms = this->rooms;

		// for (int i=0; i<teachingCourses.size(); i++) {
		// 	for (int m=0; m<teachingCourses[i].size(); m++) {
		// 		cout << teachingCourses[i][m] << " ";
		// 	}
		// 	cout << endl;
		// }
		// cout << endl;

		generateLecturer(teachingCourses, hours, preference, 0, 0, genericTimetable, taught, roomLayout);			//the start of it all
		cout << "done" << endl;

	}
	
};