#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {



    int N = atoi(argv[1]);
    int child = 0;
    int i;
    if(N!= 0)
    {
        child =1;
    }

    for (i=0; i<child; i++) {
	      if (fork() == 0) {
            child++;  // increase the number of child every time when fork()==0
            if (child > N){
		            break; 
            }
            i=-1;   // somehow I have to set i equals to -1 to make the loop starts over.
            
        }
    }


  printf("%d\t%d\n", getpid(), getppid());
	sleep(1);    
	return 0;
}

