#include <wiringPi.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "global.h"
#include "timeMeasurement.h"

struct executionTimeMotor forwardMotorExecutionArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];
struct executionTimeMotor backwardMotorExecutionArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct motorBlockTime forwardMotorBlockArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];
struct motorBlockTime backwardMotorBlockArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct d arr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

void step1() {
	digitalWrite(25, 1);
	usleep(1000);
	digitalWrite(25, 0);
}

void step2() {
	digitalWrite(24, 1);
	digitalWrite(25, 1);
	usleep(1000);
	digitalWrite(25, 0);
	digitalWrite(24, 0);
}

void step3() {
	digitalWrite(24, 1);
	usleep(1000);
	digitalWrite(24, 0);
}

void step4() {
	digitalWrite(23, 1);
	digitalWrite(24, 1);
	usleep(1000);
	digitalWrite(23, 0);
	digitalWrite(24, 0);
}

void step5() {
	digitalWrite(23, 1);
	usleep(1000);
	digitalWrite(23, 0);
}

void step6() {
	digitalWrite(18, 1);
	digitalWrite(23, 1);
	usleep(1000);
	digitalWrite(18, 0);
	digitalWrite(23, 0);
}

void step7() {
	digitalWrite(18, 1);
	usleep(1000);
	digitalWrite(18, 0);
}

void step8() {
	digitalWrite(18, 1);
	digitalWrite(25, 1);
	usleep(1000);
	digitalWrite(18, 0);
	digitalWrite(25, 0);
}

void stepForward() {
	step1();
	step2();
	step3();
	step4();
	step5();
	step6();
	step7();
	step8();
}

void stepBackward() {
	step8();
	step7();
	step6();
	step5();
	step4();
	step3();
	step2();
	step1();
}

void *motor(void* val) {

	int i = 0, x = 0, y = 0;
	int save = 0, co = 0, coTwo = 0;

	int policy;
	struct sched_param param;

	if(pthread_getschedparam(pthread_self(), &policy, &param) != 0) {
		printf("error\n");
	}
	
	printf(" Motor    policy=%s, priority=%d\n",
	(policy == SCHED_FIFO)  ? "SCHED_FIFO" :
	(policy == SCHED_RR)    ? "SCHED_RR" :
	(policy == SCHED_OTHER) ? "SCHED_OTHER" :
	"???",
	param.sched_priority);
	
	pthread_barrier_wait(&barrier);
	for(y = 0; y <= TURNS ; y++) {

		/* 512 steps = 360 grad
		* 16 steps = 11,25 grad 
		*/
		for (x = 0; x < 17; x++) {
			
			co = count;
			if(blockTime == 1) {
				clock_gettime(CLOCK_REALTIME, &forwardMotorBlockArr[co].start);
			}
			pthread_mutex_lock(&lock);	
			arr[count].position = x;
			arr[count].motor = save;
			pthread_mutex_unlock(&lock);
			if(blockTime == 1) {
				clock_gettime(CLOCK_REALTIME, &forwardMotorBlockArr[co].end);
			}

			usleep(200000);
	
			if(executionTime == 1) {
				clock_gettime(CLOCK_REALTIME, &forwardMotorExecutionArr[co].start);
			}
			
			if(x != 17) {
				
				for(i = 0; i < 17; i++) {
					
					stepForward();
				}				
			}
			save++;
			count++;
			
			if(executionTime == 1) {
				clock_gettime(CLOCK_REALTIME, &forwardMotorExecutionArr[co].end);
			}
		}
		
		for (x = 17; x >= 0; x--) {			
			
			if(blockTime == 1) {
				clock_gettime(CLOCK_REALTIME, &backwardMotorBlockArr[coTwo].start);
			}
			pthread_mutex_lock(&lock);	
			arr[count].motor = save;
			arr[count].position = x;
			pthread_mutex_unlock(&lock);
			if(blockTime == 1) {
				clock_gettime(CLOCK_REALTIME, &backwardMotorBlockArr[coTwo].end);
			}
			usleep(200000);

			
			if(executionTime == 1) {
				clock_gettime(CLOCK_REALTIME, &backwardMotorExecutionArr[coTwo].start);
			}

			if(x != 0) {
				for(i = 0; i < 17; i++) {
					stepBackward();
				}
			}
			count++;
			save++;

			if(executionTime == 1) {
				clock_gettime(CLOCK_REALTIME, &backwardMotorExecutionArr[coTwo].end);
			}
			coTwo++;
		}
		//save++;
	}
	count--;
	abbruch = 1;
	printf("motor %d\n", save);
	printf("count %d\n", count);
	return NULL;
}