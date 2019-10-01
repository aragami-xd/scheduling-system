// include "ScheduleWhen.cpp"

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Schedule
{
private:
	int totalRooms;
	int counter = 0;


	//setting up a timetable with full of -1
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
	void generateLecturer(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, vector< vector<int> > timeTable, vector<int> roomLayout)
	{
		// cout << counter++ << " call generateLectuer" << lecturerNo << " " << session << endl;

		if (lecturerNo >= preference.size()) {				//every lecturer has been generated
			// counter++;
			// if (counter % 100000 == 0) {
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

			// for (int i=0; i<hours.size(); i++) {			//invalid option: doesn't teach all the required hours
			// 	cout << hours[i] << " ";
			// }	
			// cout << endl;
			// return;

			// checkRoom(timeTable);			//check if the number of rooms allows for this timetable

		} else if (session >= 40) {
			//if all 40 hours of the week have been checked, then move on to the next lecturer
			generateLecturer(teachingCourses, hours, preference, 0, lecturerNo + 1, timeTable, roomLayout);

		} else if (preference[lecturerNo][session] == 0 || roomLayout[session] <= 0) {
			// if lecturer is not available for teaching at that moment or there is no room left to teach
			generateLecturer(teachingCourses, hours, preference, session + 1, lecturerNo, timeTable, roomLayout);
			
		} else {			//lecturer can teach in that session
			//if the lecturer decides to teach in that session
			generateCourseToTeach(teachingCourses, hours, preference, session, lecturerNo, 0, timeTable, roomLayout);

			//if lecturer doesn't teach in that session, then move on to the next session
			generateLecturer(teachingCourses, hours, preference, session + 1, lecturerNo, timeTable, roomLayout);

		}
	}






	//if the lecturer decides to teach, which course to teach (since a lecturer can tech multiple courses)
	void generateCourseToTeach(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, int courseNo, vector< vector<int> > timeTable, vector<int> roomLayout)
	{
		// cout << counter++ << " call generateCourseToTeach" << endl;

		//as long as it's not the last course in the lecturer's list, he/she will have the option to not teach it in this session
		//this condition will also cover the case that there is no hour left in that course, which the lecturer will move onto the next course
		if (courseNo < teachingCourses[lecturerNo].size() - 1) {
			generateCourseToTeach(teachingCourses, hours, preference, session, lecturerNo, courseNo + 1, timeTable, roomLayout);
		}

		//if the last course the lecturer has to teach doesn't have any hours left, that means the lecturer is done for the week
		if (hours[ teachingCourses[lecturerNo][courseNo] ] == 0 && courseNo == teachingCourses[lecturerNo].size() - 1) {
			generateLecturer(teachingCourses, hours, preference, 0, lecturerNo + 1, timeTable, roomLayout);

		} else if (hours[ teachingCourses[lecturerNo][courseNo] ] > 0) {		//if there are still hours left in that course
			generateTeach(teachingCourses, hours, preference, session, lecturerNo, courseNo, timeTable, roomLayout);			//if lecturer chooses to teach it

		}		
	}





	//this fucntion will determine if the lecturer can and will teach 2 hour session or not
	void generateTeach(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, int courseNo, vector< vector<int> > timeTable, vector<int> roomLayout)
	{
		// cout << counter++ << " call generateTeach" << endl;

		timeTable[lecturerNo][session] = teachingCourses[lecturerNo][courseNo];			//modify the time table at session to mark that you'll teach at that time
		hours[ teachingCourses[lecturerNo][courseNo] ]--;								//and subtract 1 to the remaining hour of that course
		roomLayout[session]--;

		generateLecturer(teachingCourses, hours, preference, session + 2, lecturerNo, timeTable, roomLayout);		//jump an hour ahead as teaching break



		// if there is still hour left in the course, next session is free and there are rooms available, then check to see if the lecturer can teach the next session or not
		if (preference[lecturerNo][session + 1] != 0 && hours[ teachingCourses[lecturerNo][courseNo] ] > 1 && roomLayout[session + 1] > 0) {		// if lecturer is free next hour (or it's lunch break), he/she can teach 2 hours in a row
			if (session % 8 == 7) {			// if the next hour is end of day (or, this session is the last hour of the day), then do nothing
				return;	
				
			} else {				//if that's not the last hour of the day, then the lecturer can teach 2 hour sessions
				hours[ teachingCourses[lecturerNo][courseNo] ]--;
					
				if (session % 8 == 6) {
					// if the next hour is the last hour of the day, then just move on to the next session (i.e. start of new day) and no teaching break				
					timeTable[lecturerNo][session + 1] = teachingCourses[lecturerNo][courseNo];
					roomLayout[session + 1]--;
					generateLecturer(teachingCourses, hours, preference, session + 2, lecturerNo, timeTable, roomLayout);

				} else if (session % 8 == 2) {
					//since the lunch break is not an official teaching session, it's not on the timetable and doesn't affect the room count
					//no courses can start during the lunch break, so there is no need to worry about the extra room
					generateLecturer(teachingCourses, hours, preference, session + 2, lecturerNo, timeTable, roomLayout);

				} else {
					// if the next hour is not the last hour of the day, then you have to jump 1 more hour as teaching break			
					timeTable[lecturerNo][session + 1] = teachingCourses[lecturerNo][courseNo];
					roomLayout[session + 1]--;
					generateLecturer(teachingCourses, hours, preference, session + 3, lecturerNo, timeTable, roomLayout);	

				}
			}
		}
		
	}









public:
	void generate(int rooms, int courses, vector<int> hours, vector<string> names, vector<string> lecturers, vector< vector<int> > binaryMapping, vector< vector<int> > preferences)
	{
		totalRooms = rooms;
		vector< vector<int> > teachingCourses = generateTeachingCourses(binaryMapping);
		vector< vector<int> > genericTimetable = generateGenericTimetable(lecturers.size());

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

		generateLecturer(teachingCourses, hours, preferences, 0, 0, genericTimetable, roomLayout);			//the start of it all
		cout << "done" << endl;

	}
	
};