#include <wiringPi.h>
#include <unistd.h> //F wird auch zum Zeichnen gebraucht
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <stdio.h>
#include <allegro.h>
#include <math.h>
#include <stdbool.h>
#include "temperatur.h"
#include "smbus.h"

#define NANOSECONDS_PER_SECOND 1000000000
#define TURNS 1 

pthread_mutex_t lock;
pthread_barrier_t barrier;
int abbruch = 0;
volatile int gemessen = 0;
int executionTime = 1;
int blockTime = 0;
int volatile count = 0;
double temperature;
bool tempSensor = true;

struct d {
	float entfernung;
	int position;

	int motor;
	int gui;
	int sensor;
	
	int readedDistance;

} entity;

int arrayLength = (32* (TURNS + 1)) +  ((TURNS + 1) * 2);
struct d arr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct guiBlockTime {
	struct timespec start;
	struct timespec end;

};
struct guiBlockTime guiBlockArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct motorBlockTime {
	struct timespec start;
	struct timespec end;
};
struct motorBlockTime forwardMotorBlockArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];
struct motorBlockTime backwardMotorBlockArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct sensorBlockTime {
	struct timespec start;
	struct timespec end;
};
struct sensorBlockTime sensorBlockArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct executionTimeGui {
	struct timespec start;
	struct timespec end;
};
struct executionTimeGui guiExecutionArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct executionTimeMotor {
	struct timespec start;
struct timespec end;
};
struct executionTimeMotor forwardMotorExecutionArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];
struct executionTimeMotor backwardMotorExecutionArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct executionTimeSensor {
	struct timespec start;
	struct timespec end;
};
struct executionTimeSensor sensorExecutionArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

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

double Xpos(double deg, double dis) {

	double strecke = dis * 400 / 30;
	double rad = deg * M_PI / 180;
	double radX = cos(rad);	
	double x = 425 + radX * strecke;

	return x;
}

double Ypos(double deg, double dis) {

	double strecke = dis * 400 / 30;
	double rad = deg * M_PI / 180;
	double radY = sin(rad);	
	double y = 400 + radY * strecke;

	return y;
}

double diff_time(struct timespec start, struct timespec end)
{
	double ret = 0;
	struct timespec result;
	
	if ((end.tv_sec < start.tv_sec) ||
			((end.tv_sec == start.tv_sec) &&
				(end.tv_nsec <= start.tv_nsec))) { 
		/* end before start */
		result.tv_sec = result.tv_nsec = 0;
	}

	result.tv_sec = end.tv_sec - start.tv_sec;
	result.tv_nsec = end.tv_nsec - start.tv_nsec;

	if(result.tv_nsec < 0 )
	{
		result.tv_sec--;
		result.tv_nsec = NANOSECONDS_PER_SECOND + result.tv_nsec;
	}

	//printf("sec  : %d\n", result.tv_sec);
	//printf("nsec : %d\n", result.tv_nsec);

	ret = NANOSECONDS_PER_SECOND * result.tv_sec + result.tv_nsec;
	ret = ret / NANOSECONDS_PER_SECOND;
	//ret = (NANOSECONDS_PER_SECOND * result.tv_sec + result.tv_nsec) / NANOSECONDS_PER_SECOND;
	//ret =  result.tv_sec + ( result.tv_nsec / NANOSECONDS_PER_SECOND) ;
	
	//printf("result1 %lu\n", NANOSECONDS_PER_SECOND * result.tv_sec + result.tv_nsec);
	
	return ret;  
}

