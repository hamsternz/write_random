#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define BUFFER_SIZE (4*1024*1024)
#define FILES            16
#define BLOCKS_PER_FILE  16
unsigned char rand1[BUFFER_SIZE];
unsigned char rand2[BUFFER_SIZE];
unsigned char buffer[BUFFER_SIZE];

void merge(int offset) {
   int i;
   for(i = 0; i < BUFFER_SIZE; i++) {
     buffer[i] ^= rand1[i] ^ rand2[offset];
     if(offset == BUFFER_SIZE-1)
       offset = 0;
     else
       offset++;
   }
}

void write_random_data(FILE *f, int offset) {
  int i;
  for(i = 0; i < BLOCKS_PER_FILE; i++) {
    merge(offset);
    if(offset == BUFFER_SIZE-1)
       offset = 0;
     else
       offset++;
    if(fwrite(buffer,sizeof(buffer),1,f) != 1) {
      fprintf(stderr,"Write error\n");
      return;
    }
  }
}

void init_rand() {
  int i;
  srand(time(NULL));
  for(i = 0; i < BUFFER_SIZE; i++) {
    rand1[i] = rand();
    rand2[i] = rand();
  }
  for(i = 0; i < BUFFER_SIZE-4; i++) {
    if(rand1[i+0] == rand1[BUFFER_SIZE-4] && 
       rand1[i+1] == rand1[BUFFER_SIZE-3] && 
       rand1[i+2] == rand1[BUFFER_SIZE-2] && 
       rand1[i+3] == rand1[BUFFER_SIZE-1]) {
      fprintf(stderr,"rand1[] Repeat found at %i\n",i);
      exit(1);
    }
    if(rand2[i+0] == rand2[BUFFER_SIZE-4] && 
       rand2[i+1] == rand2[BUFFER_SIZE-3] && 
       rand2[i+2] == rand2[BUFFER_SIZE-2] && 
       rand2[i+3] == rand2[BUFFER_SIZE-1]) {
      fprintf(stderr,"rand2[] Repeat found at %i\n",i);
      exit(1);
    }
  }
}

int main(int argc,char *argv[]) {
  int i;
  init_rand();
  printf("This is only good for generating %i files of %iMB each\n",
          FILES, BUFFER_SIZE/1024/FILES*BUFFER_SIZE/1024);
  for(i = 0; i < FILES; i++) {
    char fname[10];
    FILE *f;

    sprintf(fname,"random%i",i);
    f = fopen(fname,"wb");
    if(f == NULL) {
       fprintf(stderr,"Unable to open %s\n",fname);
       continue;
    }
    if(fork() == 0) {
       fprintf(stderr,"Writing to %s\n",fname); 
       write_random_data(f, BUFFER_SIZE*i/FILES+(time(NULL)%9999));
       fprintf(stderr,"Writing to %s finished\n",fname); 
       exit(0);
    }
    fclose(f);
  }
  for(i = 0; i < FILES; i++) {
    int status;
    wait(&status);
  }
  return 0;
}
