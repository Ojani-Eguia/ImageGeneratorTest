Ojani Eguia
287 Term project
Image Generator / Processor
ReadMe File

Table of Contents
1. Platform Used
2. External Files / Scripts used
3. How to Run code
	a. In-file Variable Parameters
	b. Compiling
4. Citations / Wroks Cited







1. Platform Used
	For all three versions of this program, the C programming language was used.	
	Sequential and OpenMP version were compiled with gcc directly, and the MPI
	version uses a makefile given by the 287 course (credit to JD.) 

2. External Files / Scripts used
	There are no input files, but the code does generate an output file which is 
	always called 'first.ppm', and this file appears in the same folder that you 
	execute the respective program from.

	In addition, code that converts an array to a ppm file is used from the 	
	following website:
	http://rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C

3. How to Run code

	a. In-file Variable Parameters
		In all three files, there are some variables at the top that determine
		how the program is run. In all three versions, you will find:

		-printing
			When set to anything above 0, will print debugging statements.
			Higher number will print more types of debug statements.
		-dimention
			This determines the length of the side of the output ppm file.
			The output file will be a dimention*dimention size ppm file.
		-generation
			This determines how the inputs will be generated. The options 
			avalible are whether the base file is random or the set matrix 
			from http://rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C, and 
			how the base file is modified.
			Put simply:

			generation == 0 will make set matrix with no color shift
			generation == 1 will make random base with no color shift
			generation == 2 will make set matrix with red color shift
			generation == 3 will make random base with red color shift
			generation == 4 will make set matrix with green color shift
			generation == 5 will make random base with green color shift
			generation == 6 will make set matrix with blue color shift
			generation == 7 will make random base with blue color shift
			
			Note: Setting generation to any value not mentioned above will
			create a random base with random shifts for each pixel. 

		There is also a variable that is only avalible for the OpenMP version:
		-nthreads
			This determines the number of threads that will run the program.
			Maximum of 8 on a typical H110 machine.

	b. Compiling
		Compiling is slightly different for each of the three versions.

		Sequential: In terminal, locate the sequential folder of this project. 	
		Once there, type the following command:
			gcc -o Seq SeqShader.c
		This will compile the source code and create the executable 'Seq'. 
		Simply run the executable to run the program.



		OpenMP: In terminal, locate the sequential folder of this project. 	
		Once there, type the following command:
			gcc -fopenmp -o ParShader ParShader.c
		This will compile the source code linked with OpenMP and create the 
		executable 'ParShader'. Simply run the executable to run the program.


		
		MPI: In terminal, locate the sequential folder of this project. 	
		Once there, type the following commands:
			make
			mpirun MPIShader
		This will compile the source code using the built-in Makefile, as well as
		execute the program with 4 threads (if you are using H110 Computer). If
		choose to run MPI with a different amount of threads, or on different 			computers, you can use this instead of "mpirun MPIShader":
			mpirun -np <Number of Threads> -- hostfile <Hostfile> MPIShader
		An example: (2 threads, using hostfile "QuaCS")
			mpirun -np 2 -- hostfile QuaCS MPIShader
		Note that I was not able to get these substititue commands to work
		when running my own tests. 


4. Citations / Wroks Cited

	Source of converting array to ppm / source of cool Amazing Matrix file;
	http://rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C

	How to generate single sheet of color ppm:
	https://people.cs.clemson.edu/~levinej/courses/S15/1020/handouts/lec05/creatingPPM.pdf
