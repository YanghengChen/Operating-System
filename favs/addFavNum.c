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

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Enter an integer argument!\n");
        return 0;
    }

    FILE *fopen(), *fp;
    int shmid;
        //Read from BBID.txt provided in class
    if ((fp = fopen("/pub/csc460/bb/BBID.txt","r")) == NULL) {
        printf("Could not open this file.\n");
        return 0;
    }
    fscanf(fp,"%d",&shmid);
  // Attach shared memory
    bStruct *shmem;
    if ((shmem = (bStruct *) shmat(shmid, NULL, SHM_RND)) == (void *)-1) {
        printf("Can not create shared memory.\n");
        return 0;
    }
    // Located index 1 on the list.
    shmem += 1;
    shmem->favNum = atoi(argv[1]); // ignore type checks
    return 0;
}
