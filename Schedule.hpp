#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <set>
#include <fstream>
#include <algorithm>
// #include "EvalUCS.hpp"
using namespace std;

class Schedule
{
private:
	vector< vector<int> > solution;
	double fit = 0;


	//print the solution into an xml file
	void output()
	{
		ofstream xml;
		xml.open("solution.xml");
		for (int i=0; i<solution.size(); i++) {
			for (int m=0; m<solution[i].size() - 1; m++) {
				//add individual numbers in, followed by a comma
				xml << to_string( solution[i][m] ) << ",";						
			}

			//last value will not have the comma
			xml << to_string( solution[i][solution[i].size() - 1] ) << "\n";	
		}
		xml.close();
		return;
	}


public:
	void generate(int rooms, int courses, vector<int> hours, vector<string> names, vector<string> lecturers, vector< vector<int> > binaryMapping, vector< vector<int> > preference)
	{
		//create a blank solution 
		for (int i=0; i<lecturers.size(); i++) {					
			solution.push_back({});
			for (int m=0; m<40; m++) {
				solution.back().push_back(-1);
			}
		}

		//output the xml file
		output();
		return ;
	}
	
};