static void execTime() {
	
	double gui = 0.0, guiTmp = 0.0;
	double fMotor = 0.0, fMotorTmp = 0.0;
	double bMotor = 0.0, bMotorTmp = 0.0;
	double sensor = 0.0, sensorTmp = 0.0;
	FILE *fp;
	int i = 0;
	fp =fopen("nonRealtimeExecutionTime.txt","w");
	
	if(fp == NULL) {
		printf("error opening file\n");
		exit(1);
	}
	
	for( i = 0; i < arrayLength; i++) {
		guiTmp = diff_time(guiExecutionArr[i].start, guiExecutionArr[i].end);
		fMotorTmp = diff_time(forwardMotorExecutionArr[i].start, forwardMotorExecutionArr[i].end);
		bMotorTmp = diff_time(backwardMotorExecutionArr[i].start, backwardMotorExecutionArr[i].end);
		sensorTmp = diff_time(sensorExecutionArr[i].start, sensorExecutionArr[i].end);
		/*
		fprintf(fp, "GUI  start  sec %lld  nsec %ld \n",  (long long)guiExecutionArr[i].start.tv_sec, guiExecutionArr[i].start.tv_nsec );
		fprintf(fp, "GUI  end    sec %lld  nsec %ld \n",  (long long)guiExecutionArr[i].start.tv_sec, guiExecutionArr[i].end.tv_nsec );
		
		fprintf(fp, "MO F  start  sec %lld  nsec %ld \n",  (long long)forwardMotorExecutionArr[i].start.tv_sec, forwardMotorExecutionArr[i].start.tv_nsec );
		fprintf(fp, "MO F  end    sec %lld  nsec %ld \n",  (long long)forwardMotorExecutionArr[i].start.tv_sec, forwardMotorExecutionArr[i].end.tv_nsec);
		
		fprintf(fp, "MO B  start  sec %lld  nsec %ld \n",  (long long)backwardMotorExecutionArr[i].start.tv_sec, backwardMotorExecutionArr[i].start.tv_nsec );
		fprintf(fp, "MO B  end    sec %lld  nsec %ld \n",  (long long)backwardMotorExecutionArr[i].start.tv_sec, backwardMotorExecutionArr[i].end.tv_nsec );
		
		fprintf(fp, "SEN  start  sec %lld  nsec %ld \n",  (long long)sensorExecutionArr[i].start.tv_sec, sensorExecutionArr[i].start.tv_nsec );
		fprintf(fp, "SEN  end    sec %lld  nsec %ld \n",  (long long)sensorExecutionArr[i].start.tv_sec, sensorExecutionArr[i].end.tv_nsec );
		*/
	fprintf(fp, "ExecutionTime Sensor:      %f\n", sensorTmp);
		if(gui < guiTmp) {
			gui = guiTmp;
		}
		if(fMotor < fMotorTmp) {
			fMotor = fMotorTmp;
		}
		if(bMotor < bMotorTmp) {
			bMotor = bMotorTmp;
		}
		if(sensor < sensorTmp) {
			sensor = sensorTmp;
		}
	}
		
	fprintf(fp, "ExecutionTime GUI:      %f\n", gui);
	fprintf(fp, "ExecutionTime F MOTOR:  %f\n", fMotor);
	fprintf(fp, "ExecutionTime B MOTOR:  %f\n", bMotor);
	fprintf(fp, "ExecutionTime SENSOR:   %f\n", sensor);
	
	fclose(fp);
}

