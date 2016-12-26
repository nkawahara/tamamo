#ifndef _TMMAPX_SWHJ_
#define _TMMAPX_SWHJ_

#define UINT64 unsigned long long int
#define UINT32 unsigned int
#define UINT16 unsigned short
#define UINT8  unsigned char


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





#endif
