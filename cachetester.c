/*******************************************************************************
*Cachetester.c
*Goes through an array and determines Levels of Cache. Marking from L1, L2, L3,
*then uses that data to find the block size of each level of cache, and the set
*associativity. 
*To get block size for each level, go through char array to the point of the
*cache watermarks, then time a simple stride until a time spike. Mark that
*section. From the section marked to the relevant cache level watermark is the
*block size.
*To get Set associativity, we must know the dimensions of cache,
*and access sections of the array within those dimensions in even spaced pieces.
*    -Turn off any optimizations that might be messing with us. This might not
*      work very well on the Core i series, anyway, so just go with it. 
*
* WARNING! Tested mostly on an 
* Intel Pentium 200 MMX.
* Edit: Actually, works on a Xeon now!
* MISSING: Finding block size. Timing was just too erratic. Loss of portability
* as soon as we use POSIX functions. To make matters worse, time resolution
* changes between Windows Versions. Ugh. Fixed.
*******************************************************************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>


int main(){ /*defining everything inside.*/
volatile static char cacheArray[32*1024*1024]; /*32MB*/

volatile double previousTime=0.1;
volatile double currentTime=0.2;
volatile double timeRatio=0;

volatile bool levelOneMark=0;   /* L1 cache */
volatile bool levelTwoMark=0;   /* L2 cache */
volatile bool levelThreeMark=0; /* L3 cache */
int firstrun=0; /*have we ran any arrays at all?*/
volatile long L1Mark=0;
volatile long L2Mark=0;
volatile long L3Mark=0;
int doNothing = 0;
volatile long i = 0;            /*for our loops*/
volatile long j = 0;
volatile long k = 0;
volatile long arraySize = 32*1024*1024;
int L1BlockSize = system("getconf LEVEL1_DCACHE_LINESIZE"); /*Cheater!*/
int L2BlockSize = system("getconf LEVEL2_CACHE_LINESIZE");
volatile long fraction = 0;

clock_t start, end;

printf("Init OK.\n");
printf("Please set to RealTime Priority in Windows.\n");
printf("Note: if you have other things running, expect a wrong");
printf(" result. since Windows has such an awful timer, this has ");
printf("to be slow for a reason.\n");
printf("If this program does not work for you, you can edit the\n");
printf("Source to adjust ratios. the formula is as follows: \n");
printf("100 / percent = ratio\n");
printf("So, 20 percent would be: 100 / 20 = 5\n");
/*Note to self, remove a lot of this gibberish.*/

printf("Testing Level 1 Cache with %d KB array.\n", (int) arraySize / 1024);

while (levelOneMark==0)
{ /*while we're still in L1*/
  for (i=1024; i<arraySize; i*=2)
  {
   printf(".");
    start = clock(); /* start clock*/
    for (j=0; j<10000; j++) /*just a large number.*/
    {
     for (k=0; k<i; k++)
        {
             cacheArray[(k*16) & (i-1)] *=10; /*invoking the array!*/
             cacheArray[(k*16) & (i-1)] /=10; /*writes*/
       }
    }
    end = clock();
 timeRatio = (double) (currentTime) * 2 ; /*create expected value*/
     previousTime = (double) (currentTime);
    currentTime = (double) (end - start) / CLOCKS_PER_SEC;
/*
* the 20 is a 5% "fudge factor" to account for the clock.
* You can tweak this if you wish. the ratio Intel gives is around
* 20-25. It may differ from CPU to CPU.
*/

   if(currentTime > timeRatio + (previousTime / 20 ) && firstrun >1)
                   {
                   L1Mark=i;
                   levelOneMark=1;
                   printf("\nL1 Cache Marked.\n");
                   printf("L1 Cache Marked at %d KiloBytes,\n",(int) (i /1024));
                   break;
                   }
                   firstrun+=1;
  };
}
firstrun=0;      /*setting all this back*/
previousTime=0.0;
currentTime=0.0;
timeRatio=0;


printf("Testing Level 2 Cache with %d KB array.\n", (int)arraySize / 1024);
/*shouldn't have to loop here as much.*/
while (levelTwoMark==0 )
{ /*while we're still in L2*/

  for (i=L1Mark; i<arraySize; i*=2)
  { printf(".");
    start = clock(); /* start clock*/
    {
      for (j=0; j<500; j++) /*just a large number.*/
    {
    for (k=0; k<i; k++)
        {
             cacheArray[(k*16) & (i-1)] *=10; /*invoking the array!*/
             cacheArray[(k*16) & (i-1)] /=10;
             }
       }
    }
    end = clock();
     timeRatio = (double) (currentTime) * 2;
    previousTime = (double) (currentTime);
    currentTime = (double) (end - start) / CLOCKS_PER_SEC;
/*Same reason as above for the 20.*/
    if(currentTime > timeRatio + (previousTime / 20) && firstrun >1)
                   {
                   L2Mark=i;
                   levelTwoMark=1;
                   printf("\nL2 Cache Marked.\n");
                   printf("L2 Cache Marked at %d KiloBytes,\n", (int)(i /1024));
                   break;
                   }
                   firstrun+=1;
  };
}
firstrun=0;
/* L3 is a mystery. I never got consistent times on this.*/
printf("Level 3 might not work. Some processors do not have it, so\n");
printf("it's a possibility that you'll be hitting main memory. there is \n");
printf("no good way to check for this. Sorry! \n");

