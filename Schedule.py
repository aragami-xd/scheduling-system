#dump all of the spaghetti codes here

class Schedule:
	def __init__ (self):
		pass



	#with one lecturer, see if they can teach at a certain hour or not. if they can, see if they want to teach or not
	def generateLecturer(self, teachingCourses, hour, preference, session, lecturerNo, timeTable):
		if lecturerNo > len(preference):		#generated with every single lecturers
			for i in range(timeTable):
				for m in range(timeTable[i]):
					print(timeTable[i][m], end ='')
					pass

		elif preference[lecturerNo][session] == 0:		#if lecturer is not available for teaching at that moment
			self.generateLecturer(teachingCourses, hour, preference, session + 1, lecturerNo, timeTable)


		elif session >= 40:				#if all 40 hours have been checked, setup for the next lecturer
			self.generateLecturer(teachingCourses, hour, preference, 0, lecturerNo + 1, timeTable)
			

		else:					#lecturer is available for teaching at that moment and not in the teaching break
			#if lecturer decides to teach in that session
			#we'll start with course number 0, hence 0 at the end
			#once all the teaching sessions have been made, this generateLecturer fucntion is called again, with teachingBreak = true
			self.generateCourseToTeach(teachingCourses, hour, preference, session, lecturerNo, 0, timeTable)

			#if lecturer doesn't want to teach in that session, despite being able to, then we just call this function again but move up one session
			#set teaching break again to false, just for safety purposes
			self.generateLecturer(teachingCourses, hour, preference, session + 1, lecturerNo, timeTable)			
			pass

	

	#with a lecturer that teaches a course, see which course he/she will teach
	def generateCourseToTeach(self, teachingCourses, hour, preference, session, lecturerNo, courseNumber, timeTable):
		#if this is not the last course, the lecturer has a choice of either teaching it or not (i.e. teaches other courses)
		if (courseNumber < len(teachingCourses[lecturerNo])):
			#if the lecturer doesn't want to teach that course, call this function again with the next course
			self.generateCourseToTeach(teachingCourses, hour, preference, session, lecturerNo, courseNumber + 1, timeTable)
			pass
		
		#regardless of which courseNumber it is, the lecturer will always have the option to teach that course
		self.generateTeach(teachingCourses, hour, preference, session, lecturerNo, courseNumber, timeTable)
		pass



	#with a lecturer that teaches a certain course, see if they can teach more than one hour not ot
	def generateTeach(self, teachingCourses, hour, preference, session, lecturerNo, courseNumber, timeTable):
		#if the next hour is lunch break or end of day then they can only teach for 1 hour. else, they can teach either 1 or 2
		if preference[lecturerNo][session + 1] % 8 == 0 or preference[lecturerNo][session + 1] % 8 == 3:
			hour[courseNumber] -= 1
			timeTable[lecturerNo][session] = courseNumber	#1 hour teaching
			self.generateLecturer(teachingCourses, hour, preference, session + 2, lecturerNo, timeTable)		#teachingBreak true, jump to the next free hour
			pass
		
		#teaching 2 hour session
		hour[courseNumber] -= 2
		timeTable[lecturerNo][session] = courseNumber
		timeTable[lecturerNo][session + 1] = courseNumber
		self.generateLecturer(teachingCourses, hour, preference, session + 3, lecturerNo, timeTable)		#jump 3 hours to the next free session
		pass

	


			









def main():
	binaryMapping = [[0,0,1,0],[1,1,0,0],[1,0,0,0],[0,1,0,0],[0,0,0,1]]
	preference = [[5,2,2,0,1,1,1,2,5,2,2,0,1,1,1,2,0,0,0,0,0,0,0,0,5,2,2,0,1,1,1,2,5,2,2,0,2,2,5,5],[1,1,1,5,2,2,2,5,0,0,0,0,0,0,0,0,1,1,1,5,2,2,2,5,1,1,1,5,2,2,2,5,1,1,1,5,2,2,2,5],[1,1,1,2,2,5,0,0,1,1,1,2,2,5,0,0,1,1,1,2,2,5,0,0,1,1,1,2,2,5,0,0,1,1,1,0,0,0,0,0],[1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0]]
	blankTimetable = [[-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,],[-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,],[-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,],[-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,]]
	hours = [2,3,2,4,2]
	# rooms = 2 
	# courses = 5 
	# names = ["ADSA", "EDC", "PSSD", "OOP", "CS"]
	lecturers = ["Cruz", "Minygu", "Cheryl", "Fred"]

	timeTable = Schedule()
	teachingCourses = [[1,2],[1,3],[0],[4]]
	timeTable.generateLecturer(teachingCourses, hours, preference, 0, 0, blankTimetable)




if __name__ == "__main__":
	main()
	pass