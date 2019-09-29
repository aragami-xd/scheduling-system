#include "ScheduleWhen.cpp"

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


	//session will point out which session of the week the lecturer is teaching
	void generateLecturer(vector<int> hours, int lecturerNo, int session, vector<vector<int>> &binaryMapping, vector<vector<int>> &preference)
	{
		
	}


	void generate(int rooms, int courses, vector<int> hours, vector<string> names, vector<string> lecturers, vector< vector<int> > binaryMapping, vector< vector<int> > preferences)
	{
		setupTimetable(lecturers.size());		//setup the timetable


	}
	
};