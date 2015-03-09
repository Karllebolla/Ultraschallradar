#ifndef GLOBAL_H
#define GLOBAL_H

#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

#define TURNS 1
#define NANOSECONDS_PER_SECOND 1000000000

extern pthread_mutex_t lock;
extern pthread_barrier_t barrier;
extern int abbruch;
extern volatile int gemessen;
extern int executionTime;
extern int blockTime;
extern int volatile count;
extern unsigned int temperature;
extern bool tempSensor ;
extern int arrayLength;
struct d {
	float entfernung;
	int position;
	int motor;
	int gui;
	int sensor;
	int readedDistance;
};

#endif
