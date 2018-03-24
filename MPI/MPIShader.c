/*
Ojani Eguia 
287 Term Project
Image Generator / Procressor
MPI Version
*/
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>   /* PROVIDES THE BASIC MPI DEFINITION AND TYPES */
//#include <string.h>
//#define SEED 35791247

int main(int argc, char **argv){

	int my_rank; 
	int size;

	// to get node name, from http://www.cisl.ucar.edu/docs/lightning/examples/mpi.jsp
	int name_len;
	char nodename[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv); /*START MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); /*DETERMINE RANK OF THIS PROCESSOR*/
	MPI_Comm_size(MPI_COMM_WORLD, &size); /*DETERMINE TOTAL NUMBER OF PROCESSORS*/
	MPI_Get_processor_name(nodename, &name_len);

//	MPI Setup: Since this version generates values from scratch, Threads have 
//	Been set up to only send results back to main thread, since there is no
//	need to send starting data. 
//
//	-All threds (except main thread) generate numbers and calculate answers
//	-these threads then send back calculated data back to main thread
//	-main thread gathers data and puts it into the new ppm file.

	int printing = 0;
	int dimention = 256;
	int workerThreads = size-1;
        int segment = dimention/workerThreads;
	int i = 0,j = 0; // Indicies
	srand(time(NULL));   // should only be called once

	int generation = 2;
	/*
	Instrctions for Generate:

	For First Input:
	Even numbers start with awesome base, 
	Odd numbers generate random fields.

	For Sencond Input:
	0/1 is no change
	2/3 is Shift to red
	4/5 is Shift to green
	6/7 is Shift to blue
	anything else is random


	NOTE: Cool array is thanks to http://rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C

	*/

	if(my_rank != 0){// for non master threads
		int Input1[segment][dimention][3];
		int Input2[segment][dimention][3];
		int SmallOut[segment][dimention][3];
		for (i = 0; i < segment; i++){
			for (j = 0; j < dimention; j++){
				int a, b, c;
				if((generation % 2) == 0){//cool array
					a = (i + (segment*(my_rank-1))) % 256;		/* red */
					b = j % 256;					/* green */
					c = ((i+ ((segment*(my_rank-1)))) * j) % 256;	/* blue */
				}
				else{//random 
					a = rand()%0x100;
					b = rand()%0x100;
					c = rand()%0x100;
				}
				Input1[i][j][0] = a;
				Input1[i][j][1] = b;
				Input1[i][j][2] = c;
			}
		}

		for (i = 0; i < segment; i++){
			for (j = 0; j < dimention; j++){
				int a, b, c;
				if((generation == 1) || (generation == 0)){//no change
					a = 0xff;
					b = 0xff;
					c = 0xff;
				}
				else if((generation == 2) || (generation == 3)){//red
					a = 0xff;
					b = 0x80;
					c = 0x80;
				}
				else if((generation == 4) || (generation == 5)){//green
					a = 0x80;
					b = 0xff;
					c = 0x80;
				}
				else if((generation == 6) || (generation == 7)){//blue
					a = 0x80;
					b = 0x80;
					c = 0xff;
				}
				else{//random
					a = rand()%0x100;
					b = rand()%0x100;
					c = rand()%0x100;
				}
				Input2[i][j][0] = a;
				Input2[i][j][1] = b;
				Input2[i][j][2] = c;
			}
		}

		if (printing >= 3){
			sleep(2*my_rank); //easier to read print output this way
			printf("Here is Input 1 before Multiplication: \n");
			int a = 0, b = 0;// Indicies
			for (a = 0; a < segment; a++){
				for (b = 0; b < dimention; b++){
					printf("%.2x ,", Input1[a][b][0]);
					printf("%.2x ,", Input1[a][b][1]);
					printf("%.2x \t", Input1[a][b][2]);
				}
				printf("\n");
			}
		}

		if (printing >= 3){
			sleep(2*my_rank); //easier to read print output this way
			printf("Here is Input 2 before Multiplication: \n");
			int a = 0, b = 0;// Indicies
			for (a = 0; a < segment; a++){
				for (b = 0; b < dimention; b++){
					printf("%.2x ,", Input2[a][b][0]);
					printf("%.2x ,", Input2[a][b][1]);
					printf("%.2x \t", Input2[a][b][2]);
				}
				printf("\n");
			}
		}

		//gonna overlap the files now

		int d = 0,e = 0, f = 0; // Indicies
		for (d = 0; d < segment; d++){
			for (e = 0; e < dimention; e++){
				for(f = 0; f < 3; f++){//Algorth is based on overlapping of colors: how much Input2 "affects' inpu a based on intensity of that component in input2
					double intA = Input2[d][e][f];
					double percentage = (intA/0x100);
					int newValue = (Input1[d][e][f] * percentage);
					SmallOut[d][e][f] = newValue+1;
				}
			}
		}

		if (printing >= 2){
			sleep(2*my_rank); //easier to read print output this way
			printf("Rank %d of %d on %s Multipled and got the following:\n", my_rank, size, nodename);
			int a = 0, b = 0;// Indicies
			for (a = 0; a < segment; a++){
				for (b = 0; b < dimention; b++){
					printf("%.2x ,", SmallOut[a][b][0]);
					printf("%.2x ,", SmallOut[a][b][1]);
					printf("%.2x \t", SmallOut[a][b][2]);
				}
				printf("\n");
			}
		}

		//Sending out results of multiplication
		MPI_Send(&SmallOut[0][0][0], segment*dimention*3, MPI_INT, 0, 1, MPI_COMM_WORLD);
		printf("Rank %d of %d on %s Sent Results back to Rank 0\n", my_rank, size, nodename);
		//Finished Sending

	}//end worker only segment;  begins master only segment

	if(my_rank == 0){//Master work only
		int Result[dimention][dimention][3];
		//Going to recieve info from other ranks now
		int k = 1;
		for (k = 1; k <= workerThreads; k++){
			int g = k-1;
			int f = g*segment;
			MPI_Recv(&Result[f][0][0], segment*dimention*3, MPI_INT, k, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		//Checking if there were leftover rows: if yes, this rank will do them
		int leftovers = dimention % workerThreads;
		if (leftovers != 0){
			int c = (dimention - leftovers),d = 0; // Indicies
			for (c = (dimention - leftovers); c < dimention; c++){
				for (d = 0; d < dimention; d++){
//For now, simply turns the pixes white instead of being garbage
					int a, b, c;
					a = 0xff;
					b = 0xff;
					c = 0xff;
					Result[c][d][0] = a;
					Result[c][d][1] = b;
					Result[c][d][2] = c;
				}
			}
		printf("%d rows were left behind, but Rank 0 clenaed them up so it's okay :v \n", leftovers);
		}
		//Finished Checking now

		//Finished recieving info
		//sleep(2*my_rank); //easier to read print output this way
		if (printing >= 1){
			printf("Here is Master Result: \n");
			int a = 0, b = 0;// Indicies
			for (a = 0; a < dimention; a++){
				for (b = 0; b < dimention; b++){
					printf("%.2x ,", Result[a][b][0]);
					printf("%.2x ,", Result[a][b][1]);
					printf("%.2x \t", Result[a][b][2]);
				}
			printf("\n");
			}
		}


		//The below is adapted from http://rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C
		const int dimx = dimention, dimy = dimention;
  
		FILE *fp = fopen("first.ppm", "wb"); /* b - binary mode */
		(void) fprintf(fp, "P6\n%d %d\n255\n", dimx, dimy);
		for (j = 0; j < dimy; ++j){
			for (i = 0; i < dimx; ++i){
				static unsigned char color[3];
				color[0] = Result[i][j][0];  /* red */
				color[1] = Result[i][j][1];  /* green */
				color[2] = Result[i][j][2];  /* blue */
				(void) fwrite(color, 1, 3, fp);
			}
		}
		(void) fclose(fp);
	}//end of master only commands

	MPI_Finalize();  /* EXIT MPI */
}
