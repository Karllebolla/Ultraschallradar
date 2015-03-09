#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "util/temperatur.h"
#include "util/motorMovement.h"
#include "util/timeMeasurement.h"
#include "util/global.h"
#include "util/gui.h"
#include "util/sensorMeasurement.h"

struct d arr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

int main(void) {
	pthread_barrier_t barrier;
	pthread_t thread1, thread2, guiThread;
	pthread_attr_t attrMotor, attrSensor, attrGui;
	int rc = 0;
	//FILE *fp; 

	if (tempSensor) {
		bmp085_Calibration();
	}

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

	/*fp =fopen("measure/nonRealtimeLog.txt","w");
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
	fclose(fp);*/

	execTime();
	blTime();

	return 0;
}
