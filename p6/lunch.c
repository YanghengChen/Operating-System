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
#define THINKING 0
#define HUNGRY   1
#define EATING   2
#define DEAD     3

// init all the variables 
int i, shmid, *shmem;
int loopCount = 5;
int semid, mutex;

// we need time to count up to 60 second
time_t start_time;

void philosopher(int i);
void think();
void eat();
void take_chopsticks(int i);
void put_chopsticks(int i);
void test(int i);

int main(){
    int id = -1; 

    shmid = shmget(IPC_PRIVATE, sizeof(int)*N, 0770);

    if (shmid == -1){
        printf("Could not get shared memory.\n");
        return(0);
    }


/****   Attach to the shared memory  ******/

    shmem = (int *) shmat(shmid, NULL, SHM_RND);
   // Attach to the Semaphore
    semid = semget (IPC_PRIVATE, N, 0777);
    if (semid == -1) {
        printf("Failed to get semaphore.\n");
        return(-1);
    }
     mutex = semget (IPC_PRIVATE, 1, 0770);
     
     // Init the Sems
     semctl(mutex,0, SETVAL, 1);
     
     for(i =0; i< loopCount; i++){
         semctl(semid, i, SETVAL, 0);
     }

     int j;
     for( j = 0; j<N; j++){
         if(fork()){
	    break;
	 }
         id++; 	
     }

     start_time = time(NULL); 
     
     if(id ==-1){
     int t = 1;
         while (1) { 
            printf("%2d.  ", t);
            for(i=0; i<N; i++) {
		if (shmem[i] == THINKING) {
		    printf("%010s", "thinking");
		} else if (shmem[i] == HUNGRY) {
		    printf("%010s", "hungry");
		} else if (shmem[i] == EATING) {
		    printf("%010s", "eating");
		} else if (shmem[i] == DEAD) {
		    printf("%010s", "dead");
		}      	
            }
        
        printf("\n");
        // break the program when they all dead
	if (shmem[0] == DEAD && shmem[1] == DEAD && shmem[2] == DEAD &&
            shmem[3] == DEAD && shmem[4] == DEAD) {
            break;
        }
        sleep(1);  
        t++;
  	}                                                               
     }else{
         srand(time(NULL) + 100*getpid());
         philosopher(id);
     }
  
if (shmdt(shmem) == -1 ) printf("shmgm: ERROR in detaching.\n");

if (id == -1)         // ONLY need one process to do this
{
if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
    printf("ERROR removing shmem.\n");
if ((semctl(mutex, 0, IPC_RMID, 0)) == -1)
    printf("%s", "Parent: ERROR in removing sem\n");


}
 
    
     return 0;
}

void philosopher(int i) {
    int thinkTime = rand() % 7 + 4;
    int eatTime = rand() % 3 + 1; 
    while (difftime(time(NULL), start_time) < 60) { 
        sleep(thinkTime);
        if (difftime(time(NULL), start_time) >= 60) {
            break;
        }
        take_chopsticks(i);
        sleep(eatTime);
        put_chopsticks(i);
    }
    shmem[i] = DEAD;
}

void take_chopsticks(int i) {
    p(0, mutex);
    shmem[i] = HUNGRY;
    test(i);
    v(0, mutex);
    p(i, semid);
}

void put_chopsticks(int i) {
    p(0, mutex);
    shmem[i] = THINKING;
    test(LEFT);
    test(RIGHT);
    v(0, mutex);
}

void test(int i) {
    if (shmem[i] == HUNGRY && shmem[LEFT] != EATING && shmem[RIGHT] != EATING) {
        shmem[i] = EATING;
        v(i, semid);
    }
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
