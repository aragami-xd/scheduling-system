#include "Schedule.cpp"
using namespace std;

int main()
{
	Schedule schedule;
	
// 	vector< vector<int> > preference = {
// {5,2,2,0,1,1,1,2,5,2,2,0,1,1,1,2,0,0,0,0,0,0,0,0,5,2,2,0,1,1,1,2,5,2,2,0,2,2,5,5},
// {1,1,1,5,2,2,2,5,0,0,0,0,0,0,0,0,1,1,1,5,2,2,2,5,1,1,1,5,2,2,2,5,1,1,1,5,2,2,2,5},
// {1,1,1,2,2,5,0,0,1,1,1,2,2,5,0,0,1,1,1,2,2,5,0,0,1,1,1,2,2,5,0,0,1,1,1,0,0,0,0,0},
// {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0}};

// 	vector< vector<int> > binaryMapping = {
// {0,0,1,0},
// {1,1,0,0},
// {1,0,0,0},
// {0,1,0,0},
// {0,0,0,1}};

// 	vector<int> hours = {2,3,2,4,2};
// 	int rooms = 2;
// 	int courses = 5;
// 	vector<string> names = {"ADSA", "EDC", "PSSD", "OOP", "CS"};
// 	vector<string> lecturers = {"Cruz", "Minygu", "Cheryl", "Fred"};


		vector< vector<int> > preference = {
{5,2,2,0,1,1,1,2,5,2,2,0,1,1,1,2,0,0,0,0,0,0,0,0,5,2,2,0,1,1,1,2,5,2,2,0,2,2,5,5},
{1,1,1,5,2,2,2,5,0,0,0,0,0,0,0,0,1,1,1,5,2,2,2,5,1,1,1,5,2,2,2,5,1,1,1,5,2,2,2,5}};

	vector< vector<int> > binaryMapping = {
{0,1},
{1,1},
{1,0}};

	vector<int> hours = {2,3,2};
	int rooms = 2;
	int courses = 3;
	vector<string> names = {"ADSA", "EDC", "PSSD"};
	vector<string> lecturers = {"Cruz", "Minygu"};
	




	schedule.generate(rooms, courses, hours, names, lecturers, binaryMapping, preference);
}