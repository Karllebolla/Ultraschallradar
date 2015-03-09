#include "global.h"
#include <stdbool.h>

pthread_mutex_t lock;
pthread_barrier_t barrier;
int abbruch = 0;
volatile int gemessen = 0;
int executionTime = 1;
int blockTime = 0;
int volatile count = 0;
unsigned int temperature = 0;
bool tempSensor = true;
int arrayLength = (32* (TURNS + 1)) +  ((TURNS + 1) * 2);
struct d arr[(32* (TURNS + 1)) +  ((TURNS + 1) * 2)];