static void blTime() {
	
	double gui = 0.0, guiTmp = 0.0;
	double fMotor = 0.0, fMotorTmp = 0.0;
	double bMotor = 0.0, bMotorTmp = 0.0;
	double sensor = 0.0, sensorTmp = 0.0;
	FILE *fp;
	int i = 0;
	fp =fopen("NonRealtimeBlockTime.txt","w");
	
	if(fp == NULL) {
		printf("error opening file\n");
		exit(1);
	}
	
	for( i = 0; i < arrayLength; i++) {
		guiTmp = diff_time(guiBlockArr[i].start, guiBlockArr[i].end);
		fMotorTmp = diff_time(forwardMotorBlockArr[i].start, forwardMotorBlockArr[i].end);
		bMotorTmp = diff_time(backwardMotorBlockArr[i].start, backwardMotorBlockArr[i].end);
		sensorTmp = diff_time(sensorBlockArr[i].start, sensorBlockArr[i].end);
		
		fprintf(fp, "GUI  start  sec %lld  nsec %ld \n",  (long long)guiBlockArr[i].start.tv_sec, guiBlockArr[i].start.tv_nsec );
		fprintf(fp, "GUI  end    sec %lld  nsec %ld \n",  (long long)guiBlockArr[i].start.tv_sec, guiBlockArr[i].end.tv_nsec );
		
		fprintf(fp, "MO F  start  sec %lld  nsec %ld \n",  (long long)forwardMotorBlockArr[i].start.tv_sec, forwardMotorBlockArr[i].start.tv_nsec );
		fprintf(fp, "MO F  end    sec %lld  nsec %ld \n",  (long long)forwardMotorBlockArr[i].start.tv_sec, forwardMotorBlockArr[i].end.tv_nsec);
		
		fprintf(fp, "MO B  start  sec %lld  nsec %ld \n",  (long long)backwardMotorBlockArr[i].start.tv_sec, backwardMotorBlockArr[i].start.tv_nsec );
		fprintf(fp, "MO B  end    sec %lld  nsec %ld \n",  (long long)backwardMotorBlockArr[i].start.tv_sec, backwardMotorBlockArr[i].end.tv_nsec );
		
		fprintf(fp, "SEN  start  sec %lld  nsec %ld \n",  (long long)sensorBlockArr[i].start.tv_sec, sensorBlockArr[i].start.tv_nsec );
		fprintf(fp, "SEN  end    sec %lld  nsec %ld \n",  (long long)sensorBlockArr[i].start.tv_sec, sensorBlockArr[i].end.tv_nsec );
		

		if(gui < guiTmp) {
			gui = guiTmp;
		}
		if(fMotor < fMotorTmp) {
			fMotor = fMotorTmp;
		}
		if(bMotor < bMotorTmp) {
			bMotor = bMotorTmp;
		}
		if(sensor < sensorTmp) {
			sensor = sensorTmp;
		}
	}	
	
	fprintf(fp, "BlockTime GUI:      %f\n", gui);
	fprintf(fp, "BlockTime F MOTOR:  %f\n", fMotor);
	fprintf(fp, "BlockTime B MOTOR:  %f\n", bMotor);
	fprintf(fp, "BlockTime SENSOR:   %f\n", sensor);
	
fclose(fp);
}

static void *sensor(void* val) {
	
	struct timespec start;
	struct timespec end;
	double result = 0.0;
	double distance = 0.0;
	double speed = 0.0;
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
				temperature = (bmp085_GetTemperature(bmp085_ReadUT()) / 10);
			} else {
				temperature = 34300;
			}
			speed = 331.5 + (0.6 * temperature);
			speed = speed * 100;
			result = diff_time(start, end);	
			distance = (result * speed) /2;
				if( distance >= 30.0) {
					distance = 0.0;
				}
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

