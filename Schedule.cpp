// include "ScheduleWhen.cpp"

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Schedule
{
private:
	int counter = 0;


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
		while (1) {
			//continue looping until hit the base case or a valid case

			if (lecturerNo == preference.size() - 1 && session >= 39) {
				// if the last day of the last lecturer has been generated, then that's all for the week, recursion function stops

				for (int i=0; i<timeTable.size(); i++) {			//just print everything out for testing purposes
					for (int m=0; m<timeTable[i].size(); m++) {
						if (timeTable[i][m] != -1) {			//print occupied hours with blue color for clarity
							cout << " \e[94m";
						}
						cout << timeTable[i][m] << "\e[0m ";
					}
					cout << endl;
				}
				cout << endl;
				return;

			} else if (preference[lecturerNo][session] == 0 || roomLayout[session] <= 0) {
				// if lecturer is not available for teaching at that moment or there is no room left to teach
				session ++;
				
			} else {			//lecturer can teach in that session

				// if the lecturer decides to teach in that session
				for (int i=0; i<teachingCourses[lecturerNo].size(); i++) {

					if (hours[ teachingCourses[lecturerNo][i] ] > 0) {
						// if there is still time left in the course and that course has not been taught that day
						generateTeach(teachingCourses, hours, preference, session, lecturerNo, teachingCourses[lecturerNo][i], timeTable, taught, roomLayout);			//if lecturer chooses to teach it
					}

				}

				if (lecturerNo == preference.size() - 1 && session % 8 == 7) {
					// if one day of the last lecturer has been generated then move back to the next day of the first lecturer
					session++;
					lecturerNo = 0;
					// fill(taught.begin(), taught.end(), false);

				} else if (lecturerNo < preference.size() - 1 && session % 8 == 7) {
					// if one day of a lecturer has been generated then move on to the next lecturer, go back to the start of the day
					session -= 7;
					lecturerNo++;
					// fill(taught.begin(), taught.end(), false);

				} else {
					// if this is not the final session of the day for that lecturer, then move on to the next session 
					session ++;

				}

			}
		}

	}








	//this fucntion will determine if the lecturer can and will teach 2 hour session or not
	void generateTeach(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, int courseNo, vector< vector<int> > timeTable, vector<bool> taught, vector<int> roomLayout)
	{

		//update the time table, subtract to hours remaining of the course, change status to taught (course has been taught that day) and update roomLayout
		timeTable[lecturerNo][session] = courseNo;
		hours[courseNo]--;
		// taught[courseNo] = true;
		roomLayout[session]--;

		//teaching 1 hour sessions
		if (session % 8 == 7 || session % 8 == 2) {
			// if this is the last hour of the day or the next hour is lunch break, then jump directly to the next hour
			generateLecturer(teachingCourses, hours, preference, session + 1, lecturerNo, timeTable, taught, roomLayout);

		} else {
			// else, jump 1 hour ahead as teaching break
			generateLecturer(teachingCourses, hours, preference, session + 2, lecturerNo, timeTable, taught, roomLayout);	
			
		}
		


		// if there are still time left in the course, next session is free and there are rooms available, then check to see if the lecturer will teach 2 hour sessionr or not
		if (preference[lecturerNo][session + 1] != 0 && hours[ courseNo ] > 1 && roomLayout[session + 1] > 0) {		

			if (session % 8 == 7) {		
				// if this is the last hour of the day then just go home, can't teach 2 hours
				return;	
				
			} else if (session % 8 == 6) {
				// if the next hour is the last hour of the day, then just move on to the next session (i.e. start of new day) and no teaching break	
				hours[courseNo]--;			
				timeTable[lecturerNo][session + 1] = courseNo;
				roomLayout[session + 1]--;
				generateLecturer(teachingCourses, hours, preference, session + 1, lecturerNo, timeTable, taught, roomLayout);

			} else if (session % 8 == 2) {
				// since the lunch break is not an official teaching session, it's not on the timetable and doesn't affect the room count
				// no courses can start during the lunch break, so there is no need to worry about the extra room
				hours[courseNo]--;	
				generateLecturer(teachingCourses, hours, preference, session + 2, lecturerNo, timeTable, taught, roomLayout);

			} else {
				// if the next hour is not the last hour of the day, then you have to jump 1 more hour as teaching break	
				hours[courseNo]--;		
				timeTable[lecturerNo][session + 1] = courseNo;
				roomLayout[session + 1]--;
				generateLecturer(teachingCourses, hours, preference, session + 3, lecturerNo, timeTable, taught, roomLayout);	

			}
		}
		
	}









public:
	void generate(int rooms, int courses, vector<int> hours, vector<string> names, vector<string> lecturers, vector< vector<int> > binaryMapping, vector< vector<int> > preferences)
	{
		vector< vector<int> > teachingCourses = generateTeachingCourses(binaryMapping);			//convert from binaryMapping to a typical vector for faster access

		vector< vector<int> > genericTimetable;			//a generic blank timetable
		for (int i=0; i<names.size(); i++) {
			genericTimetable.push_back({});
			fill(genericTimetable[i].begin(), genericTimetable[i].end(), -1);
		}

		vector<bool> taught(courses);					//see if the course has already been taught that day
		fill(taught.begin(), taught.end(), false);

		vector<int> roomLayout(40);						//see if there is any room left throughout the day
		fill(roomLayout.begin(), roomLayout.end(), rooms);

		// for (int i=0; i<teachingCourses.size(); i++) {
		// 	for (int m=0; m<teachingCourses[i].size(); m++) {
		// 		cout << teachingCourses[i][m] << " ";
		// 	}
		// 	cout << endl;
		// }
		// cout << endl;

		generateLecturer(teachingCourses, hours, preferences, 0, 0, genericTimetable, taught, roomLayout);			//the start of it all
		cout << "done" << endl;

	}
	
};