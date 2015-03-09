#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "timeMeasurement.h"
#include "global.h"

struct guiBlockTime guiBlockArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct motorBlockTime forwardMotorBlockArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];
struct motorBlockTime backwardMotorBlockArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct sensorBlockTime sensorBlockArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct executionTimeGui guiExecutionArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct executionTimeMotor forwardMotorExecutionArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];
struct executionTimeMotor backwardMotorExecutionArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

struct executionTimeSensor sensorExecutionArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

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

	ret = NANOSECONDS_PER_SECOND * result.tv_sec + result.tv_nsec;
	ret = ret / NANOSECONDS_PER_SECOND;
	//ret = (NANOSECONDS_PER_SECOND * result.tv_sec + result.tv_nsec) / NANOSECONDS_PER_SECOND;
	//ret =  result.tv_sec + ( result.tv_nsec / NANOSECONDS_PER_SECOND) ;
	
	return ret;  
}

void execTime() {
	
	double gui = 0.0, guiTmp = 0.0;
	double fMotor = 0.0, fMotorTmp = 0.0;
	double bMotor = 0.0, bMotorTmp = 0.0;
	double sensor = 0.0, sensorTmp = 0.0;
	FILE *fp;
	int i = 0;
	fp =fopen("measure/nonRealtimeExecutionTime.txt","w");
	
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

void blTime() {
	
	double gui = 0.0, guiTmp = 0.0;
	double fMotor = 0.0, fMotorTmp = 0.0;
	double bMotor = 0.0, bMotorTmp = 0.0;
	double sensor = 0.0, sensorTmp = 0.0;
	FILE *fp;
	int i = 0;
	fp =fopen("measure/NonRealtimeBlockTime.txt","w");
	
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
