/******************************************************************************

 * * Filename: 		pi.c
 * * Description: 	use ptherad to acclerate Monte Carlo method calculationg pi
 * * 
 * * Version: 		1.3
 * * Created:		2014/10/27
 * * Revision:		2014/10/28
 * * Compiler: 		gcc -lpthread pi.c
 * *
 * * Author: 		Wayne
 * * Organization:	CoDesign 

 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>		// rand()
#include <time.h>		// time()
#include <unistd.h>		// sysconf()
#include <pthread.h>


typedef struct thread_data { 
	double *in_circle;
	pthread_mutex_t *in_circle_lock;
	int threadID;
	long long num_tosses;
}thread_data;

void *toss_thread(void *tosses_of_thread_void);

int main(int argc, char const *argv[])
{
	int i;
	double in_circle = 0;
	long long number_of_tosses;
	if(argc == 2) {
		number_of_tosses = atoi(argv[1]);
	}
	else {
		number_of_tosses = 100000000;
	}

	int NUM_THREADS = sysconf(_SC_NPROCESSORS_ONLN);

	long long tosses_of_thread = number_of_tosses / NUM_THREADS;
	long long tosses_of_remain = number_of_tosses % NUM_THREADS;

	pthread_t* threads;
	thread_data* threadD;
	pthread_mutex_t in_circle_lock;
	
	threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
	threadD = (thread_data*)malloc(sizeof(thread_data) * NUM_THREADS);

	pthread_mutex_init(&in_circle_lock, NULL);

	for(i = 0; i < NUM_THREADS; i++) {
		threadD[i].in_circle= &in_circle;
		threadD[i].in_circle_lock = &in_circle_lock;
		threadD[i].threadID = i;
		if(i == 0) threadD[i].num_tosses = tosses_of_thread + tosses_of_remain;
		else threadD[i].num_tosses = tosses_of_thread;
		pthread_create(&threads[i], NULL, toss_thread, (void*)&threadD[i]);	
	}

	for(i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	double pi_estimate = 4*in_circle/(double)number_of_tosses;
	printf("pi = %.16lf\n", pi_estimate);

	return 0;
}

// thread function
void *toss_thread(void *threadD)
{
	long long toss;
	double x, y;
	double distance_square;
	long long local_in_circle = 0;
	unsigned seed = (unsigned)time(NULL);
	thread_data* tData = (thread_data*)threadD;

	//printf("%lld\n", tData->num_tosses);
	
	for(toss = 0; toss < tData->num_tosses; toss++) {
		x = 2 * (rand_r(&seed)/(double)RAND_MAX) - 1;
		y = 2 * (rand_r(&seed)/(double)RAND_MAX) - 1;
		distance_square = x*x + y*y;
		if(distance_square <= 1.0) {
			local_in_circle++;
		}
	}

	pthread_mutex_lock(tData->in_circle_lock);
	*(tData->in_circle) += local_in_circle;
	pthread_mutex_unlock(tData->in_circle_lock);

	pthread_exit(NULL);
}