while (levelThreeMark==0 )
{
       for (i=L2Mark; i<arraySize; i*=2)
  {
    start = clock(); /* start clock*/
    for (j=0; j<50; j++) /*we shouldn't have to go through much here.*/
    {
    for (k=0; k<i; k++)
        {
             cacheArray[(k*16) & (i-1)] *=10; /*invoking the array!*/
             cacheArray[(k*16) & (i-1)] /=10;
       }
    }
    end = clock();

     timeRatio = (double) (currentTime * 2); 
     previousTime = (double) (currentTime);
    currentTime = (double) (end - start) / CLOCKS_PER_SEC;
/*added a little extra room here for error. Didn't seem to help in some cases.*/
    if((currentTime > timeRatio + (previousTime / 10) && firstrun > 1) 
							||( i == 16*1024*1024))
                   {
                   L3Mark=i;
                   levelThreeMark=1;
                   printf("L3 Cache Marked.\n");
                   printf("L3 Cache Marked at %d KiloBytes,\n", (int)(i /1024));
                   break;
                   }
                   firstrun+=1;
  };
}
printf("Either you had L3 cache or not.\n");
printf("Your results:\n");
printf("L1 Cache: %d KB \n",(int) L1Mark / 1024);
printf("L2 Cache: %d KB \n",(int) L2Mark / 1024);
printf("L3 Cache: %d KB\n", (int) L3Mark / 1024);
/*******************************************************************************
*Every watermark location in the array needs to be retained, because we need
*those marks to figure out the rest of the information.
*In the next step, importing the old blocksize code would work perfectly here.
*We only tested for L1 cache, but with knowledge of the watermarks, we can test
*specific ranges now. We would, however, need to step through just as before.
*but since we're only striding through until we hit a time spike, it won't take
*too long. go through the array n+1 sections x times, until we hit outside the
*block because the times will fail to match!
*******************************************************************************/ 
/*NEW CODE*/
/*testing L1 Block size */
long arrayblock = 16;
firstrun=0;
/*double block[32 * 1024];*/
printf("Testing block Size for L1 Cache, please wait.\n");
while (arrayblock <= 512){
	/*printf("%d",(int) i);*/
	start = clock();
for  (k=0; k<200000; k++){
	for (j = 0; j<L1Mark; j+=arrayblock){
	cacheArray[j] = cacheArray[j] + 2;
	}
	}
	end = clock();
	previousTime = currentTime;
	currentTime = (double) (end - start) / CLOCKS_PER_SEC;
/*block[arrayblock] = currentTime;*/
printf("%f\n", currentTime);
	if ((currentTime*2) < (previousTime) && 
					(arrayblock !=16)&& firstrun ==1){
	printf("Blocksize: %4d Bytes\n", (int) arrayblock/2);
	break;
	}
arrayblock *=2;
firstrun = 1;
}
/*testing L2 Block Size*/
arrayblock = 16;
firstrun=0;
printf("Testing block Size for L2 Cache, please wait.\n");
while (arrayblock <=512){
	start = clock();
	for (k=0; k<5000; k++){
	for (j = L1Mark; j < L2Mark; j+=arrayblock){
	cacheArray[j] = cacheArray[j] +2;
	}
	}
	end =  clock();
	previousTime = currentTime;
	currentTime = (double)  (end - start) / CLOCKS_PER_SEC;
printf("%f\n", currentTime);
	if ((currentTime*1.5) < (previousTime) + ((previousTime) / 25) && 
				(arrayblock !=16) && firstrun ==1){
	printf("Blocksize: %4d Bytes\n", (int) arrayblock/2);
break;
}
arrayblock *=2;
firstrun = 1;
}

