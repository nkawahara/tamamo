#include <iostream>

#include <Poco/Net/Socket.h>
#include <Poco/Net/StreamSocket.h>

#include "tmmapx-swhj.hpp"
#include "tmmapx-net.hpp"

#define DEBUG
#define TIME_BENCH


int main(int argc, char** argv){
  //int port = 33039;
  //std::string host = "localhost";
 
  //Data Initialize 
  if(argc != 5){
    printf("%s HW_core(core) nodes(node_num) wsize(wsize on 1 node) tuples\n", argv[0]);
    exit(1);
  }
    int core, node_num, wsize, tuples;
  sscanf(argv[1], "%d", &core);
  sscanf(argv[2], "%d", &node_num);
  sscanf(argv[3], "%d", &wsize);
  sscanf(argv[4], "%d", &tuples);
  //int window_block = node_num;
  int dsize = tuples * 32 + wsize *32;
  
  double te, ts;
  joinOpeItem daemonOpe;
  //daemonOpe.dsize = 4096*8;
  //daemonOpe.wsize = 4096;
  tmm_joinOpeAsgmt(&daemonOpe,0,0,0,0,dsize,wsize,0,0);
  tmm_joinOpePrint(&daemonOpe);
  
  const char *fnameR = "input_R.bin";
  const char *fnameS = "input_S.bin";
  FILE *fiR, *fiS;
  UINT8 *streamR, *streamS;
  fiR = myfopen(fnameR, "rb");
  fiS = myfopen(fnameS, "rb");
  streamR = (UINT8 *)malloc(sizeof(UINT8)*dsize);
  streamS = (UINT8 *)malloc(sizeof(UINT8)*dsize);
  fread(streamR, sizeof(UINT8), dsize, fiR);
  fread(streamS, sizeof(UINT8), dsize, fiS);
  fclose(fiR); fclose(fiS);
  
  //Inform 
  fprintf(stderr,"(INPUT Parametor) HW_core(%d) node_num(%d) tuples(%d) \n",core, node_num, tuples);
  fprintf(stderr,"(Examination Parametor) wsize(%d) dsize(%d) \n", wsize, dsize);
  fprintf(stderr,"(Tamamo Message) Are you ready?\n");
  fprintf(stderr,"(Tamamo Message)1\n");
  sleep(1);

  //send_ope(33039,'localhost',&daemonOpe);

  ts = get_dtime();
  send_ope(33039, "192.168.137.2", &daemonOpe);
  fprintf(stderr,"(Tamamo Message) send ope\n");
  tmm_send(33039,"192.168.137.2", streamR, dsize);
  tmm_send(33039,"192.168.137.2", streamS, dsize);
  fprintf(stderr,"(Tamamo Message) send R S data\n");
  
  //daemon local serv
  tmm_pullOpe(33039,"192.168.137.2",&daemonOpe);

  te = get_dtime();
  
  fprintf(stderr,"Total Time %6.5f [sec]\n", te - ts);
  fprintf(stderr,"Throughput %6.5f [Mt/s]\n\n\n",
          (float) dsize/16.0 / ( te - ts ) / 1000000.0);  
}
  
  
