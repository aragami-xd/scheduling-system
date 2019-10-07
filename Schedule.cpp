// include "ScheduleWhen.cpp"

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Schedule
{
private:
	int counter = 0;
	int rooms = 0;


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
	void generateLecturer(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, vector< vector<int> > timeTable, vector<bool> taught, vector<int> roomLayout)
	{
		cout << "crash at generateLecturer " << lecturerNo << " " << session << endl;
		while (1) {
			//continue looping until hit the base case or a valid case

			if (preference[lecturerNo][session] == 0 || roomLayout[session] < 1 || session % 8 == 3) {
				// if lecturer is not available for teaching at that moment, no room left to teach or that session is the lunch break, then move on
				session ++;
				
			} else {			
				//lecturer can teach in that session, then the lecturer will either teach one of the course he/she is supposed to teach, or skip the session

				//if the lecturer decides to teach
				for (int i=0; i<teachingCourses[lecturerNo].size(); i++) {			//test with all courses that the lecturer has to teach
					if (hours[ teachingCourses[lecturerNo][i] ] > 0 && taught[ teachingCourses[lecturerNo][i] ] == false) {				
						//if there is still time left to teach in that course and it has not been taught that day

						if (session % 8 == 7) {
							//if this the last session of the day then the lecturer can only teach 1 hour
							lastHour(teachingCourses, hours, preference, session, lecturerNo, teachingCourses[lecturerNo][i], timeTable, taught, roomLayout);

						} else if (session % 8 == 6) {
							//if the next session is the last session of the day then the lecturer can teach 2 hour session then teach the next one immediately (bc you take a break at home)
							generateOneHour(teachingCourses, hours, preference, session, lecturerNo, teachingCourses[lecturerNo][i], 1, timeTable, taught, roomLayout);
							generateTwoHour(teachingCourses, hours, preference, session, lecturerNo, teachingCourses[lecturerNo][i], 0, timeTable, taught, roomLayout);

						} else if (session % 8 == 2) {
							//if the next session is the lunch break then you can only teach 1 hour session
							generateOneHour(teachingCourses, hours, preference, session, lecturerNo, teachingCourses[lecturerNo][i], 2, timeTable, taught, roomLayout);

						} else {
							//teach 2 hour session then take an extra hour break
							generateOneHour(teachingCourses, hours, preference, session, lecturerNo, teachingCourses[lecturerNo][i], 2, timeTable, taught, roomLayout);
							generateTwoHour(teachingCourses, hours, preference, session, lecturerNo, teachingCourses[lecturerNo][i], 2, timeTable, taught, roomLayout);

						}
					}
				}

				//else if the lecturer decides not to teach
				session++;

			}
		}

		return;
	}





	void lastHour(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, int courseNo, vector< vector<int> > timeTable, vector<bool> taught, vector<int> roomLayout)
	{
		cout << "crash at last hour " << lecturerNo << " " << session << endl;
		//modify the data
		timeTable[lecturerNo][session] = courseNo;				//modify the data and call the generateLecturer function agian
		hours[courseNo]--;										//courseNo hold the id number of the course (not related to the lecturer)



		//different cases will have different case of calling the recursion function
		if (lecturerNo < preference.size() - 1) {
			//if this is not the final lecturer, check the next lecturer						
			generateLecturer(teachingCourses, hours, preference, session - 8, lecturerNo + 1, timeTable, taught, roomLayout);

		} else if (lecturerNo == preference.size() - 1 && session == 39) {
			//if this is the final lecturer and this is the final hour of the week then function is done
			cout << "1 cycle done" << endl;
			return;

		} else {
			//if this is the final lecturer but not the final hour of the week then move on to the next day with first lecturer
			fill(taught.begin(), taught.end(), false);				//reset the taught vector and roomLayout vector for the next day
			fill(roomLayout.begin(), roomLayout.end(), rooms);

			generateLecturer(teachingCourses, hours, preference, session + 1, 0, timeTable, taught, roomLayout);

		}
	}




	void generateOneHour(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, int courseNo, int skip, vector< vector<int> > timeTable, vector<bool> taught, vector<int> roomLayout)
	{
		cout << "crash at 1 hour " << lecturerNo << " " << session << endl;
		timeTable[lecturerNo][session] = courseNo;				//modify the data and call the generateLecturer function agian
		hours[courseNo]--;										//courseNo hold the id number of the course (not related to the lecturer)
		taught[courseNo] = true;
		roomLayout[session]--;

		generateLecturer(teachingCourses, hours, preference, session + skip, lecturerNo, timeTable, taught, roomLayout);
	}




	void generateTwoHour(vector< vector<int> > &teachingCourses, vector<int> hours, vector< vector<int> > &preference, int session, int lecturerNo, int courseNo, int skip, vector< vector<int> > timeTable, vector<bool> taught, vector<int> roomLayout)
	{
		cout << "crash at 2 hour " << lecturerNo << " " << session << endl;
		if (hours[courseNo] < 2) {								//if there are more than 1 hour left to teach
			return;
		}

		timeTable[lecturerNo][session] = courseNo;				//modify the data and call the generateLecturer function agian
		timeTable[lecturerNo][session + 1] = courseNo;
		hours[courseNo] -= 2;									//courseNo hold the id number of the course (not related to the lecturer)
		taught[courseNo] = true;
		roomLayout[session]--;
		roomLayout[session + 1]--;

		generateLecturer(teachingCourses, hours, preference, session + skip + 1, lecturerNo, timeTable, taught, roomLayout);
	}






public:
	void generate(int rooms, int courses, vector<int> hours, vector<string> names, vector<string> lecturers, vector< vector<int> > binaryMapping, vector< vector<int> > preferences)
	{
		vector< vector<int> > teachingCourses = generateTeachingCourses(binaryMapping);			//convert from binaryMapping to a typical vector for faster access

		vector< vector<int> > genericTimetable = generateGenericTimetable(lecturers.size());			//a generic blank timetable

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

		generateLecturer(teachingCourses, hours, preferences, 0, 0, genericTimetable, taught, roomLayout);			//the start of it all
		cout << "done" << endl;

	}
	
};