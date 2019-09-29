#dump all of the spaghetti codes here

class Schedule:
	def __init__ (self):
		pass

	def scheduleWhen(self, lecturers, hour, binaryMapping, preference):
		for day in range(5):				# 5 days a week
			for session in range(8):				# 8 hours a day
				for people in range(len(lecturers)):			#lecturers

					
					if preference[people][day * 5 + session] != 0:			# if lecturer can teach at that time

						# either 0 hour, 1 hour or 2 hours being taught that day, as long as the course have enough time for it
						#it's already "n^3" at this point, doesn't look too good so far. but there isn't any way i think i can do better than this
						pass




def main():
	binaryMapping = [[0,0,1,0],[1,1,0,0],[1,0,0,0],[0,1,0,0],[0,0,0,1]]
	preference = [[5,2,2,0,1,1,1,2,5,2,2,0,1,1,1,2,0,0,0,0,0,0,0,0,5,2,2,0,1,1,1,2,5,2,2,0,2,2,5,5],[1,1,1,5,2,2,2,5,0,0,0,0,0,0,0,0,1,1,1,5,2,2,2,5,1,1,1,5,2,2,2,5,1,1,1,5,2,2,2,5],[1,1,1,2,2,5,0,0,1,1,1,2,2,5,0,0,1,1,1,2,2,5,0,0,1,1,1,2,2,5,0,0,1,1,1,0,0,0,0,0],[1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0]]

	hours = [2,3,2,4,2]
	# rooms = 2 
	# courses = 5 
	# names = ["ADSA", "EDC", "PSSD", "OOP", "CS"]
	lecturers = ["Cruz", "Minygu", "Cheryl", "Fred"]

	timeTable = Schedule()
	timeTable.scheduleWhen(lecturers, hours, binaryMapping, preference)




if __name__ == "__main__":
	main()
	pass