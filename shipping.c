#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAXCHAR 1000
#define PACKAGES 100 // total number of packages
#define INSTRUCTIONS 2 // instructions for each package
#define TEAMS 4 // total number of teams
#define STATIONS 4 // total number of packing stations
#define WORKERS 10 // total number of workers per team
#define MOVE 100000 // time in micro seconds to move package

void *red(void *ptr); // red function prototype
void *green(void *ptr); // green function prototype
void *blue(void *ptr); // blue function prototype
void *yellow(void *ptr); // yellow function prototype
void wait_station(int stationNum, pthread_mutex_t mutex);
void signal_station(int stationNum);

int PPP[PACKAGES][INSTRUCTIONS]; // Pile of Packages data structure, values are 1-4 indicating instruction for package
int stations[STATIONS] = {0, 0, 0, 0}; // Index: 1 is scale, 2 is barcoder, 3 is x-ray, 4 is shaker, contains package numbes
int packageNum = 0; // global current package number
int packagesOnFirst = 0; // counts the number of packages on their first instruction
pthread_mutex_t the_mutex; // ensures mutual exclusion when modifying shared variables
pthread_mutex_t redMutex, greenMutex, blueMutex, yellowMutex; // mutual exclusion variables, 1 worker at a time
pthread_cond_t cond_st1, cond_st2, cond_st3, cond_st4; // condition variables for the scale (1), barcoder (2), x-ray (3), and shaker (4) stations
int redID = 0, greenID = 0, blueID = 0, yellowID = 0; // global red, green, blue, and yellow team identification

void wait_station(int stationNum, pthread_mutex_t mutex) // waits on the station given by station number
{
  switch(stationNum)
  {
  case 1:
    pthread_cond_wait(&cond_st1, &mutex); // wait on station 1
    break;
  case 2:
    pthread_cond_wait(&cond_st2, &mutex); // wait on station 2
    break;
  case 3:
    pthread_cond_wait(&cond_st3, &mutex); // wait on station 3
    break;
  case 4:
    pthread_cond_wait(&cond_st4, &mutex); // wait on station 4
    break;
  }
}

void signal_station(int stationNum) // signals the station given by station number
{
  switch(stationNum)
  {
  case 1:
    pthread_cond_signal(&cond_st1); // signal on station 1
    break;
  case 2:
    pthread_cond_signal(&cond_st2); // signal on station 2
    break;
  case 3:
    pthread_cond_signal(&cond_st3); // signal on station 3
    break;
  case 4:
    pthread_cond_signal(&cond_st4); // signal on station 4
    break;
  }
}

void *red(void *ptr) // red team worker thread
{
  pthread_mutex_lock(&redMutex); // ensure mutual exclusion, only one red worker at a time
  pthread_mutex_lock(&the_mutex); // ensure mutual exclusion for variables
  int thisID = redID; // thread identifier
  int thisPackNum = packageNum; // the package number this thread is working on
  int firstIn = PPP[thisPackNum][0]; // grab first instruction of package at the front of queue
  int secondIn = PPP[thisPackNum][1]; // grab second instruction of package at the front of queue
  redID++;
  packageNum++;
  pthread_mutex_unlock(&the_mutex);
  printf("[Red %d]: Package %d requires actions %d and %d!\n", thisID, thisPackNum, firstIn, secondIn);
  while(stations[firstIn] != 0 || packagesOnFirst >= 2) // while the first instruction's station is occupied
  // while there are two packages on their first instruction
  {
    wait_station(firstIn, redMutex);
  }
  packagesOnFirst++;
  stations[firstIn] = thisPackNum;
  printf("[Red %d]: Package %d was placed in station %d!\n", thisID, thisPackNum, firstIn);
  while(stations[secondIn] != 0) // while the second instruction's station is occupied
  {
    wait_station(secondIn, redMutex);
  }
  packagesOnFirst--;
  stations[secondIn] = thisPackNum;
  printf("[Red %d]: Moving package %d!\n", thisID, thisPackNum);
  usleep(MOVE);
  printf("[Red %d]: Package %d was conveyed to station %d!\n", thisID, thisPackNum, secondIn);
  printf("[Red %d]: Package %d complete!\n", thisID, thisPackNum);
  stations[firstIn] = 0;
  stations[secondIn] = 0;
  signal_station(firstIn);
  signal_station(secondIn);
  pthread_mutex_unlock(&redMutex); // unlock mutex
  pthread_exit(0);
}

