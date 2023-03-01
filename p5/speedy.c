#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

#define FIRST 0 // define the first process


int main(int argc, char **argv) {


    if (argc == 1) {
        printf("We need integer arguments between 1 to 26.\n");
        return 1;
    }

    int n = atoi(argv[1]);
    if (n<1 || n>26) {
        printf("Argument needs to be in between 1 to 26.\n");
        return 1;
    }



    int  semid, myid = FIRST;

    //Ask OS for semaphores.
    semid = semget (IPC_PRIVATE, n, 0777);
    //Chck for the request.
    if (semid == -1) {
         printf("Cannot not get a semaphore.\n");
         return 1;
    }
   // set the fist value to 1 
    semctl(semid, FIRST, SETVAL, 1); 
   // and set the rest to be 0 waiting to be spawn
    int i;
    for (i=1; i<n; i++) {
        semctl(semid, i, SETVAL, 0);
    }


  // Spawn the processes
   for (i=1; i<n; i++) {
        if (fork()) {
            break;
        }
        myid++;
    }



 // Loop 5 times to print characters
    for (i=0; i<5; i++) {
         p(myid, semid);
         printf("%c:%d\n", 'A'+myid, getpid());
	//printf("v on %d\n", (myid+1)%n)
         v((myid+1)%n, semid);
    }

    sleep(1);
 
   if (myid == FIRST) {
        if ((semctl(semid, 0, IPC_RMID, 0)) == -1) {
            printf("Connot removing semaphores.\n");
        }
    }

    return 0;
}



p(int s,int sem_id) {
    struct sembuf sops;
    sops.sem_num = s;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    if((semop(sem_id, &sops, 1)) == -1)
        printf("%s", "Error removing P\n");
}

v(int s, int sem_id) {
    struct sembuf sops;
    sops.sem_num = s;
    sops.sem_op =1;
    sops.sem_flg = 0;
    if((semop(sem_id, &sops, 1)) == -1)
        printf("%s","'Errir removing V\n");
}

