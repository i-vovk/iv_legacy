#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define ISIZE  3000
#define JSIZE  3000


int main(int argc, char **argv)
{
  int parallel;
  if(argc != 2){
    printf("Please speciy serial[0] or parallel[1]\n");
    return -1;
  }else
    parallel = atoi(argv[1]);

  struct timeval s_tv, e_tv;
  struct timezone tz;
  gettimeofday(&s_tv, &tz);

  int i, j;
  FILE *ff;
  double** a = (double**)malloc(ISIZE * sizeof(double*));
  for (j = 0; j < ISIZE; j++)
     a[j] = (double*) malloc(JSIZE * sizeof(double));

  #pragma omp parallel for private(j) if(parallel)
   for (i=0; i<ISIZE; i++){
    #pragma omp parallel for if(parallel)
    for (j=0; j<JSIZE; j++){
      a[i][j] = 10*i +j;
    }
   }
   
  for (i=1; i<ISIZE; i++){
    #pragma omp parallel for if(parallel)
    for (j = 0; j < JSIZE-1; j++){
      a[i][j] = sin(0.00001*a[i-1][j+1]); 
    }
  }
   
  ff = fopen(parallel ? "outs2_p.txt" : "outs2_s.txt","w");

  for(i=0; i < ISIZE; i++){
    for (j=0; j < JSIZE; j++){
      fprintf(ff,"%f ",a[i][j]);
    }
    fprintf(ff,"\n");
  }     

  fclose(ff);

  gettimeofday(&e_tv, &tz);
  double diff_sec = (double)(e_tv.tv_sec - s_tv.tv_sec);
  double diff_usec = (double)(e_tv.tv_usec - s_tv.tv_usec);
  printf("Elapsed time = %.3fsec\n", diff_sec + diff_usec/1000000 ) ; 
}