static void *motor(void* val) {

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
			if(x == 0) {
				textprintf_ex(screen,font,25,425,makecol(0,0,255),-1,"0");
			} else if ( x == 1) {
				textprintf_ex(screen,font,50,425,makecol(0,0,255),-1,"1");
			} else if( x == 2) {
				textprintf_ex(screen,font,75,425,makecol(0,0,255),-1,"2");
			} else if( x== 3) {
				textprintf_ex(screen,font,100,425,makecol(0,0,255),-1,"3");
			} else if (x == 4) {
				textprintf_ex(screen,font,125,425,makecol(0,0,255),-1,"4");
			} else if ( x == 5) {
				textprintf_ex(screen,font,150,425,makecol(0,0,255),-1,"5");
			} else if( x == 6) {
				textprintf_ex(screen,font,175,425,makecol(0,0,255),-1,"6");
			} else if( x == 7) {
				textprintf_ex(screen,font,200,425,makecol(0,0,255),-1,"7");
			} else if (x == 8) {
				textprintf_ex(screen,font,225,425,makecol(0,0,255),-1,"8");
			} else if ( x== 9) {
				textprintf_ex(screen,font,250,425,makecol(0,0,255),-1,"9");
			} else if ( x == 10) {
				textprintf_ex(screen,font,275,425,makecol(0,0,255),-1,"a");
			} else if (x == 11) {
				textprintf_ex(screen,font,300,425,makecol(0,0,255),-1,"b");
			} else if ( x == 12) {
				textprintf_ex(screen,font,325,425,makecol(0,0,255),-1,"c");
			} else if (x == 13) {
				textprintf_ex(screen,font,350,425,makecol(0,0,255),-1,"d");
			} else if (x == 14) {
				textprintf_ex(screen,font,375,425,makecol(0,0,255),-1,"e");
			} else if (x == 15) {
				textprintf_ex(screen,font,400,425,makecol(0,0,255),-1,"f");
			} else if (x == 16) {
				textprintf_ex(screen,font,425,425,makecol(0,0,255),-1,"d");
			} else {
				printf("zu viel punkte\n");
			}
			
			if(x != 16) {
				
				for(i = 0; i < 16; i++) {
					
					stepForward();
				}				
			}
			save++;
			count++;
			
			if(executionTime == 1) {
				clock_gettime(CLOCK_REALTIME, &forwardMotorExecutionArr[co].end);
			}
		}
		
		for (x = 16; x >= 0; x--) {			
			
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
			if(x == 0) {
				textprintf_ex(screen,font,25,425,makecol(0,0,0),-1,"0");
			} else if ( x == 1) {
				textprintf_ex(screen,font,50,425,makecol(0,0,0),-1,"1");
			} else if( x == 2) {
				textprintf_ex(screen,font,75,425,makecol(0,0,0),-1,"2");
			} else if( x== 3) {
				textprintf_ex(screen,font,100,425,makecol(0,0,0),-1,"3");
			} else if (x == 4) {
				textprintf_ex(screen,font,125,425,makecol(0,0,0),-1,"4");
			} else if ( x == 5) {
				textprintf_ex(screen,font,150,425,makecol(0,0,0),-1,"5");
			} else if( x == 6) {
				textprintf_ex(screen,font,175,425,makecol(0,0,0),-1,"6");
			} else if( x == 7) {
				textprintf_ex(screen,font,200,425,makecol(0,0,0),-1,"7");
			} else if (x == 8) {
				textprintf_ex(screen,font,225,425,makecol(0,0,0),-1,"8");
			} else if ( x== 9) {
				textprintf_ex(screen,font,250,425,makecol(0,0,0),-1,"9");
			} else if ( x == 10) {
				textprintf_ex(screen,font,275,425,makecol(0,0,0),-1,"a");
			} else if (x == 11) {
				textprintf_ex(screen,font,300,425,makecol(0,0,0),-1,"b");
			} else if ( x == 12) {
				textprintf_ex(screen,font,325,425,makecol(0,0,0),-1,"c");
			} else if (x == 13) {
				textprintf_ex(screen,font,350,425,makecol(0,0,0),-1,"d");
			} else if (x == 14) {
				textprintf_ex(screen,font,375,425,makecol(0,0,0),-1,"e");
			} else if (x == 15) {
				textprintf_ex(screen,font,400,425,makecol(0,0,0),-1,"f");
			} else if (x == 16) {
				textprintf_ex(screen,font,425,425,makecol(0,0,0),-1,"d");
			} else {
				printf("zu viel punkte\n");
			}

			if(x != 0) {
				for(i = 0; i < 16; i++) {
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

static void *gui(void* val) {
	char poswert;
	float entfernung;
	double x = 0.0, y = 0.0;
	int save = 0, policy, countBefore = -1, co = 0, ge = 0;
	struct sched_param param;

	allegro_init();
	install_keyboard();
	set_color_depth(16);
	set_gfx_mode( GFX_AUTODETECT_WINDOWED, 850, 500, 0, 0);

	if(pthread_getschedparam(pthread_self(), &policy, &param) != 0) {
		printf("error\n");
	}
	
	printf(" GUI    policy=%s, priority=%d\n",
	(policy == SCHED_FIFO)  ? "SCHED_FIFO" :
	(policy == SCHED_RR)    ? "SCHED_RR" :
	(policy == SCHED_OTHER) ? "SCHED_OTHER" : "???",
	param.sched_priority);

	double rad1 = 225 * M_PI / 180;
	double rad2 = 315 * M_PI / 180;

	double  radX1 = cos(rad1);
	double 	radY1 = sin(rad1);
	double  radX2 = cos(rad2);
	double 	radY2 = sin(rad2);	

	double x1 = 425 + radX1 * 400;
	double y1 = 400 + radY1 * 400;
	double x2 = 425 + radX2 * 400;
	double y2 = 400 + radY2 * 400;

	hline(screen, 25, 400, 825, makecol(255,255,255));
	vline(screen, 425, 400, 0, makecol(255,255,255));
	line(screen, 425, 400, x1, y1, makecol(255,255,255));
	line(screen, 425, 400, x2, y2, makecol(255,255,255));
	circle(screen, 425, 400, 200, makecol(255,255,255));
	circle(screen, 425, 400, 10, makecol(255,255,255));
	circle(screen, 425, 400, 400, makecol(255,255,255));

	rectfill(screen, 25, 401, 825, 600, makecol(0,0,0));

	pthread_barrier_wait(&barrier);

	while(abbruch == 0) {

		usleep(60000);
		//kritischer Bereich

		co = count;
		ge = gemessen;

		if((!(co == countBefore)) && ge == 1) {
			
			//clock_gettime(CLOCK_REALTIME, &startUpdate);
			
			//kritischer Bereich
			if(blockTime == 1) {
				clock_gettime(CLOCK_REALTIME, &guiBlockArr[co].start);
			}	
			pthread_mutex_lock(&lock);
			
			poswert = arr[co].position;
			entfernung = arr[co].entfernung;
			arr[co].gui = save;
			gemessen = 0;
			
			if ((entfernung == 0) || (entfernung >= 30)) {
				entfernung = 30;
			}
			
			pthread_mutex_unlock(&lock);
			if(blockTime == 1) {
				clock_gettime(CLOCK_REALTIME, &guiBlockArr[co].end);
			}				
			
			if(executionTime == 1) {
				clock_gettime(CLOCK_REALTIME, &guiExecutionArr[co].start);
			}	
			
			countBefore = count;
			save++;

			hline(screen, 25, 400, 825, makecol(255,255,255));
			vline(screen, 425, 400, 0, makecol(255,255,255));
			line(screen, 425, 400, x1, y1, makecol(255,255,255));
			line(screen, 425, 400, x2, y2, makecol(255,255,255));
			circle(screen, 425, 400, 200, makecol(255,255,255));
			circle(screen, 425, 400, 10, makecol(255,255,255));
			circle(screen, 425, 400, 400, makecol(255,255,255));

			rectfill(screen, 25, 401, 825, 600, makecol(0,0,0));

			switch(poswert) {	
			case 0: 
				entfernung = entfernung; // entfernung *= 10;

				x = Xpos(180.0, entfernung);
				y = Ypos(180.0, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 1: 
				entfernung = entfernung;

				x = Xpos(191.25, entfernung);
				y = Ypos(191.25, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 2: 
				entfernung = entfernung;

				x = Xpos(202.5, entfernung);
				y = Ypos(202.5, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 3: 
				entfernung = entfernung;

				x = Xpos(213.75, entfernung);
				y = Ypos(213.75, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 4: 
				entfernung = entfernung;

				x = Xpos(225.0, entfernung);
				y = Ypos(225.0, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 5: 
				entfernung = entfernung;

				x = Xpos(236.25, entfernung);
				y = Ypos(236.25, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 6: 
				entfernung = entfernung;

				x = Xpos(247.5, entfernung);
				y = Ypos(247.5, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 7:
				entfernung = entfernung;

				x = Xpos(258.75, entfernung);
				y = Ypos(258.75, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 8:
				entfernung = entfernung;

				x = Xpos(270.0, entfernung);
				y = Ypos(270.0, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 9:
				entfernung = entfernung;

				x = Xpos(281.25, entfernung);
				y = Ypos(281.25, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 10:
				entfernung = entfernung;

				x = Xpos(292.5, entfernung);
				y = Ypos(292.5, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 11:
				entfernung = entfernung;

				x = Xpos(303.75, entfernung);
				y = Ypos(303.75, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 12:
				entfernung = entfernung;

				x = Xpos(307.08, entfernung);
				y = Ypos(307.08, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 13:
				entfernung = entfernung;

				x = Xpos(315.0, entfernung);
				y = Ypos(315.0, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 14:
				entfernung = entfernung;

				x = Xpos(326.25, entfernung);
				y = Ypos(326.25, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 15:
				entfernung = entfernung;

				x = Xpos(337.5, entfernung);
				y = Ypos(337.5, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
				break;
			case 16:
				entfernung = entfernung;

				x = Xpos(348.75, entfernung);
				y = Ypos(348.75, entfernung);
				circlefill(screen, x, y, 5, makecol(255,0,255));

				arr[co].readedDistance = entfernung;
			default: 
				break;
			}
			//clock_gettime(CLOCK_REALTIME, &endUpdate);
			//double resRadar = diff_time(startUpdate, endUpdate);
			if(executionTime == 1) {
				clock_gettime(CLOCK_REALTIME, &guiExecutionArr[co].end);
			}			
		}
	}
	sleep(3);
	printf("gui %d\n", save);
	return NULL;
}

int main(void) {
	
	pthread_t thread1, thread2, guiThread;
	pthread_attr_t attrMotor, attrSensor, attrGui;
	int rc = 0, h = 0;
	FILE *fp;

	bmp085_Calibration();

	if(pthread_attr_init(&attrMotor) !=0 ) {
		printf("error\n");
	}
	if( pthread_attr_init(&attrSensor) != 0) {
		printf("error\n");
	}
	if(pthread_attr_init(&attrGui) != 0) {
		printf("error\n");
	}
	
	//barier
	pthread_barrier_init(&barrier,NULL,3);	
	
	wiringPiSetupGpio();
	// Radar
	pinMode(7, OUTPUT);
	pinMode(8, INPUT);
	// Motor
	pinMode(18, OUTPUT);
	pinMode(23, OUTPUT);
	pinMode(24, OUTPUT);
	pinMode(25, OUTPUT);
	
	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		printf("\n mutex init failed\n");
		return 1;
	}
	
	//threads erzeugen
	rc = pthread_create(&guiThread, &attrGui, &gui, NULL);
	if (rc != 0 ) {
		printf("Konnte GUI Thread nicht starten\n");
		return 1;
	}
	
	rc = pthread_create( &thread1, &attrSensor, &sensor, NULL);
	if( rc != 0 ) {
		printf("Konnte Thread 1 - Radar - nicht erzeugen\n");
		return 1;
	}
	
	rc = pthread_create( &thread2, &attrMotor, &motor, NULL);
	if( rc != 0 ) {
		printf("Konnte Thread 2 - Motor - nicht erzeugen\n"); 
		return 1;
	}
		
	pthread_join( guiThread, NULL );
	pthread_join( thread1, NULL );
	pthread_join( thread2, NULL );
	pthread_mutex_destroy(&lock);

//	temperature = (bmp085_GetTemperature(bmp085_ReadUT()) / 10);
	printf("Temperatur: %0.2f\n", temperature);

	fp =fopen("nonRealtimeLog.txt","w");
	if(fp == NULL) {
		printf("error opening file\n");
		exit(1);
	}

	for( h = 0; h < arrayLength ; h++) {
		fprintf(fp, "\n\nTURNS %d\n", h);
		fprintf(fp, "entfernung        %f\n", arr[h].entfernung);
		fprintf(fp, "position          %d\n", arr[h].position);
		fprintf(fp, "motor             %d\n", arr[h].motor);
		fprintf(fp, "gui               %d\n", arr[h].gui);
		fprintf(fp, "sensor            %d\n\n", arr[h].sensor);
		fprintf(fp, "readedDistance:   %d\n", arr[h].readedDistance);
	}
	fclose(fp);

	execTime();
	blTime();
	
	return 0;
}
