
/*
Ojani Eguia 
287 Term Project
Image Generator / Procressor
OpenMP Version
*/
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

void main(){
	int printing = 0;
	int dimention = 256;
	int nthreads = 5;
	int i = 0,j = 0; // Indicies
	srand(time(NULL));   // should only be called once
	int generation = 4;
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

	int Input1[dimention][dimention][3];
	int Input2[dimention][dimention][3];
	int Result[dimention][dimention][3];


	for (i = 0; i < dimention; i++){//FIRST INPUT ARRAY
		for (j = 0; j < dimention; j++){
			int a, b, c;
			if((generation % 2) == 0){//cool array
				a = i % 256;	/* red */
				b = j % 256;	/* green */
				c = (i * j) % 256;	/* blue */
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

	for (i = 0; i < dimention; i++){//SECOND INPUT ARRAY
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

	if (printing >= 1){
		printf("Here is Input 1 before Multiplication: \n");
		int a = 0, b = 0;// Indicies
		for (a = 0; a < dimention; a++){
			for (b = 0; b < dimention; b++){
				printf("%.2x ,", Input1[a][b][0]);
				printf("%.2x ,", Input1[a][b][1]);
				printf("%.2x \t", Input1[a][b][2]);
			}
			printf("\n");
		}
	}

	if (printing >= 1){
		printf("Here is Input 2 before Multiplication: \n");
		int a = 0, b = 0;// Indicies
		for (a = 0; a < dimention; a++){
			for (b = 0; b < dimention; b++){
				printf("%.2x ,", Input2[a][b][0]);
				printf("%.2x ,", Input2[a][b][1]);
				printf("%.2x \t", Input2[a][b][2]);
			}
			printf("\n");
		}
	}

//gonna overlap the files now

	#pragma omp parallel num_threads(nthreads)
	{
	printf("this is thread %d \n", omp_get_thread_num());


	int d = 0,e = 0, f = 0; // Indicies
	#pragma omp for
	for (d = 0; d < dimention; d++){
		for (e = 0; e < dimention; e++){
			for(f = 0; f < 3; f++){//Algorth is based on overlapping of colors: how much Input2 "affects' inpu a based on intensity of that component in input2
				double intA = Input2[d][e][f];
				double percentage = (intA/0x100);
				int newValue = (Input1[d][e][f] * percentage);
				Result[d][e][f] = newValue+1;
			}
		}
	}
	}

	//Checking if there were leftover rows: if yes, this rank will do them
	int leftovers = dimention % nthreads;
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
	printf("%d rows were left behind, but Thread 0 clenaed them up so it's okay :v \n", leftovers);
	}


	if (printing >= 1){
		printf("Here is Result after Multiplication: \n");
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
}
