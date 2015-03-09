#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <stdbool.h>
#include "sensorMeasurement.h"
#include "timeMeasurement.h"
#include "global.h"
#include "temperatur.h"

struct sensorBlockTime sensorBlockArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];
struct executionTimeSensor sensorExecutionArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct d arr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

void *sensor(void* val) {
	
	struct timespec start;
	struct timespec end;
	double result = 0.0;
	double distance = 0.0;
	double speed = 0.0, tmp;
	int save = 0;
	int countBefore= -1;
	int policy;
	struct sched_param param;
	int co = 0;
	if(pthread_getschedparam(pthread_self(), &policy, &param) != 0 ) {
		printf("error\n");
	}	

	printf(" Sensor    policy=%s, priority=%d\n",
	(policy == SCHED_FIFO)  ? "SCHED_FIFO" :
	(policy == SCHED_RR)    ? "SCHED_RR" :
	(policy == SCHED_OTHER) ? "SCHED_OTHER" :
	"???",
	param.sched_priority);

	pthread_barrier_wait(&barrier);

	while(abbruch == 0) {

		co = count;
		if(!(co == countBefore)) {
			if(executionTime == 1) {
				clock_gettime(CLOCK_REALTIME, &sensorExecutionArr[co].start);
			}
			countBefore = co;

			digitalWrite(7, 1);
			usleep(10);
			digitalWrite(7, 0);
			
			clock_gettime(CLOCK_REALTIME, &start);
			while(digitalRead(8) == 0) {
				clock_gettime(CLOCK_REALTIME, &start);
			}
			
			while(digitalRead(8) == 1) {
				clock_gettime(CLOCK_REALTIME, &end);
			}
			
			if (tempSensor == true )
			{
				temperature = bmp085_GetTemperature(bmp085_ReadUT());
				tmp = (double)temperature / 10;
				//temperature = 23.0;
			} else {
				tmp = 20.0;
			}
			printf("temperatur: %.2f\n", tmp);
			speed = 331.5 + (0.6 * tmp);
			speed = speed * 1000;
			result = diff_time(start, end);
			printf("Geschwindigkeit: %f\n", speed);
			printf("zeit: %.20f\n", result);	
			distance = ((result * speed) ) / 2;
			printf("entfernung: %.2f\n", distance);
			
			if( distance >= 30.0) {
					distance = 0.0;
			}
			
			printf("entfernung: %.2f\n", distance);
			save++;
			if(executionTime == 1) {
				clock_gettime(CLOCK_REALTIME, &sensorExecutionArr[co].end);
			}
			
			//kritischer Bereich
			if(blockTime == 1) {
				clock_gettime(CLOCK_REALTIME, &sensorBlockArr[co].start);
			}
			pthread_mutex_lock(&lock);
			arr[co].sensor = save - 1;
			arr[co].entfernung = distance;
			gemessen = 1;
			pthread_mutex_unlock(&lock);
			if(blockTime == 1) {
				clock_gettime(CLOCK_REALTIME, &sensorBlockArr[co].end);
			}	
		}
		usleep(60000);
	}
	printf("sensor %d\n", save);
	return NULL;
}
