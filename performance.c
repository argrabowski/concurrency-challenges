#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAXCHAR 1000
#define TYPES 3 // types of performers
#define DANCERS 15 // number of dancers
#define JUGGLERS 8 // number of jugglers
#define SOLOISTS 2 // number of soloists
#define POSITIONS 4 // number of stage positions

void *dancer(void *ptr); // function prototype for dancer thread
void *juggler(void *ptr); // function prototype for juggler thread
void *soloist(void *ptr); // function prototype for soloist thread

sem_t mutex; // binary semaphore that ensures mutual exclusion when waiting on and waking semaphores
sem_t positions; // semaphore that counts the number of stage positions available
sem_t danStage, jugStage, solStage; // binary semaphores that tracks whether or not a dancer, juggler, or soloist is on stage
int dancerID = 0, jugglerID = 0, soloistID = 0; // global dancer, juggler, and soloist identification
int danOnStage = 0, jugOnStage = 0, solOnStage = 0; // counts the number of dancers, jugglers, and soloists on the stage
int danStarve = 0, jugStarve = 0, solStarve = 0; // variables that track the number of consecutive stage occurances

void *dancer(void *ptr) // dancer thread
{
  int preWait = rand()%400000+100000; // get random wait time before thread tries to join stage between 0.1 and 0.4 seconds to check starvation
  usleep(preWait); // sleep
  sem_wait(&positions);
  if(jugOnStage >= 1 | solOnStage >= 1 | danStarve >= 4) // if there are jugglers or soloists on the stage,
  // if this type of performer has performed more then four times consecutively
  {
    sem_wait(&danStage);
  }
  int wait = rand()%4+1; // get random wait time between 1 and 4 seconds
  int* thisID = (int*)ptr;
  int pos;
  sem_getvalue(&positions, &pos);
  dancerID++; // increment global dancer identification
  danOnStage++;
  danStarve++;
  printf("[Dancer %d]: I have entered the stage at position %d and will perform for %d seconds!\n", *thisID, 4-pos, wait); // enter message
  sleep(wait); // sleep perform
  printf("[Dancer %d]: I have left the stage from position %d!\n", *thisID, 4-pos);
  danOnStage--;
  sem_wait(&mutex); // mutual exclusion while signalling
  if(danOnStage == 0 && pos == 0) // if there are no dancers on stage
  {
    sem_post(&jugStage); // signal juggler semaphore
    sem_post(&solStage); // signal soloist semaphore
    danStarve = 0; // reset starve since other types on stage
  }
  sem_post(&mutex);
  sem_post(&positions);
}

void *juggler(void *ptr) // juggler thread
{
  int preWait = rand()%400000+100000; // get random wait time before thread tries to join stage between 0.1 and 0.4 seconds to check starvation
  usleep(preWait); // sleep
  sem_wait(&positions);
  if(danOnStage >= 1 | solOnStage >= 1 | jugStarve >= 4) // if there are dancers or soloists on the stage,
  // if this type of performer has performed more then four times consecutively
  {
    sem_wait(&jugStage);
  }
  int wait = rand()%4+1; // get random wait time between 1 and 4 seconds
  int* thisID = (int*)ptr;
  int pos;
  sem_getvalue(&positions, &pos);
  jugglerID++; // increment global juggler identification
  jugOnStage++;
  jugStarve++;
  printf("[Juggler %d]: I have entered the stage at position %d and will perform for %d seconds!\n", *thisID, 4-pos, wait); // enter message
  sleep(wait); // sleep perform
  printf("[Juggler %d]: I have left the stage from position %d!\n", *thisID, 4-pos);
  jugOnStage--;
  sem_wait(&mutex); // mutual exclusion while signalling
  if(jugOnStage == 0 && pos == 0) // if there are no jugglers on stage
  {
    sem_post(&solStage); // signal soloist semaphore
    sem_post(&danStage); // signal dancer semaphore
    jugStarve = 0; // reset starve since other types on stage
  }
  sem_post(&mutex);
  sem_post(&positions);
}

