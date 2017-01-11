#include <iostream>

#include <Poco/Net/Socket.h>
#include <Poco/Net/StreamSocket.h>

#include "tmmapx-swhj.hpp"
#include "tmmapx-net.hpp"

#define DEBUG
#define TIME_BENCH

void aggr_distribute(int port, std::string host, aggrOpeItem * daemonOpe,   UINT8 *streamS){
  send_ope(port, host, daemonOpe);
  fprintf(stderr,"[tamamo DEBUG] send ope\n");
  tmm_send(port, host, streamS, daemonOpe->dsize);
  //tmm_send(port, host, streamS, daemonOpe->dsize);
  fprintf(stderr,"[tamamo DEBUG] send R S data\n");
}

void swhj_check_comp(int port, std::string host, aggrOpeItem * daemonOpe){
  tmm_pullOpe(port, host, daemonOpe);  
}

int main(int argc, char** argv){

  //Data Initialize 
  if(argc != 5){
    printf("[tamamo DEBUG]%s HW_core(core) nodes(node_num) wsize(wsize on 1 node) tuples\n", argv[0]);
    exit(1);
  }

  int /*core,*/ node_num, wsize, tuples;
  //sscanf(argv[1], "%d", &core);
  sscanf(argv[2], "%d", &node_num);
  sscanf(argv[3], "%d", &wsize);
  sscanf(argv[4], "%d", &tuples);
  //int window_block = node_num;
  int dsize = Tuple*NB_TUPLE;
  // int dsize = tuples * 32 + wsize *32;
  
  double te, ts;
  aggrOpeItem daemonOpe[node_num];
  //daemonOpe.dsize = 4096*8;
  //daemonOpe.wsize = 4096;
  daemonOpe.dsize = dsize;
  for( int i = 0; i<node_num; i++){
    tmm_aggrOpeAsgmt(&daemonOpe[i],0,0,0,0,dsize,wsize,0,0);
    tmm_aggrOpePrint(&daemonOpe[i]);
  }
  
  const char *fname = "./data/NASDAQ_merge.csv";
  //const char *fnameR = "input_R.bin";
  //const char *fnameS = "input_S.bin";
  FILE  *fiS;
  UINT8 *streamS;

  file = myfopen(fname,"rb");
  //fiR = myfopen(fnameR, "rb");
  //fiS = myfopen(fnameS, "rb");
  streamS = (UINT8 *)malloc(sizeof(UINT8)*dsize);
  //streamR = (UINT8 *)malloc(sizeof(UINT8)*dsize);
  fread(streamS, sizeof(UINT8), dsize, fiS);
  //fread(streamS, sizeof(UINT8), dsize, fiS);
  fclose(fiS); //fclose(fiS);
  
  //Inform 
  fprintf(stderr,"[tamamo DEBUG](INPUT Parametor) HW_core(%d) node_num(%d) tuples(%d) \n",core, node_num, tuples);
  fprintf(stderr,"[tamamo DEBUG](Examination Parametor) wsize(%d) dsize(%d) \n", wsize, dsize);
  fprintf(stderr,"[tamamo DEBUG]:Message Are you ready?\n");
  fprintf(stderr,"[tamamo DEBUG]:Message 1 \n");
  sleep(1);

  //send_ope(33039,'localhost',&daemonOpe);
  //swhj_distribute(33039, "192.168.137.2", &daemonOpe[1], streamR, streamS);    

  std::string addr_prefix("192.168.137.");


  ts = get_dtime();
  
  //ノード数が増えると、ウィンドウサイズが拡張され、１処理にnode_num*2[Stage]かかる
  for(int step = 0 ; step < node_num*2; step++){
    //ノンブロッキングデータ配布
    for(int i=0; i < node_num;i++){
      aggr_distribute(33039, addr_prefix + std::to_string(i+1), &daemonOpe[i], streamS); 
    }
    
    for(int i=0; i < node_num;i++){
      aggr_check_comp(33039, addr_prefix + std::to_string(i+1), &daemonOpe[i]);
    }
  }
  te = get_dtime();

  fprintf(stderr,"[tamamo] Total Time %6.5f [sec]\n", te - ts);
  fprintf(stderr,"Throughput %6.7lf [Mt/s]\n\n\n",
          (((float)tuples * 2 * node_num * 2)) / (te-ts) / 1000000.0);
}
  
  
