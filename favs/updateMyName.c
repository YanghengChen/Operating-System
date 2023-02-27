#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[20];
    int favNum;
    char favFood[20];
} bStruct;

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Type in something \n");
        return 0;
    }

    FILE *fopen(), *fp;
    int shmid;
        //Read from BBID.txt provided in class
    if ((fp = fopen("/pub/csc460/bb/BBID.txt","r")) == NULL) {
        printf("Cannot open the file.\n");
        return 0;
    }
    fscanf(fp,"%d",&shmid);
      // Attach shared memory
    bStruct *shmem;
    if ((shmem = (bStruct *) shmat(shmid, NULL, SHM_RND)) == (void *)-1) {
        printf("Cannot attach shared memory.\n");
         return 0;
    }

    // Located index 1 on the list.
    shmem += 1;
    strncpy(shmem->name, argv[1], 19);
    return 0;
}
