#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


//declare  semaphores
 int mutex, full, empty;

//declare shared mem
 int shmid, *id;

int main() {
    FILE *fp;
    fp = fopen("resources.txt", "r");
    if (fp == NULL) {
        printf("Failed to read the file.\n");
        exit(1);
    }

    int i;
    for(i=0; i<5; i++) {
        fscanf(fp, "%d\n", &shmid);
    }

    if ((id = (int*) shmat(shmid, NULL, SHM_RND)) == (void *)-1) {
        printf("Cannot attach the shared memory.\n");
    }

    fscanf(fp, "%d\n", &mutex);
    fscanf(fp, "%d\n", &full);
    fclose(fp);
   //Have to set the end flag to 1 to end the program
   id[0] = 1; 

   v(0, mutex);
   v(0, full);

   if (shmdt(id) == -1 )
        printf(" Failed to detach.\n");

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
