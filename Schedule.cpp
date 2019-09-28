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
	void setupTimetable(int noLecturers)			//preocuppy the table with a bunch of -1
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


	void setupRoom(int rooms)				// reset the number of available rooms throughout the day
	{
		roomsLayout.clear();
		for (int i=0; i<8; i++) {
			roomsLayout.push_back(rooms);
		}
	}



	void generatePerDay(vector<string> &lecturers, vector<int> hours, vector< vector<int> > &binaryMapping, vector< vector<int> > &preference)
	{
		for (int i=0; i<5; i++) {

		}
	} 

	void generatePerHour(vector<string> &lecturers, vector<int> hours, vector< vector<int> > &binaryMapping, vector< vector<int> > &preference, int day) {
		for (int i=0; i<8; i++) {
		}
	}


	void generate(int rooms, int courses, vector<int> hours, vector<string> names, vector<string> lecturers, vector< vector<int> > binaryMapping, vector< vector<int> > preferences)
	{
		setupTimetable(lecturers.size());		//setup the timetable


	}
	
};