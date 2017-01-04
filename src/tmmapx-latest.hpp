#ifndef _TMMAPX_LATEST_
#define _TMMAPX_LATEST_

#include <sys/time.h>

#define UINT64 unsigned long long int
#define UINT32 unsigned int
#define UINT16 unsigned short
#define UINT8  unsigned char

double get_dtime(void){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 0.001 * 0.001);
}

FILE* myfopen(const char* fileName, const char* mode){
  FILE* fp;

  fp = fopen(fileName, mode);
  if(fp == NULL){
    printf("%s : No such files found.\n", fileName);
    exit(1);
  }
  return fp;
};

#endif
