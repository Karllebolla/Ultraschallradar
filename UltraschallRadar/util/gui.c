#include <allegro.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "timeMeasurement.h"
#include "global.h"
#include "timeMeasurement.h"

struct guiBlockTime guiBlockArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];
struct executionTimeGui guiExecutionArr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];
struct d arr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];

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

void *gui(void* val) {
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
				break;
			case 17:
				entfernung = entfernung;

				x = Xpos(360, entfernung);
				y = Ypos(360, entfernung);
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