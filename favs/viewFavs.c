#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char name[20];
    int favNum;
    char favFood[30];
} bStruct;

int main() {
    FILE *fopen(), *fp;
    int shmid;
    //Read from BBID.txt provided in class
    if ((fp = fopen("/pub/csc460/bb/BBID.txt","r")) == NULL) {
        printf("Cannot open current file.\n");
         return 0;
    }
    fscanf(fp,"%d",&shmid);

    // Attach shared memory
    bStruct *shmem;
    if ((shmem = (bStruct *) shmat(shmid, NULL, SHM_RND)) == (void *)-1) {
        printf("Cannot attatch shared memory.\n");
         return 0;
    }

    // Print all the data from shared memory
    printf("\n\t\t\tSHARED MEMORY:\n\n");
    // There are 12 of these structs
    int i;
    for (i=0; i<12; i++) {
        printf("%2d: %20s| %8d| %30s|\n", shmem->id, shmem->name, shmem->favNum, shmem->favFood);
        shmem++;
    }
    printf("\n");
    return 0;
}

