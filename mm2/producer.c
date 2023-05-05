//Yang Chen 
//CSC 460
//Memory Management Project 1
//Producer.c

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct request{
  char id;
  int pid;
  int sec;
  int size;
  int semid;
  bool inRAM;
};

struct RAM {
  int row;
  int col;
  int bufSize;
};

int shmid;
struct RAM *dem;
struct request *buf;
int headShm, *head;
int endShmem, *doneMark;

int mutex, full, empty,semid;

// For producer we need to have a head instead of tail to start the program 
#define HEAD head[0]
#define SIZE (dem->bufSize - 1)
#define END doneMark[0]

int main(int argc, char** argv){
    if(argc !=3 ){
	printf("Must have 3 arguments");
	exit(0);
    }

    int size = atoi(argv[1]);
    int sec = atoi(argv[2]);
    // input check for size and time.
    if (1>size || size>dem->row * dem->col) {
        printf("Out of Range for size ( 1 to  rows*cols)\n");
        exit(1);
    }
    if (1>sec || sec>30) {
        printf("Out of range for time ( 1-30)\n");
        exit(1);
    }
    // creating a file and scan everything into the file.
    FILE *fp;
    fp = fopen("input.txt", "r");
    if (fp == NULL) {
        printf("Error reading file.\n");
        exit(1);
    }

  
    if ((dem = (struct RAM*) shmat(shmid, NULL, SHM_RND)) == (void *)-1) {
        printf("Failed to attach shared memory.\n");
    }


    if ((head = (int*) shmat(headShm, NULL, SHM_RND)) == (void *)-1) {
        printf("Failed to attach shared memory.\n");
    }

  
    if ((doneMark = (int*) shmat(endShmem, NULL, SHM_RND)) == (void *)-1) {
        printf("Failed to attach shared memory.\n");
    }

    fscanf(fp,"%d\n", &shmid);
    fscanf(fp, "%d\n", &headShm);
    fscanf(fp, "%d \n", &endShmem);
    fscanf(fp, "%d\n", &mutex);
    fscanf(fp, "%d\n", &full);
    fscanf(fp, "%d\n", &empty);

    fclose(fp);

    semid = semget (IPC_PRIVATE, 1, 0777);
    if (semid == -1) {
        printf("Failed to get semaphore.\n");
        exit(1);
    }
    semctl(semid, 0, SETVAL, 0);

    struct request req = {'\0', (int)getpid(), size, sec, semid, false};

    if (!END) {
        p(0, empty);
        p(0, mutex);
        buf[HEAD] = req;
        HEAD = (HEAD+1) % SIZE;
        v(0, mutex);
        v(0, full);
    }

    p(0, semid);
    if (shmdt(dem) == -1 )
        printf("ERROR in detaching.\n");
    if (shmdt(buf) == -1 )
        printf("ERROR in detaching.\n");
    if (shmdt(head) == -1 )
        printf(" ERROR in detaching.\n");
    if ((semctl(semid, 0, IPC_RMID, 0)) == -1) {
       printf("Error in removing semaphore.\n");
    }
    return 0;

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
