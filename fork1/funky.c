#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {



    int N = atoi(argv[1]);
    int child = 0;
    int i;
    if(  N<0 || N >5 ){
        printf("Input out of range \n");
        return 1;
    }
    if(N!= 0)
    {
        child =1;
    }

    for (i=0; i<child; i++) {
	if (fork() == 0) {
            child++;
	    if (child > N){
		 break;
            }
            i= -1;
        }
    }


  printf("%d\t%d\n", getpid(), getppid());
	sleep(1);    
	return 0;
}

