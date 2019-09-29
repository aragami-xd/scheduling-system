// include "ScheduleWhen.cpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
using namespace std;

class Schedule
{
private:
	vector< vector<int> > timetable;
	vector<int> roomsLayout;

public:
	//setting up a timetable with full of -1
	void setupTimetable(int noLecturers)
	{
		timetable.clear();
		for (int i=0; i<noLecturers; i++) {
			timetable.push_back({});
			for (int m=0; m<40; m++) {
				timetable[i].push_back(-1);
			}
		}

		for (int i=0; i<timetable.size(); i++) {
			for (int m=0; m<timetable[0].size(); m++) {
				cout << timetable[i][m] << ", ";
			}
			cout << endl;
		}
	}


	//setting up the number of rooms remaining throughout the day
	void setupRoom(int rooms)
	{
		roomsLayout.clear();
		for (int i=0; i<8; i++) {
			roomsLayout.push_back(rooms);
		}
	}


	//determining if the lecturer can teach at this session, and if can, decides to teach or not
	void generateLecturer(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, vector< vector<int> > timeTable)
	{
		if (lecturerNo >= preference.size()) {				//every lecturer has been generated
			for (int i=0; i<timeTable.size(); i++) {			//just print everything out for testing purposes
				for (int m=0; m<timeTable[i].size(); m++) {
					cout << timeTable[i][m] << " ";
				}
				cout << endl;
			}
			cout << endl;

		} else if (session >= 40) {			//all 40 hours have been checked
			generateLecturer(teachingCourses, hours, preference, 0, lecturerNo + 1, timeTable);

		} else if (preference[lecturerNo][session] == 0) {			//if lecturer is not available for teaching at that session
			generateLecturer(teachingCourses, hours, preference, session + 1, lecturerNo, timeTable);
			
		} else {			//lecturer can teach in that session
			//if the lecturer decides to teach in that session
			generateCourseToTeach(teachingCourses, hours, preference, session, lecturerNo, 0, timeTable);

			//if lecturer doesn't teach in that session, then move on to the next session
			generateLecturer(teachingCourses, hours, preference, session + 1, lecturerNo, timeTable);

		}
	}



	//if the lecturer decides to teach, which course to teach (since a lecturer can tech multiple courses)
	void generateCourseToTeach(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, int courseNo, vector< vector<int> > timeTable)
	{
		//if this is not the last course, the lecturer has a choice of either teaching it or not (i.e. teaches other courses)
		if (courseNo == teachingCourses[lecturerNo].size()) {
			return;

		} else {			//if this is not the last course, the lecturer can decide
			if (courseNo < teachingCourses[lecturerNo].size() || hours[courseNo] == 0) {
				// if the lecturer doesn't want to teach that course, call this function again with the next course; or if that course has been fully taught
				generateCourseToTeach(teachingCourses, hours, preference, session, lecturerNo, courseNo + 1, timeTable);
									
				// regardless of which courseNo it is, the lecturer will always have the option to teach that course
				generateTeach(teachingCourses, hours, preference, session, lecturerNo, courseNo, timeTable);
			}
		}
	}



	//this fucntion will determine if the lecturer can and will teach 2 hour session or not
	void generateTeach(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, int courseNo, vector< vector<int> > timeTable)
	{
		timeTable[lecturerNo][session] = courseNo;			//since you'll have to teach at that session, modify the time table at session to mark that you'll teach at that time
		hours[courseNo]--;
		generateLecturer(teachingCourses, hours, preference, session + 2, lecturerNo, timeTable);

		// these if conditions will see if the lecturer can teach two hour sessions or not
		if (preference[lecturerNo][session + 1] != -1 && hours[courseNo] > 1) {		// if lecturer is free next hour, he/she can teach 2 hours in a row
			if (session % 8 == 7) {			// if the next hour is end of day (or, this session is the last hour of the day), then do nothing
				return;	
			}

			else if (session % 8 == 6) {		// if the next hour is the last hour of the day, then just move on to the next session (i.e. start of new day)
				hours[courseNo]--;
				timeTable[lecturerNo][session + 1] = courseNo;
				generateLecturer(teachingCourses, hours, preference, session + 2, lecturerNo, timeTable);

				hours[courseNo]++;
				timeTable[lecturerNo][session + 1] = -1;			// undo the changes to the timetable since you'll need it again

			} else {							// if the next hour is not the last hour of the day, then you have to jump 1 more hour as teaching break
				hours[courseNo]--;
				timeTable[lecturerNo][session + 1] = courseNo;
				generateLecturer(teachingCourses, hours, preference, session + 3, lecturerNo, timeTable);

				hours[courseNo]++;
				timeTable[lecturerNo][session + 1] = -1;
			
			}
		}
		

	}



	void generate(int rooms, int courses, vector<int> hours, vector<string> names, vector<string> lecturers, vector< vector<int> > binaryMapping, vector< vector<int> > preferences)
	{
		setupTimetable(lecturers.size());		//setup the timetable


	}
	
};