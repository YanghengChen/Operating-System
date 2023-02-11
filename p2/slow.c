// Yang Chen
// Slow.c
// CSC 460

#include <stdio.h>

main(int argc, char *argv[])
{
  FILE *fopen(), *fp;
   int n;
   n = atoi(argv[1]);
  
  /*****  Open File to write a value  *****/
  
  if((fp = fopen("syncfile","w")) == NULL)
      {
          printf(":( could not open myLittleFile to write.\n");
          return(0);
      }
  
  
  /*****  Write into syncfile ID of whose turn.  *****/
  
  fprintf(fp,"%c",'A');
  fclose(fp);
  
  
  /*****  Create Child process and give unique myID *****/
  
  char  myID = 'A';  //start with char A
  char  otherID = 'A' + (n-1);
  
  int j;  // start J with 1 so I dont have to do n-1 
  for( j=1;j<=n;j++) // loop will run n times
  {
      if(fork() == 0)
      {
          myID++;
      }
      else 
      {
  	break;   
      }
  }
  
  
  /*****  LOOP 5 Times  *****/
  
  int  i = 0;
  char  found;
  
  for (i=0; i<5; i++)// change i to 5 so it only loop 5 times
  {
  
       /***** Repeatedly check file until myID is found *****/
  
       found = ' ';  // instead of -1, we set found to a empty char
       while (found != myID)
       {
           if((fp = fopen("syncfile","r")) == NULL)
               {
                   printf(":( could not open syncfile to read.\n");
                   return(0);
               }
  
               fscanf(fp,"%c",&found);
               fclose(fp);
       }
       
       // It must be my turn to do something.....
  
       printf("%c: %d\n",myID,getpid());
  
       // Update file to allow otherID to go 
  
       if((fp = fopen("syncfile","w")) == NULL)
           {
               printf(":( could not open myLittleFile to write.\n");
               return(0);
           }
  
          if (myID == otherID)
              fprintf(fp,"%c", 'A'); //when last id is reached
          else
              fprintf(fp, "%c", myID+1); //write next id
  
          fclose(fp);
  
  
  }

      return(0);

}
