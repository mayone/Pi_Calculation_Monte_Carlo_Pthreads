/******************************************************************************

 * * Filename: 		pi.c
 * * Description: 	use ptherad to acclerate Monte Carlo method calculationg pi
 * * 
 * * Version: 		1.0
 * * Created:		2014/10/26
 * * Revision:		none
 * * Compiler: 		gcc -lpthread pi.c
 * *
 * * Author: 		Wayne
 * * Organization:	CoDesign 

 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>		// rand()
#include <time.h>		// time()
#include <pthread.h>

#define NUM_THREADS 8


double rand_num_gen()
{
	return 2 * (random()/(double)RAND_MAX) - 1;	// RAND_MAX = 2147483647
}

void *toss_thread(void *tosses_of_thread_void);

int main(int argc, char const *argv[])
{
	int i;
	long long number_in_circle = 0;
	long long number_of_tosses = 100000000;
	if(argc == 2) {
		number_of_tosses = atoi(argv[1]);
	}
	long long tosses_of_thread = number_of_tosses / NUM_THREADS;
	long long tosses_of_remain = number_of_tosses % NUM_THREADS;
	srandom((unsigned)time(NULL));

	pthread_t threads[NUM_THREADS];

	pthread_create(&threads[i], NULL, toss_thread, (void*)(tosses_of_thread + tosses_of_remain));
	for(i = 1; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, toss_thread, (void*)tosses_of_thread);	
	}

	void *partial_number_in_circle;
	for(i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], &partial_number_in_circle);
		number_in_circle += *(long long*)partial_number_in_circle;
	}
	
	// output result
	double pi_estimate = 4*number_in_circle/(double)number_of_tosses;
	printf("pi = %.16lf\n", pi_estimate);

	return 0;
}

void *toss_thread(void *tosses_of_thread_void)
{
	double x, y;
	double distance_square;
	long long tosses_of_thread = (long long)tosses_of_thread_void;
	long long *number_in_circle = (long long*)malloc(sizeof(long long));
	long long toss;

	for(toss = 0; toss < tosses_of_thread; toss++) {
		x = rand_num_gen();		//printf("x = %lf\n", x);
		y = rand_num_gen();		//printf("y = %lf\n", y);
		distance_square = x*x + y*y;
		if(distance_square <= 1.0) {
			(*number_in_circle)++;
		}
	}

	pthread_exit(number_in_circle);
}
