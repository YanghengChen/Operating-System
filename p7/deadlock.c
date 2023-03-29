//Yang Chen
//CSC 460 
//3/19/2023
//p6

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// From the given sheet and the instructions
#define N        5          // number of philosophers
#define LEFT     (i+N-1)%N  // i's left neighbor
#define RIGHT    (i+1)%N    // i's right neighbor


// init all the variables 
int i, j,shmid, *shmem;
int loopCount = 5;
int semid, mutex;
time_t start_time;

void philosopher(int i);
void think();
void eat();
void take_chopsticks(int i);
void put_chopsticks(int i);

int main(){
    int id = 0; 
    start_time = time(NULL); 

   // Attach to the Semaphore
    semid = semget (IPC_PRIVATE, N, 0777); // getting chopsticks 
    if (semid == -1) {
        printf("Failed to get semaphore.\n");
        return(-1);
    }

     // Init the Sems
     for(i =0; i< loopCount; i++){
         semctl(semid, i, SETVAL, 1);
     }


     for( j = 0; j<N; j++){
         if(fork()){
	          break;
         }
         id++; 	
     }

     srand(start_time + 100*getpid());
     philosopher(id);

  
 
    
     return 0;
}


void philosopher(int i) {
    while (1) { // this is inifinite loop
        think(i);
        take_chopsticks(i);
        eat(i);
        put_chopsticks(i);           
    }
}

void think(int i) {
    printf("%d %10s\n", i, "THINKING"); // print thinking

}

void take_chopsticks(int i) {
    int k,j,x;
    for(k=0;k<i;k++) {printf("\t\t");}
    printf("%d %5s\n", i, "HUNGRY"); //print hungry
    p(LEFT, semid);  // pick left chopstick
   for (j = 0;j< 100000; j++) {   //busy block
 	    x = j*j;	
   }
    p(RIGHT, semid); // pick right chopstick  
}

void eat(int i) {
   int l;
   for(l=0;l<i;l++){printf("\t\t");}  // add extra tabs
   printf("%d %s\n", i, "EATING");// print eating
}



void put_chopsticks(int i) {
    int j,x;
    v(LEFT, semid);  // put left chopstick
   for (j = 0;j< 100000; j++) {   // busy block
            x = j*j;
   }

    v(RIGHT, semid); // put  right chopstick  
}

p(int s,int sem_id) {
    struct sembuf sops;
    sops.sem_num = s;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    if((semop(sem_id, &sops, 1)) == -1)
        printf("%s", "'P' error\n");
}

v(int s, int sem_id) {
    struct sembuf sops;
    sops.sem_num = s;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    if((semop(sem_id, &sops, 1)) == -1)
        printf("%s","'V' error\n");
}
