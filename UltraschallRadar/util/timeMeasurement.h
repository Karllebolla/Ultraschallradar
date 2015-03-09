#ifndef TIMEMEASUREMENT_H
#define TIMEMEASUREMENT_H

#include <stdio.h>
#include <time.h>

struct guiBlockTime {
	struct timespec start;
	struct timespec end;

};
struct motorBlockTime {
	struct timespec start;
	struct timespec end;
};
struct sensorBlockTime {
	struct timespec start;
	struct timespec end;
};
struct executionTimeGui {
	struct timespec start;
	struct timespec end;
};
struct executionTimeMotor {
	struct timespec start;
struct timespec end;
};
struct executionTimeSensor {
	struct timespec start;
	struct timespec end;
};

double diff_time(struct timespec start, struct timespec end);
void execTime();
void blTime();

#endif


