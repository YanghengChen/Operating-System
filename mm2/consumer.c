//Yang Chen 
//CSC460
//Memory Management Project 1
//Consumer.c

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//struct for requests
struct request{
  char id;
  int pid;
  int size;
  int sec;
  int semid;
  bool inRAM;
};

//declare RAM
struct RAM {
  int RAMrow;
  int RAMcol;
  int bufSize;
};

//Free spot inside of RAM
struct FREE{
    int row;
    int col;
};

//Delare the Semaphores
int mutex, full, empty;

//Declaree the shared memory
int shmid;
struct RAM *ram;

// for the buffer
struct request *buf;
int tailShmem, *tail;


// we need a ending signal so we will know when to stop
int endShmem, *doneMark;

// for an array of requests using queue
struct request *queue; 

// Function that finds the first fit.
struct FREE find_free_spot(int rows, int cols, char arr[rows][cols], int size);
// check if the queue is empty
bool emptyQueue();

//Initialize the terms
#define BASE 0
#define SYNC 1
#define MANAGER 2
#define DISPLAY 3

#define DEM ram[0]
#define DONE doneMark[0]

#define TAIL tail[0]
#define SIZE (ram[0].bufSize - 1)

int main(int argc, char* argv[]){
   if(argc != 4 ){
       printf("Must be 4 arguments");
       exit(1);
   }  

   int row  = atoi(argv[1]);
   int col  = atoi(argv[2]);
   int bSize  = atoi(argv[3]);
   //condition check     
   if(row <1 || row >20){
     printf("Out of range...(1 to 20)");
     exit(1);
   }
   if(col <1 || col >50){
     printf("Out of range... (1 to 50)");
     exit(1);
   }
   if(bSize <1 || bSize >26){
     printf("Out of range... (1 to 26)");
     exit(1);
   }

// Initialize the share memory base on the command line arguments
    struct RAM iniRam = {row,col,bSize};   
 
   shmid  =  shmget(IPC_PRIVATE, sizeof(struct RAM), 0777);
    if (shmid == -1) {
        printf("Could not get shared memory.\n");
        exit(1);
    }
    
    ram = (struct RAM*) shmat(shmid, NULL, SHM_RND);
    DEM = iniRam;


//Initialize the semaphores 
    //for mutex
    mutex = semget (IPC_PRIVATE, 1, 0777);
    if (mutex == -1) {
        printf("Cannot get semaphore.\n");
        exit(1);
    }
    semctl(mutex, 0, SETVAL, 1);
    //for full
    full = semget (IPC_PRIVATE, 1, 0777);
    if (full == -1) {
        printf("Cannot get semaphore.\n");
        exit(1);
    }
    semctl(full, 0, SETVAL, 0); // full = 0
    //for empty
    empty = semget (IPC_PRIVATE, 1, 0777);
    if (empty == -1) {
        printf("Cannot get semaphore.\n");
        exit(1);
    }
    semctl(empty, 0, SETVAL, iniRam.bufSize); 

   // For the tail/rear of the buffer
    tailShmem  =  shmget(IPC_PRIVATE, sizeof(int), 0777);
    if (tailShmem == -1) {
        printf("Could not get shared memory.\n");
        exit(1);
    }
    tail = (int*) shmat(tailShmem, NULL, SHM_RND);
    TAIL = 0;
    
    // For the ending signal
    endShmem  =  shmget(IPC_PRIVATE, sizeof(int), 0777);
    if (endShmem == -1) {
        printf("Could not get shared memory.\n");
        exit(1);
    }
    doneMark = (int*) shmat(endShmem, NULL, SHM_RND);

    DONE = 0;
   
    //Create a file
    FILE *fp;
    fp = fopen("input.txt", "w");
    if (fp == NULL) {
        printf("Failed to creating file.\n");
        exit(1);
    }

    
    
    
    fprintf(fp, "%d\n", shmid); 
    fprintf(fp, "%d\n", mutex);     
    fprintf(fp, "%d\n", full);      
    fprintf(fp, "%d\n", empty);    
    fprintf(fp, "%d\n", tailShmem);   
    fprintf(fp, "%d\n", endShmem); 

    fclose(fp);

    int myId = BASE;
    //create a manager for RAM mamager
    int manager = semget (IPC_PRIVATE, 1, 0777);
    if (manager == -1) {
        printf("Failed to get semaphore.\n");
        exit(1);
    }
    semctl(manager, 0, SETVAL, 1); 
    //create a semaphores for consumer to display
    int displayer = semget (IPC_PRIVATE, 1, 0777);
    if (displayer == -1) {
        printf("Failed to get semaphore.\n");
        exit(1);
    }
    semctl(displayer, 0, SETVAL, 0); 
 
    //Fork 3 different process for 3 different task
    int i;
    for(i=0; i<3; i++) {
       if(fork()) {
           break;
        }
        myId++;
    }

   //Now its time for define the tasks for sync, manager, and displaer
   if(myId == SYNC){
            int tagger = 0;
            while(!DONE) {
                p(0, full);
                p(0, mutex);
                // Read from buffer
                struct request req = buf[TAIL];
                TAIL = (TAIL+1)%SIZE;
                tagger = (tagger + 1) % 25;
                req.id = 'A' + tagger - 1;
                if (!DONE){
                     int i;
                     // Find an empty slot and put the request in 
                     for(i=0; i<26; i++) {
                         if (queue[i].pid == -1) {
                             queue[i] = req; 
                             break;
                         } 
                     }
                }
                v(0, mutex);
                v(0, empty);
            } 
    }
    else if(myId == MANAGER){
            sleep(1); // give a second for synchronizer to start filling bounded buffer
            while(!DONE) {
                p(0, manager); // start before displayer
                if(DONE) 
		    break;
                // For procs in RAM decrement time and set inRAM to false if time becomes 0
                // Should do nothing in 1st iteration because inRam is initially false 
                int j;
                for (j=0; j<26; j++) {
                    if (queue[j].inRAM) { // if proc in RAM decrement time
                        queue[j].sec--;
                        if (queue[j].sec<=0) {
                            queue[j].inRAM = false;
                            v(0, queue[j].semid);
                        }
                    }
                }       
                sleep(1);
                // print all the procs in the array
                if (!emptyQueue() && !DONE){
                   int k;
                    printf("\n ID \t thePID \t Size \t Sec \n");
                    for(k=0; k<26; k++) {
                        if(queue[k].pid != -1 && queue[k].sec>0) // don't show if time requirement is met
                            printf("%c. \t %d \t %d \t %d \n", queue[k].id, queue[k].pid, queue[k].size, queue[k].sec);
                    }
                }                               
                v(0, displayer); // let displayer go now
            }
   } 
    else if(myId == DISPLAY){
            char arr[DEM.RAMrow][DEM.RAMcol];
            int i, j;
            for (i = 0; i < DEM.RAMrow; i++) {
                for (j = 0; j < DEM.RAMcol; j++) {
                    arr[i][j] = '.';
                }
            }

            while(!DONE) {
                p(0, displayer); // starts after manager
 		int k;
                for(k=0; k<26; k++) {         
                    if (queue[k].pid != -1 && !queue[k].inRAM && queue[k].sec<=0) { // if a proc met its time requirement
    			int a, b,c;
    			for(a=0; a<DEM.RAMrow; a++) {
        		    for(b=0; b<DEM.RAMcol; b++) {
            			if (arr[a][b] == queue[k].id) {
                		    arr[a][b] = '.';
            			}
        		    }
    			}


   			struct request  curr = {'\0', -1, -1, -1, -1, false};
   			for(c=0; c<26; c++) {
       			    if(queue[c].pid == queue[k].pid) {
           			queue[c] = curr;
           			break;
       			    }
   			}


                    } 
                    if(queue[k].pid != -1 && !queue[k].inRAM && queue[k].sec>0) {
                        struct FREE spot = find_free_spot(DEM.RAMrow, DEM.RAMcol, arr, queue[k].size);
                        if (spot.row != -1) { // if an empty slot was found for a proc not in ram
                            queue[k].inRAM = true;
			    //Fill up the Ram
    			    int d, e;
    			    int filled = 0;
    			    for(d=spot.row; filled<queue[k].size; d++) {
        			for(e=spot.col; filled<queue[k].size; e++) {
            			    arr[d][e] = queue[k].id;
            			    filled++;
        			}
    			    }
                        }
                    }
                } 
                if (!emptyQueue() && !DONE){
		    
    		    int f, g;
    		    for (g=0; g<DEM.RAMcol+2; g++) {
        		    printf("%c", '*');
    		    }
    		    printf("\n");

    		    for(f=0; f<DEM.RAMrow; f++) {
            		for(g=-1; g<DEM.RAMcol+1; g++) {
                		    if (g==-1 || g==DEM.RAMcol) {
                            printf("%c", '*');
                        }
                		    else { 
                            printf("%c", arr[f][g]);
                        }
            		}
                printf("\n");
    		    }

    		    for (g=0; g<DEM.RAMcol+2; g++) {
        		    printf("%c", '*');
    		    }
    		        printf("\n");
                }
		    if(DONE) 
            break;
            v(0, manager); // back to manager
            }
            v(0, manager);
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 //The BASE should clear up all the tasks
    sleep(1);
   if (myId == BASE && DONE) {
       int z;
       for(z=0; z<26; z++) {
           if (queue[z].pid != -1) {
               v(0, queue[z].semid);
           }
        }
        
        //clear the share memory
        if (shmdt(ram) == -1 ) 
            printf("shmgm: Cannot detach.\n");
        if (shmdt(buf) == -1 ) 
            printf("shmgm: Cannot detach.\n");
        if (shmdt(tail) == -1 ) 
            printf("shmgm: Cannot detach.\n");
        if (shmdt(doneMark) == -1 ) 
            printf("shmgm: Cannot detach.\n");
        if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
            printf("Cannot remove shmem.\n");

        //clear up the semaphor
        if ((semctl(manager, 0, IPC_RMID, 0)) == -1) {
            printf("Cannot in remove semaphores.\n");
        }
        if ((semctl(displayer, 0, IPC_RMID, 0)) == -1) {
            printf("Cannot in remove semaphores.\n");
        }
        if ((semctl(mutex, 0, IPC_RMID, 0)) == -1) {
            printf("Cannot in remove semaphores.\n");
        }
        if ((semctl(full, 0, IPC_RMID, 0)) == -1) {
            printf("Cannot in remove semaphores.\n");
        }
        
        if ((semctl(empty, 0, IPC_RMID, 0)) == -1) {
            printf("ERROR in remove semaphores.\n");
        }
        system("rm input.txt");   
    }

    return 0; 
}
//find the free spot in RAM for a given size mostly from stack overflow
struct FREE find_free_spot(int row, int col, char arr[row][col], int size) {
    int i, j;
    //initialze the start of the block
    int x = -1, y = -1; 
    int free = 0;

    struct FREE slot = {x, y};
    for(i=0; i<row; i++) {
        for(j=0; j<col; j++) {
            if (arr[i][j] == '.') {
                if(x == -1) {
                    x = i; 
                    y = j;
                }   
                free++;
            } else {
                x = -1;
                y = -1;
                free = 0;
            }
            if (free == size) {
                slot.row = x;
                slot.col = y; 
                return slot;
            }
        }
    }
    return slot;
}

// check if the queue is empty
bool emptyQueue() {
   int i=0;
   for(i=0; i<26; i++) {
       if(queue[i].pid != -1 && queue[i].sec>0) {
           return false;
       }
   }
   return true;
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
