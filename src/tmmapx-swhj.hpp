#ifndef _TMMAPX_SWHJ_
#define _TMMAPX_SWHJ_

#include "tmmapx-latest.hpp"
#include <omp.h>

typedef struct _joinOpeItem{
  int nBoardIndex;
  UINT32 addrRd;
  UINT32 addrSd;
  UINT32 addrWd;
  int dsize;
  int wsize;
  int output_tuple;
  double comp_time;
} joinOpeItem;

void tmm_joinOpePrint(joinOpeItem*temp){
  fprintf(stderr,
          "[tamamo DEBUG]%d addr(%x %x %x) size(%d %d) %d %f\n",
          temp -> nBoardIndex,
          temp -> addrRd,
          temp -> addrSd,
          temp -> addrWd,
          temp -> dsize ,
          temp -> wsize ,
          temp -> output_tuple ,
          temp -> comp_time);
}

void tmm_joinOpeAsgmt(joinOpeItem*temp,
                         int nBoardIndex,  UINT32 addrRd, UINT32 addrSd, UINT32 addrWd,
                         int dsize, int wsize,
                         int output_tuple, double comp_time){
  temp -> nBoardIndex = nBoardIndex;
  temp -> addrRd = addrRd;
  temp -> addrSd = addrSd;
  temp -> addrWd = addrWd;
  temp -> dsize = dsize;
  temp -> wsize = wsize;
  temp -> output_tuple = output_tuple;
  temp -> comp_time = comp_time;
}

void tmm_joinOpeCopy(joinOpeItem*src, joinOpeItem*dst){
  dst -> nBoardIndex = src ->  nBoardIndex;
  dst -> addrRd = src ->  addrRd;
  dst -> addrSd = src ->  addrSd;
  dst -> addrWd = src ->  addrWd;
  dst -> dsize = src ->  dsize;
  dst -> wsize = src ->  wsize;
  dst -> output_tuple = src ->  output_tuple;
  dst -> comp_time = src -> comp_time;
}


int join_predicate(int i, int j, unsigned int *Rk, unsigned int *Sk){
  return (((Rk[2*i+0] >= Sk[2*j+0] - 10) &&
           (Rk[2*i+0] <= Sk[2*j+0] + 10)) &&
          ((Rk[2*i+1] >= Sk[2*j+1] - 10) &&
           (Rk[2*i+1] <= Sk[2*j+1] + 10)));
  
};

void func_join(unsigned int* Rk, unsigned int* Sk, int* rnum,
               unsigned int** result,
               int wsize, int tsize, int n_threads){
  
  int k, ri, sj;
  
  for(int i=0;i<wsize; i++){
#pragma omp parallel for num_threads(n_threads)
    for(int idx = 0; idx < n_threads; idx++){
      for(int j = (wsize / n_threads) * (idx + 0);
          j < (wsize / n_threads) * (idx + 1); j++){
        if(join_predicate(i, j, Rk, Sk)){
          result[idx][(rnum[idx] % 8) * 4 + 0] = Rk[2*i+0];
          result[idx][(rnum[idx] % 8) * 4 + 1] = Rk[2*i+1];
          result[idx][(rnum[idx] % 8) * 4 + 2] = Sk[2*j+0];
          result[idx][(rnum[idx] % 8) * 4 + 3] = Sk[2*j+1];
          rnum[idx]++;
        }
      }
    }
  }
  
  for(k=wsize; k<tsize-1;k++){
    ri = k;
    sj = k;

#pragma omp parallel for num_threads(n_threads)
    for(int idx = 0; idx < n_threads; idx++){
      for(int j=(k-wsize)+((idx+0) * wsize/n_threads);
          j<(k-wsize)+((idx+1) * wsize/n_threads);
          j++){
        if(join_predicate(ri, j, Rk, Sk)){
          result[idx][(rnum[idx] % 8) * 4 + 0] = Rk[2*ri+0];
          result[idx][(rnum[idx] % 8) * 4 + 1] = Rk[2*ri+1];
          result[idx][(rnum[idx] % 8) * 4 + 2] = Sk[2*j+0];
          result[idx][(rnum[idx] % 8) * 4 + 3] = Sk[2*j+1];
          rnum[idx]++;
        }
      }
    }
    
#pragma omp parallel for num_threads(n_threads)
    for(int idx = 0; idx < n_threads; idx++){
      for(int i=(k-wsize)+((idx+0) * wsize/n_threads);
          i<(k-wsize)+((idx+1) * wsize/n_threads);
          i++){
        if(join_predicate(i, sj, Rk, Sk)){
          result[idx][(rnum[idx] % 8) * 4 + 0] = Rk[2*i+0];
          result[idx][(rnum[idx] % 8) * 4 + 1] = Rk[2*i+1];
          result[idx][(rnum[idx] % 8) * 4 + 2] = Sk[2*sj+0];
          result[idx][(rnum[idx] % 8) * 4 + 3] = Sk[2*sj+1];
          rnum[idx]++;
        }
      }
    }
  }
};



void tamamo_swhj(joinOpeItem* joi, UINT32 *Rk_buffer, UINT32 *Sk_buffer, int n_threads){
  double te, ts;
  ts = get_dtime();

  unsigned int *Rk, *Sk,**result;
  int WSIZE, TSIZE;
  int *rnum;

  WSIZE = joi->wsize;
  TSIZE = joi->wsize;

  Rk = (unsigned int*)malloc(sizeof(unsigned int) * 2 * TSIZE);
  Sk = (unsigned int*)malloc(sizeof(unsigned int) * 2 * TSIZE);
    
  rnum = (int*)malloc(sizeof(int) * n_threads);
  for(int i=0; i<n_threads; i++){
    rnum[i] = 0;
  }
  
  result = (unsigned int**)malloc(sizeof(unsigned int*) * n_threads);
  for(int i=0; i<n_threads; i++){
    result[i] = (unsigned int*)malloc(sizeof(unsigned int) * 4 * 8);
  }

  for(int j=0; j<WSIZE; j++){
    Rk[j * 2 + 0] = Rk_buffer[j * 8 + 4 + 3];
    Rk[j * 2 + 1] = Rk_buffer[j * 8 + 4 + 2];
  }  
  for(int j=0; j<WSIZE; j++){
    Sk[j * 2 + 0] = Sk_buffer[j * 8 + 4 + 3];
    Sk[j * 2 + 1] = Sk_buffer[j * 8 + 4 + 2];
  }

  fprintf(stderr,"[tamamo DEBUG] swhj\n");
  ts = get_dtime();

  func_join(Rk, Sk, rnum, result, WSIZE, TSIZE, n_threads);

  te = get_dtime();
  joi->comp_time = te - ts;

  fprintf(stderr,"[tamamo DEBUG] Local swhj Time %6.5f [sec]\n", joi->comp_time);
  fprintf(stderr,"[tamamo DEBUG] Throughput %6.5f [Mt/s]\n\n\n",
          ((float)TSIZE * 2.0 * 2.0) / joi->comp_time / 1000000.0);

}






#endif
