#include <iostream>


//#include <time.h>
#include <Poco/Net/Socket.h>
#include <Poco/Net/StreamSocket.h>

#include "tmmapx-swhj.hpp"
//#include "tmmapx-swaggr.hpp"
#include "tmmapx-swaggr.hpp"
#include "tmmapx-net.hpp"

#define DEBUG
#define TIME_BENCH

void aggr_distribute(int port, std::string host, aggropeItem * daemonOpe, Tuple * send_data ){
  //send_ope(port, host, daemonOpe);
  tmm_send(port, host, send_data, sizeof(aggropeItem));
  fprintf(stderr,"[tamamo DEBUG] send ope\n");
  tmm_send(port, host, send_data, daemonOpe->dsize);
  //tmm_send(port, host, streamS, daemonOpe->dsize);
  fprintf(stderr,"[tamamo DEBUG] send Aggragation data\n");
}

void aggr_check_comp(int port, std::string host, aggropeItem * daemonOpe){
  //tmm_pullOpe(port, host, daemonOpe);  
  tmm_pull(port, host, daemonOpe, sizeof(aggropeItem));  
}


int main(int argc, char** argv){
  //Data Initialize 
  if(argc != 2){
    printf("[tamamo DEBUG]%s nodes(node_num) \n", argv[0]);
    exit(1);
  }

  int node_num;
  sscanf(argv[1], "%d", &node_num);

  /*

  int core, node_num, wsize, tuples;
  //sscanf(argv[1], "%d", &core);
  sscanf(argv[2], "%d", &node_num);
  sscanf(argv[3], "%d", &wsize);
  sscanf(argv[4], "%d", &tuples);
  //int window_block = node_num;
  // int dsize = tuples * 32 + wsize *32;
  
  double te, ts;
  aggrOpeItem daemonOpe[node_num];
  //daemonOpe.dsize = 4096*8;
  //daemonOpe.wsize = 4096
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

  */



  aggrGroupby masterGroup;
  //aggrGroupby masterGroup;
  
  masterGroup.makeTuple(); //データを読み込みしてクラスに格納
  int nb_tuple = masterGroup.getNBTuple();
  int nb_group = masterGroup.getNBGroup();
  int dsize = sizeof(Tuple)*nb_tuple;
  
  Tuple* master_data = masterGroup.getTupleList();
  
  //構造体作成及び，swaggrに必要なパラメータの格納
  aggropeItem daemonOpe[node_num]; 
  
  double ts,te;
  
  for( int i = 0; i<node_num; i++){
    tmm_aggrOpeAsgmt(&daemonOpe[i],0,0,dsize,0,0,0,nb_group);
  }
  
  
  std::string addr_prefix("192.168.137.");
  ts = get_dtime();
  
  for(int step = 0 ; step < node_num; step++){
    //ノンブロッキングデータ配布
    for(int i=0; i < node_num;i++){
      aggr_distribute(33039, addr_prefix + std::to_string(i+1), &daemonOpe[i], master_data); 
    }
    
    for(int i=0; i < node_num;i++){
      aggr_check_comp(33039, addr_prefix + std::to_string(i+1), &daemonOpe[i]);
    }
  }
  te = get_dtime();
  
  fprintf(stderr,"[tamamo] Total Time %6.5f [sec]\n", te - ts);
  fprintf(stderr,"Throughput %6.7lf [Mt/s]\n\n\n",
          (((float)dsize * 2 * node_num * 2)) / (te-ts) / 1000000.0);
}
  
  