void *green(void *ptr) // green team worker thread
{
  pthread_mutex_lock(&greenMutex); // ensure mutual exclusion, only one green worker at a time
  pthread_mutex_lock(&the_mutex); // ensure mutual exclusion for variables
  int thisID = greenID; // thread identifier
  int thisPackNum = packageNum; // the package number this thread is working on
  int firstIn = PPP[thisPackNum][0]; // grab first instruction of package at the front of queue
  int secondIn = PPP[thisPackNum][1]; // grab second instruction of package at the front of queue
  greenID++;
  packageNum++;
  pthread_mutex_unlock(&the_mutex);
  printf("[Green %d]: Package %d requires actions %d and %d!\n", thisID, thisPackNum, firstIn, secondIn);
  while(stations[firstIn] != 0 || packagesOnFirst >= 2) // while the first instruction's station is occupied
  {
    wait_station(firstIn, greenMutex);
  }
  packagesOnFirst++;
  stations[firstIn] = thisPackNum;
  printf("[Green %d]: Package %d was placed in station %d!\n", thisID, thisPackNum, firstIn);
  while(stations[secondIn] != 0) // while the second instruction's station is occupied
  {
    wait_station(secondIn, greenMutex);
  }
  packagesOnFirst--;
  stations[secondIn] = thisPackNum;
  printf("[Green %d]: Moving package %d!\n", thisID, thisPackNum);
  usleep(MOVE);
  printf("[Green %d]: Package %d was conveyed to station %d!\n", thisID, thisPackNum, secondIn);
  printf("[Green %d]: Package %d complete!\n", thisID, thisPackNum);
  stations[firstIn] = 0;
  stations[secondIn] = 0;
  signal_station(firstIn);
  signal_station(secondIn);
  pthread_mutex_unlock(&greenMutex); // unlock mutex
  pthread_exit(0);
}

void *blue(void *ptr) // blue team worker thread
{
  pthread_mutex_lock(&blueMutex); // ensure mutual exclusion, only one blue worker at a time
  pthread_mutex_lock(&the_mutex); // ensure mutual exclusion for variables
  int thisID = blueID; // thread identifier
  int thisPackNum = packageNum; // the package number this thread is working on
  int firstIn = PPP[thisPackNum][0]; // grab first instruction of package at the front of queue
  int secondIn = PPP[thisPackNum][1]; // grab second instruction of package at the front of queue
  blueID++;
  packageNum++;
  pthread_mutex_unlock(&the_mutex);
  printf("[Blue %d]: Package %d requires actions %d and %d!\n", thisID, thisPackNum, firstIn, secondIn);
  while(stations[firstIn] != 0 || packagesOnFirst >= 2) // while the first instruction's station is occupied
  {
    wait_station(firstIn, blueMutex);
  }
  packagesOnFirst++;
  stations[firstIn] = thisPackNum;
  printf("[Blue %d]: Package %d was placed in station %d!\n", thisID, thisPackNum, firstIn);
  while(stations[secondIn] != 0) // while the second instruction's station is occupied
  {
    wait_station(secondIn, blueMutex);
  }
  packagesOnFirst--;
  stations[secondIn] = thisPackNum;
  printf("[Blue %d]: Moving package %d!\n", thisID, thisPackNum);
  usleep(MOVE);
  printf("[Blue %d]: Package %d was conveyed to station %d!\n", thisID, thisPackNum, secondIn);
  printf("[Blue %d]: Package %d complete!\n", thisID, thisPackNum);
  stations[firstIn] = 0;
  stations[secondIn] = 0;
  signal_station(firstIn);
  signal_station(secondIn);
  pthread_mutex_unlock(&blueMutex); // unlock mutex
  pthread_exit(0);
}

