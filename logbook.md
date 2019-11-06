logbook time!

The plan:
I'll somewhat stick to basic approach given in the problem description of "when", "where" and "how". For now, i'll solve the "when" first

For the when, I'll have no other choice but to use recursion. I try to write some pseudocode using the iterative approach, but doesn't seem to work. For the recursion approach, I've thought of many different methods, but binary tree ended up being the only one that makes sense to me. So, this is my pseudocode I've written:

lecturer 1
	///check at a certain hour of the day
	free to teach? yes, move on. no, stop
		decides to teach or not? yes, move on. no, stop

			/// check a certain course in the list of courses that that lecturer teaches
			teach course 1 or not? yes, change the remaining hours of course 1 then move on. no, try with course 2,3... (the lecturer will have to teach the last course)
				if next hour is free, teach 1 hour or 2 hours? subtract remaining hours with that value
				(some conditions testing lunch break and end of the day will be here, but i'll figure that out later)

					///jump back to lecturer
					do the same with every other courses. if you get to the last day of the week and you haven't taught every courses yet, the option is out

(hope that actually make sense)




logbook time!
Update: So I've got the algorithm to somewhat follow my plan. I added the "where" part into the program and it was fairly easy: pass into the function a vector that contain the "room layout" (which tells how many rooms are available at the moment) and just skip that session if there is no room available. That one was easy enough

But I've realized a small problem with my current algorithm (that I'm not sure if it's actually right or not). Some pseudocode:
///the lecturer teaches at that session
	teach course 1 or not? yes, move on with the next function. no, call this function again with course 2

The problem with this algorithm is: it'll always skip the first course and move on to the next one, then skip it and do the same until it gets to the final course. Though, i'm not sure if this is true or not, or the algorithm is simply too slow at the moment to test it out






logbook time!
Alright, i'll start fresh again since the initial solution is no viable by any means (after all, it's just glorfied brute force). i'll now go for an optimization / iterative improvement approach

some global variables that i might use to deal with the calculations: a vector vector of result, a vector that contain the preference score of every occupied slot in the final solution, and a score of fit

i'll create an initial solution. this can be easily done iteratively through a bunch of loops (though from the pseudocode i've written, it's going to be O(n^4)). as i loop through the program, if a course can be filled into a cell of the solution vector, i'll fill in immediately and make changes to other variables. room constraints will be considered here (it's pretty easy actually). however, preference will not be considered, but all the preference scores will be saved, which later will be sorted descendingly so i can move that slot into another place

moving to another slot somehow, i'm still working on it, but for now this seems to be a good strategy for me

(i realize that this sounds pretty much like greedy algorithm)