void *soloist(void *ptr)
{
  int preWait = rand()%400000+100000; // get random wait time before thread tries to join stage between 0.1 and 0.4 seconds to check starvation
  usleep(preWait); // sleep
  sem_wait(&positions);
  if(danOnStage >= 1 | jugOnStage >= 1 | solStarve >= 4 | solOnStage == 1) // if there are dancers or jugglers on the stage,
  // if this type of performer has performed more then four times consecutively, if there is a soloist on the stage already
  {
    sem_wait(&solStage);
  }
  int wait = rand()%4+1; // get random wait time between 1 and 4 seconds
  int* thisID = (int*)ptr;
  int pos;
  sem_getvalue(&positions, &pos);
  soloistID++; // increment global soloist identification
  solOnStage++;
  solStarve++;
  printf("[Soloist %d]: I have entered the stage at position %d and will perform for %d seconds!\n", *thisID, 4-pos, wait); // enter message
  sleep(wait); // sleep perform
  printf("[Soloist %d]: I have left the stage from position %d!\n", *thisID, 4-pos);
  solOnStage--;
  sem_wait(&mutex); // mutual exclusion while signalling
  if(solOnStage == 0 && pos == 0) // if there are no soloists on stage
  {
    sem_post(&danStage); // signal dancer semaphore
    sem_post(&jugStage); // signal juggler semaphore
    solStarve = 0; // reset starve since other types on stage
  }
  sem_post(&mutex);
  sem_post(&positions);
}

int main(int argc, char **argv)
{
  printf("Welcome to the circus jazz performances!\n");
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
  sem_init(&mutex, 0, 1); // initialize dancers semaphore
  sem_init(&positions, 0, POSITIONS); // initialize positions semaphore
  sem_init(&danStage, 0, 0); // initialize dancers semaphore
  sem_init(&jugStage, 0, 0); // initialize jugglers semaphore
  sem_init(&solStage, 0, 0); // initialize soloists semaphore
  pthread_t dan[DANCERS], jug[JUGGLERS], sol[SOLOISTS];
  int danIDs[DANCERS], jugIDs[JUGGLERS], solIDs[SOLOISTS];
  for(int i = 0; i < TYPES; i++) // create threads for dancer, juggler, and soloist
  {
    switch(i)
    {
      case 0:
        for(int j = 0; j < DANCERS; j++)
        {
          danIDs[j] = j + 1;
          pthread_create(&dan[i], 0, dancer, &danIDs[j]);
        }
        break;
      case 1:
        for(int j = 0; j < JUGGLERS; j++)
        {
          jugIDs[j] = j + 1;
          pthread_create(&jug[i], 0, juggler, &jugIDs[j]);
        }
        break;
      case 2:
        for(int j = 0; j < SOLOISTS; j++)
        {
          solIDs[j] = j + 1;
          pthread_create(&sol[i], 0, soloist, &solIDs[j]);
        }
        break;
    }
  }
  for(int i = 0; i < TYPES; i++) // join threads for dancer, juggler, and soloist
  {
    switch(i)
    {
      case 0:
        for(int j = 0; j < DANCERS; j++)
        {
          pthread_join(dan[i], 0);
        }
        break;
      case 1:
        for(int j = 0; j < JUGGLERS; j++)
        {
          pthread_join(jug[i], 0);
        }
        break;
      case 2:
        for(int j = 0; j < SOLOISTS; j++)
        {
          pthread_join(sol[i], 0);
        }
        break;
    }
  }
  sem_destroy(&mutex); // destroy mutex semaphore
  sem_destroy(&positions); // destroy the positions semaphore
  sem_destroy(&danStage); // destroy the dancer semaphore
  sem_destroy(&jugStage); // destroy the juggler semaphore
  sem_destroy(&solStage); // destroy the soloist semaphore
}
