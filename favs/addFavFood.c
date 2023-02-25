
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int id;
    char name[20];
    int favNum;
    char favFood[30];
}bStruct;

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Need to type in something");
        return 0;
    }

    FILE *fopen(), *fp;
    int shmid;
    if ((fp = fopen("/pub/csc460/bb/BBID.txt","r")) == NULL) {
        printf("Wrong file. \n");
       return 0;
    }
    fscanf(fp,"%d",&shmid);

    bStruct *shmem;
    if ((shmem = (bStruct *) shmat(shmid, NULL, SHM_RND)) == (void *)-1) {
        printf("Could not get a shared memory.\n");
        return 0;
    }

    shmem += 3;
    strncpy(shmem->favFood, argv[1], 29);
    return 0;
}
