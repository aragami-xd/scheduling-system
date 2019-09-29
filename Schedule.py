#dump all of the spaghetti codes here

class Schedule:
	def __init__ (self):
		pass



	#with one lecturer, see if they can teach at a certain hour or not. if they can, see if they want to teach or not
	def generateLecturer(self, teachingCourses, hour, preference, session, lecturerNo, timeTable):
		if lecturerNo >= len(preference):		#generated with every single lecturers
			for i in range(len(timeTable)):
				for m in range(len(timeTable[i])):
					print(timeTable[i][m], end ='')
					pass
			print(end='\n')
			pass

		elif session >= 40:				#if all 40 hours have been checked, setup for the next lecturer
			self.generateLecturer(teachingCourses, hour, preference, 0, lecturerNo + 1, timeTable)

		elif preference[lecturerNo][session] == 0:		#if lecturer is not available for teaching at that moment
			self.generateLecturer(teachingCourses, hour, preference, session + 1, lecturerNo, timeTable)		

		else:					#lecturer is available for teaching at that moment
			#if lecturer decides to teach in that session. we'll start with course number 0, hence 0 at the end
			#once all the teaching sessions have been made, this generateLecturer fucntion is called again
			self.generateCourseToTeach(teachingCourses, hour, preference, session, lecturerNo, 0, timeTable)

			#if lecturer doesn't want to teach in that session, despite being able to, then we just call this function again but move up one session
			self.generateLecturer(teachingCourses, hour, preference, session + 1, lecturerNo, timeTable)			
			pass

	

	#with a lecturer that teaches a course, see which course he/she will teach
	def generateCourseToTeach(self, teachingCourses, hour, preference, session, lecturerNo, courseNo, timeTable):
		#if this is not the last course, the lecturer has a choice of either teaching it or not (i.e. teaches other courses)
		if courseNo == len(teachingCourses[lecturerNo]):		#if we made it to the last course that that lecturer has to teach
			pass
		else:
			if courseNo < len(teachingCourses[lecturerNo]) or hour[courseNo] == 0:
				#if the lecturer doesn't want to teach that course, call this function again with the next course; or if that course has been fully taught
				self.generateCourseToTeach(teachingCourses, hour, preference, session, lecturerNo, courseNo + 1, timeTable)
				pass
			
			#regardless of which courseNo it is, the lecturer will always have the option to teach that course
			self.generateTeach(teachingCourses, hour, preference, session, lecturerNo, courseNo, timeTable)
			pass



	#with a lecturer that teaches a certain course, see if they can teach more than one hour not ot
	def generateTeach(self, teachingCourses, hour, preference, session, lecturerNo, courseNo, timeTable):
		# #if the next hour is lunch break or end of day then they can only teach for 1 hour. else, they can teach either 1 or 2
		# if (session + 1) % 8 != 0 or (session + 1) % 8 != 3:
		# 	hour[courseNo] -= 1					#subtract 1 hour to that course and change the timetable
		# 	timeTable[lecturerNo][session] = courseNo	
		# 	self.generateLecturer(teachingCourses, hour, preference, session + 3, lecturerNo, timeTable)		#teachingBreak true, jump to the next free hour
		# 	pass
		
		# # teaching 1 hour session
		# hour[courseNo] -= 2				#subtract 2 hours to that course and change the timetable
		# timeTable[lecturerNo][session] = courseNo
		# timeTable[lecturerNo][session + 1] = courseNo
		# self.generateLecturer(teachingCourses, hour, preference, session + 3, lecturerNo, timeTable)		#jump 3 hours to the next free session
		# pass

		
		timeTable[lecturerNo][session] = courseNo			#modify the time table
		#these if conditions will see if the lecturer can teach two hour sessions or not
		if preference[lecturerNo][session + 1] != -1 and hour[courseNo] > 1:		#if lecturer is free next hour, he/she can teach 2 hours in a row
			if session % 8 == 7:			#if the next hour is end of day (or, this session is the last hour of the day), then do nothing
				pass	

			elif session % 8 == 6:			#if the next hour is the last hour of the day, then just move on to the next session
				hour[courseNo] -= 2
				timeTable[lecturerNo][session + 1] = courseNo
				self.generateLecturer(teachingCourses, hour, preference, session + 2, lecturerNo, timeTable)

				hour[courseNo] += 2
				timeTable[lecturerNo][session + 1] = -1				#undo the changes to the timetable since you'll need it again

			else:							#if the next hour is not the last hour of the day, then you have to jump 1 more hour as teaching break
				hour[courseNo] -= 2
				timeTable[lecturerNo][session + 1] = courseNo
				self.generateLecturer(teachingCourses, hour, preference, session + 3, lecturerNo, timeTable)
				
				hour[courseNo] += 2
				timeTable[lecturerNo][session + 1] = -1
				pass
			pass
		
		self.generateLecturer(teachingCourses, hour, preference, session + 2, lecturerNo, timeTable)

		

	


			









def main():
	# binaryMapping = [[0,0,1,0],[1,1,0,0],[1,0,0,0],[0,1,0,0],[0,0,0,1]]
	# preference = [[5,2,2,0,1,1,1,2,5,2,2,0,1,1,1,2,0,0,0,0,0,0,0,0,5,2,2,0,1,1,1,2,5,2,2,0,2,2,5,5],[1,1,1,5,2,2,2,5,0,0,0,0,0,0,0,0,1,1,1,5,2,2,2,5,1,1,1,5,2,2,2,5,1,1,1,5,2,2,2,5],[1,1,1,2,2,5,0,0,1,1,1,2,2,5,0,0,1,1,1,2,2,5,0,0,1,1,1,2,2,5,0,0,1,1,1,0,0,0,0,0],[1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0]]
	# blankTimetable = [[-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,],[-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,],[-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,],[-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,]]
	# hours = [2,3,2,4,2]
	# # rooms = 2 
	# # courses = 5 
	# # names = ["ADSA", "EDC", "PSSD", "OOP", "CS"]
	# lecturers = ["Cruz", "Minygu", "Cheryl", "Fred"]
	
	# teachingCourses = [[1,2],[1,3],[0],[4]]

	# binaryMapping = [[0,1],[1,0]]
	preference = [[5,2,2,0,1,1,1,2,5,2,2,0,1,1,1,2,0,0,0,0,0,0,0,0,5,2,2,0,1,1,1,2,5,2,2,0,2,2,5,5],[1,1,1,5,2,2,2,5,0,0,0,0,0,0,0,0,1,1,1,5,2,2,2,5,1,1,1,5,2,2,2,5,1,1,1,5,2,2,2,5]]
	blankTimetable = [[-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,],[-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,]]
	hours = [2,3]
	# rooms = 2 
	# courses = 5 
	# names = ["ADSA", "EDC", "PSSD", "OOP", "CS"]
	# lecturers = ["Cruz", "Minygu"]
	
	teachingCourses = [[1],[0]]

	timeTable = Schedule()
	timeTable.generateLecturer(teachingCourses, hours, preference, 0, 0, blankTimetable)




if __name__ == "__main__":
	main()
	pass