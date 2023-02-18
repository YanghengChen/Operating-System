#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define TURN shmem[0]

main(int argc, char *argv[])
{
    int i,shmid;
    char *shmem;
    int N;                  // Holds the number of procs to generate
    char myID = 'A';           // used to identify procs in sync
    int LoopCount = 5;     // Times each proc will "speak"
    /*****  Get the command line argument for how many procs. Default of 5 ****/
    if (argc < 2)
    {
                printf("Type in something more than a.out \n");
                return 0;
    }
    else
    {
      N = atoi(argv[1]);      //  Not error testing here :(
    }


    if(N<1 || N>26)
    {
        printf("integer n is out of range\n ");
        return 0;
    }

    /*****  Make a note of "who" is the first Process *****/

    int firstID = getpid();


    /*****  Get Shared Memory and ID *****/

    shmid  =  shmget(IPC_PRIVATE, sizeof(char), 0777);
    if (shmid == -1)
    {
        printf("Could not get shared memory.\n");
        return(0);
    }


    /****   Attach to the shared memory  ******/

    shmem = (char *) shmat(shmid, NULL, SHM_RND);


    /****  Initialize teh shared memory ****/

    TURN = 'A';
    char otherID = 'A' + (N-1);

    /*************  Spawn all the Processes *********/

    for (i = 1; i < N; i++) // total of n process
    {
      if (fork() > 0)
      {
        break; // send parent on to Body
      }
      myID++; // increase the id
    }


    /*************  BODY  OF  PROGRAM     ***********/

       for (i = 0; i < LoopCount; i++)
       {
            while(TURN != myID);  /** busy wait for Child **/
            printf("%c : %d \n",myID,getpid());
            if(TURN == otherID)
                TURN = 'A';
            else
                TURN++;
       }


    /****   Detach and clean-up the shared memory  ******/

    if (shmdt(shmem) == -1 ) printf("shmgm: ERROR in detaching.\n");


    if (firstID == getpid())         // ONLY need one process to do this
        if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
                printf("ERROR removing shmem.\n");
    sleep(1);
    return(0);
}