void *yellow(void *ptr) // yellow team worker thread
{
  pthread_mutex_lock(&yellowMutex); // ensure mutual exclusion, only one yellow worker at a time
  pthread_mutex_lock(&the_mutex); // ensure mutual exclusion for variables
  int thisID = yellowID; // thread identifier
  int thisPackNum = packageNum; // the package number this thread is working on
  int firstIn = PPP[thisPackNum][0]; // grab first instruction of package at the front of queue
  int secondIn = PPP[thisPackNum][1]; // grab second instruction of package at the front of queue
  yellowID++;
  packageNum++;
  pthread_mutex_unlock(&the_mutex);
  printf("[Yellow %d]: Package %d requires actions %d and %d!\n", thisID, thisPackNum, firstIn, secondIn);
  while(stations[firstIn] != 0) // while the first instruction's station is occupied
  {
    wait_station(firstIn, yellowMutex);
  }
  packagesOnFirst++;
  stations[firstIn] = thisPackNum;
  printf("[Yellow %d]: Package %d was placed in station %d!\n", thisID, thisPackNum, firstIn);
  while(stations[secondIn] != 0 || packagesOnFirst >= 2) // while the second instruction's station is occupied
  {
    wait_station(secondIn, yellowMutex);
  }
  packagesOnFirst--;
  stations[secondIn] = thisPackNum;
  printf("[Yellow %d]: Moving package %d!\n", thisID, thisPackNum);
  usleep(MOVE);
  printf("[Yellow %d]: Package %d was conveyed to station %d!\n", thisID, thisPackNum, secondIn);
  printf("[Yellow %d]: Package %d complete!\n", thisID, thisPackNum);
  stations[firstIn] = 0;
  stations[secondIn] = 0;
  signal_station(firstIn);
  signal_station(secondIn);
  pthread_mutex_unlock(&yellowMutex); // unlock mutex
  pthread_exit(0);
}

int main(int argc, char **argv)
{
  printf("Welcome to FedOops shipping!\n");
  FILE* seed_file;
  char seed_str[MAXCHAR];
  seed_file = fopen("seed.txt", "r"); // read seed value
  if(seed_file == NULL)
  {
    printf("Can't open file for reading\n");
  }
  else
  {
    fscanf(seed_file, "%s", seed_str);
    fclose(seed_file);
  }
  printf("Read Seed Value: %s\n", seed_str); // seed value
  int seed = atoi(seed_str);
  printf("Read Seed Value (Integer): %d\n", seed); // integer seed value
  srand(seed);
  for(int k = 0; k < PACKAGES; k++) // get random instructions for each package (1-4)
  {
    PPP[k][0] = rand()%4+1; // 1 is scale, 2 is barcoder, 3 is x-ray, 4 is shaker
    PPP[k][1] = rand()%4+1; // 1 is scale, 2 is barcoder, 3 is x-ray, 4 is shaker
  }
  pthread_t r[WORKERS], g[WORKERS], b[WORKERS], y[WORKERS];
  for(int i = 0; i < TEAMS; i++) // create threads for each worker team
  {
    switch(i)
    {
      case 0:
        for(int j = 0; j < WORKERS; j++)
        {
          pthread_create(&r[i], 0, red, 0);
        }
        break;
      case 1:
        for(int j = 0; j < WORKERS; j++)
        {
          pthread_create(&g[i], 0, green, 0);
        }
        break;
      case 2:
        for(int j = 0; j < WORKERS; j++)
        {
          pthread_create(&b[i], 0, blue, 0);
        }
        break;
      case 3:
        for(int j = 0; j < WORKERS; j++)
        {
          pthread_create(&y[i], 0, yellow, 0);
        }
        break;
    }
  }
  for(int i = 0; i < TEAMS; i++) // join threads for each worker team
  {
    switch(i)
    {
      case 0:
        for(int j = 0; j < WORKERS; j++)
        {
          pthread_join(r[i], 0);
        }
        break;
      case 1:
        for(int j = 0; j < WORKERS; j++)
        {
          pthread_join(g[i], 0);
        }
        break;
      case 2:
        for(int j = 0; j < WORKERS; j++)
        {
          pthread_join(b[i], 0);
        }
        break;
      case 3:
        for(int j = 0; j < WORKERS; j++)
        {
          pthread_join(y[i], 0);
        }
        break;
    }
  }
}
