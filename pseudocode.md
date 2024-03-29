for (int i=0; i<8; i++)
{
	if (preferenece[m][i] != 0) {
		callfunction(preferenece, )
	}
}


loop 0-5 for 5 days
	loop 0-8 for 8 hours a day
		if (preferenece[i][m] == 0)
			loop 0-? for courses
				if (lecturer teaches that course && still have hour left && haven't taught that course today)
					call function?

//recursion or iteration?
recursion: slower and less efficient, but easier to generate all the different permutations
iteration: faster and much more efficient, but much harder to generate all the different permutations



using map or vector of maps instead? map a lecturer with all the hours they can teach that a week
or tuple?
map<string, int> teachingHours;			//string = lecturer name + hour, int = course to take


eliminate room: probably the easiest part
loop 0-40 for 40 hours/week
	if > 2 classes runnning at a time -> invalid:
		return;
	else:
		move on to checking preference



for day in range 5:		//5 days a week
	for session in range 8:		//8 hours a day
		for duration in range min(hour + 1, 3):		//either 0 hour, 1 hour or 2 hours being taught that day, as long as the course have enough time for it



generate as binary tree?
lecturer 1
	///(hour of the day)
	free to teach? yes, move one. no, stop
		teach or not? yes, move on. no, stop
			///
			teach course 1 or not? yes, mod to course 1 then move on. no, move on
				if next hour is free, teach 1 hour or 2 hours? if 1, sub 1, else, sub 2. subtract remaining hours with that value
				(if the next hour is not lunch break or end of day, change it to busy so you cannot teach. lunch % 8 = 3. end of day % 8 = 0)
					///
					do the same with every other courses. if you get to the last day of the week and you haven't taught every courses yet, the option is out
						///
(yep, this one works, but missing conditions)



program flow of control simulation:
(1 iteration)
- call else from generateLecturer with session 0, call generateCourseToTeach
- call else from generateCourseToTeach with course 0, call generateCourseToTeach (since first lecturer teaches 2 courses)
- call else from generateCourseToTeach with course 1, call generateTeach
- call generateLecturer from generateTeach with 1 hour course
(2 iterations)
(many iterations later)
- call generateLecturer from generateTeach with 1 hour course, session 40



two current problems: it will always skip through the first course and just teach the second course
duplicate generations: many generations return the same result


back to the drawing board then: use the original. pseudocode:
loop 1 day for 1 lecturer:
	if last lecturer, last day of the week:
		print out
	else if not last lecturer, last hour of the day:
		call function
		back to first lecturer, next day of the week
	else if not last lecturer, last hour of the day (including last day of the week):
		call function
		go to next lecturer
	else if busy, lunch break or doesn't have any rooms left:
		move on to next hour
	else if not busy and has room:
		teach course 1 if there is still hour left
		teach course 2 if there is still hour left
		...
		don't teach, move on


teach function:
if last hour of the day:
	teach 1 hour session only and don't jump ahead since the previous function will take care of it (by just return rather than call the previous function)
else:
	teach 1 hour session then jump ahead to the next hour of the day
if last hour of the day:
	do nothing since you can only teach 1 hour session
else if next hour is the last hour of the day:
	teach 2 hour session then return and the previous function will take care of it
else if next hour is the lunch break:
	teach 1 hour session only since you have to take the lunch break
else:
	teach 2 hour session then you take the lunch break


- use shell time limit to stop the program when a certain time threashold is met:
$ timeout <time-limit-interval> <command>
-> $ make all
-> $ timeout 59s ./a.out



- other option: keep track of the time in c++. save a time stamp using clock of when the function starts. once the clock hits 60s or before it, stop every single function and return the value







alright, start fresh. brute force like that is not a great option by any mean. we'll go straight to the result then trying to improve it, like a optimization / iterative improvement problem
based on what cruz said, you can start the problem randomly or from any where you wanted to. i'll start from the first valid solution i can create which is created using typical iteration approach. i'll keep track of the preference score of the lecturer in a vector (i.e. the preference score of every session that the lecturer teaches), sort it so i can deal with the highest score first




logbook time!
Alright, i'll start fresh again since the initial solution is no viable by any means (after all, it's just glorfied brute force). i'll now go for an optimization / iterative improvement approach

some global variables that i might use to deal with the calculations: a vector vector of result, a vector that contain the preference score of every occupied slot in the final solution, and a score of fit

i'll create an initial solution. this can be easily done iteratively through a bunch of loops (though from the pseudocode i've written, it's going to be O(n^4)). as i loop through the program, if a course can be filled into a cell of the solution vector, i'll fill in immediately and make changes to other variables. room constraints will be considered here (it's pretty easy actually). however, preference will not be considered, but all the preference scores will be saved, which later will be sorted descendingly so i can move that slot into another place

moving to another slot somehow, i'm still working on it, but for now this seems to be a good strategy for me

(i realize that this sounds pretty much like greedy algorithm)





so after having the ascendingly sorted preference score vector, i'll try to extend every 1 hour session into 2 hour session. the reason is that teaching 2 hour session is always more "efficient" than 1: while there is more room for room overflow (pardon the pun), it opens up more free space to relocate the bad slots



so i think the code works now. i have managed to generate a solution, though it's not a viable solution. during the swap or the relocate function, i have messed up the initial solution somehow. but, it looks like this algorithm will be able to give me a correct final solution




generate an initial solution
go from the start to the end, move every bad solution to:
- the best place it can move to then move all of the invalid coureses away but way too complex
- a valid place but may not be able to find one


everything need to take in consideration when move a cell:
- room count that day
- the course has been taught that day or not
- the lecturer has taught that day or not, if not, they cannot teach 2 hour session after it -> move that course away
- is there any courses before and after it or not, if yes then you'll have to move both of them away



- insertRemaining:
- loop through every lecturer, every hour of the week. if the lecturer can teach the course at that time then insert it in
- move one cell away
- note: might have to set a flag that the cell cannot be moved anywhere, which shouldn't happen since it can always move back to it's own place
- anyways: 