/*testing L3 Block Size*/
arrayblock = 16;
firstrun=0;
printf("Testing block Size for L3 Cache, please wait.\n");
while (arrayblock <=512){
	start = clock();
	for (k=0; k<5000; k++){
	for (j = L2Mark; j < L3Mark; j+=arrayblock){
	cacheArray[j] = cacheArray[j] +2;
	}
	}
	end =  clock();
	previousTime = currentTime;
	currentTime = (double)  (end - start) / CLOCKS_PER_SEC;
printf("%f\n", currentTime);
	if ((currentTime*1.5) < (previousTime) + ((previousTime) / 25) && 
					(arrayblock !=16) && firstrun ==1){
	printf("Blocksize: %4d Bytes\n", (int) arrayblock/2);
break;
}
arrayblock *=2;
firstrun = 1;
}

/*******************************************************************************
* Here is the hardest part: Determining Set associativity of the Cache levels.
* The best way to do this is to use our old array, make boundaries using the 
* cache watermarks, and access sections of the array in fractional pieces.
* A visual representation using a 16-byte char array:
*
*    [ 0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15 ] 16/1 = nothing
*2-way:
*    [ 0  1  2  3  4  5  6 |7| 8  9  10  11  12  13  14  15 ] 16/2 = 8 spaces
*4-way:
*    [ 0  1  2 |3| 4  5  6 |7| 8  9  10 |11| 12  13  14 |15|] 16/4 = 4 spaces
*6-way:
*    [ 0 |1| 2 |3| 4 |5| 6 |7| 8 |9| 10 |11| 12 |13| 14 |15|] 16/6 = 2 spaces
*8-way:
*    [ 0 |1| 2 |3| 4 |5| 6 |7| 8 |9| 10 |11| 12 |13| 14 |15|] 16/8 = 2 spaces
*16-way:
*    [|0||1||2||3||4||5||6||7||8||9||10||11||12||13||14||15|] 16/16 = 1 spaces
*
*Since the section would be pretty big, we won't run into duplicate cases. The
*Idea is to access those sections (and anything after them, as long as we don't
*hit the other boundary) over and over until there is a significant time
*Difference. The way to measure that is to take the average of each step, and
*compare that to the previous result. if there is a different value, we've hit
*the limit of set-associativity, and the last value will be the result.
*This needs to be done for all levels of cache!
*******************************************************************************/
currentTime = 0.0;
previousTime = 0.0;
printf("Determining Set Associativity:\n");
int runs = 0;
for (i=0; i<L1Mark; i+=2){

     fraction = ((L1Mark) / (i+1));
printf("Testing for %d Way\n", (int) (L1Mark / fraction+1));
start = clock();
	for (runs=0; runs<32*1024*1024; runs++){

     for (j=0; j<L1Mark; j+=fraction){

	 doNothing ^= cacheArray[j];

	}

	}
	end = clock();
	previousTime = currentTime;
	currentTime= (double) (end - start) / CLOCKS_PER_SEC;
printf("%f\n",currentTime);
	if (currentTime < (previousTime *2) && firstrun ==1){
	printf("Set Associativity For L1 Cache is %d - Way.\n", 
					(int) (L1Mark/fraction)+1);
	break;
						       };
	firstrun = 1;
}
previousTime = 0.0;
currentTime = 0.0;
firstrun = 0;

for (i=L1Mark; i<L2Mark; i+=2){
fraction = ((L2Mark-L1Mark) / ((i-L1Mark)+1));
int setWay = (L2Mark / fraction + 1);
printf("Testing for %d Way\n", setWay);
start = clock();
for (runs=0; runs<32*1024*1024; runs++){

	for (k=L1Mark; k<L2Mark; k+=fraction){
		doNothing ^= cacheArray[k];
		}
	}
	end = clock();
	previousTime = currentTime;
	currentTime = (double) ( end - start ) / CLOCKS_PER_SEC;
	printf("%f\n", currentTime);
	if (currentTime < (previousTime* 2) && firstrun ==1){
	printf("Set Associativity for L2 Cache is %d - Way.\n",setWay);
	break;
								}
firstrun = 1;
}
previousTime = 0.0;
currentTime = 0.0;
firstrun = 0;

for (i=L2Mark; i<L3Mark; i+=2){
fraction = ((L3Mark-L2Mark) / ((i-L2Mark)+1));
int setWay = (L3Mark / fraction + 1);
printf("Testing for %d Way\n", setWay);
start = clock();
for (runs=0; runs<32*1024*1024; runs++){

	for (k=L2Mark; k<L3Mark; k+=fraction){
		doNothing ^= cacheArray[k];
		}
	}
	end = clock();
	previousTime = currentTime;
	currentTime = (double) ( end - start ) / CLOCKS_PER_SEC;
	printf("%f\n", currentTime);
	if (currentTime < (previousTime* 2) && firstrun ==1){
	printf("Set Associativity for L3 Cache is %d - Way.\n",setWay);
	break;
								}
firstrun = 1;
}
  return 0;
}

