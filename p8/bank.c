//Yang Chen
//CSC460
//Project 8



#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>



// init all the variables
int i,myID, amount;
int shmid, *shmem; // shm
int loopCount = 16;
int semid, mutex; // sems
#define Balance shmem[0]
typedef enum { false, true } bool;

int main(int argc, char* argv[]){
    myID = 0;
    bool parent = true;
    bool isFile = false;
    if(argc < 2){
 	if(isFile==true){
	    printf("File already exits for simulation.");
	    exit(1);
	}
	shmid = shmget(IPC_PRIVATE, sizeof(int), 0770);
	if (shmid == -1){
            printf("Could not get shared memory.\n");
            return(0);
        }
	shmem = (int *) shmat(shmid, NULL, SHM_RND);
       //Make the Semaphore
        mutex = semget (IPC_PRIVATE, 1, 0777);
        if (mutex == -1) {
            printf("Failed to get semaphore.\n");
            return(-1);
        }
        Balance  = 1000;  

        FILE *file;
        file = fopen("infile.txt", "w");
        if (file == NULL) {
            printf("Failed to create file.\n");
            exit(1);
        }
 	fprintf(file,"%d \t %d \n",shmid, mutex);   
	isFile = true;    
        printf("system is ready for simulation.\n");
    }
    if(argc==2){
	if(strcmp(argv[1],"balance")==0){
	    FILE *file;
	    file = fopen("infile.txt", "r"); 
	    if(file==NULL){
		printf("Failed to read the file");
		exit(1);
	    }
	    
	    fscanf(file,"%d \t %d \n", &shmid, &mutex);
            shmem = (int *) shmat(shmid, NULL, SHM_RND);	    
	    printf("%d \t \n",Balance);	       	    

	}
	else if(strcmp(argv[1],"die")==0){
	   FILE *file;
            file = fopen("infile.txt", "r");
            if(file==NULL){
                printf("Failed to read the file");
                exit(1);
            }

            fscanf(file,"%d \t %d \n", &shmid, &mutex);
            shmem = (int *) shmat(shmid, NULL, SHM_RND);
            printf("%d \t \n", Balance);

	   if ((shmctl(shmid, IPC_RMID, NULL)) == -1) {
      		 printf("Error removing shmem.\n");
      		 exit(1);
   	   }
   	   if ((semctl(mutex, 0, IPC_RMID, 0)) == -1) {
       		printf("Error in removing semaphore.\n");
   	   }
   	   system("rm infile.txt");
   	   printf("Shutting down...\n");
   	   exit(0);

	}
	else{
	    int input  = atoi(argv[1]);
	    if(input <0 || input> 100){
		printf("Must be within 1-100 range.\n");
		exit(1);
	    }
	    
	    FILE *file;
            file = fopen("infile.txt", "r");
            if(file==NULL){
                printf("Failed to read the file");
                exit(1);
            }

            fscanf(file,"%d \t %d \n", &shmid, &mutex);
            shmem = (int *) shmat(shmid, NULL, SHM_RND);
	    
	    //initialized the sem
	    semctl(mutex, 0, SETVAL, 1); 

            for (i=0; i<loopCount; i++) {
                if (fork()) {
                    break;
                }
                myID++;
		amount = pow(2,myID);
            }
            if(fork() ==0){
		parent =false;
	    }
	    for(i=0;i<input;i++){
		int old = Balance;
		p(0,mutex);
		if(parent){
		    Balance+=amount;
		    printf("%d +  %d =  %d \n",old,amount,  Balance);
		}else{
		    Balance -=amount;
		    printf("\t\t\t %d +  %d = %d \n",old,  amount, Balance);
		}
		v(0,mutex);		
	    }     
	}	
    }
    sleep(1);
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

