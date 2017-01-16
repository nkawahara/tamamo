#include <iostream>
#include<omp.h>


#include <Poco/Net/Socket.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Net/StreamSocket.h>

#include "tmmapx-swhj.hpp"
#include "tmmapx-net.hpp"
#include "tmmapx-swaggr.hpp"

#define DEBUG
#define TIME_BENCH

void daemon_loop(int n_threads){

  Poco::Net::ServerSocket *serv;
  Poco::Net::StreamSocket *ss;

  serv = new Poco::Net::ServerSocket(33039);
  serv -> listen();
  
  aggropeItem buffer_ope;

  Tuple *TupleList;
  
  for(;;){
    ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
    ss -> setNoDelay(true);
    ss -> setBlocking(true);
    tmm_recv(ss, &buffer_ope, sizeof(aggropeItem));
    ss->close();
    tmm_aggrOpePrint(&buffer_ope);

    if( buffer_ope.dsize > 0){ 
      //データを2つ受信する
      //Rk_buffer = (unsigned int*)malloc(buffer_ope.dsize);

      //箱作る
      TupleList = (Tuple *)malloc(buffer_ope.dsize);
      
      ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
      ss -> setNoDelay(true);

      //データサイズ分受信
      tmm_recv(ss, TupleList, buffer_ope.dsize);
      ss->close();
      /*ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
      ss -> setNoDelay(true);
      tmm_recv(ss, Sk_buffer, buffer_ope.dsize);
      ss->close();*/


      ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
      ss -> setNoDelay(true);
      ss -> setBlocking(true);
      
      //sw aggr GB=GroupBy
      tamamo_swGB(&buffer_ope, TupleList, n_threads);
      fprintf(stderr, "ここでAggregation処理を実行\n");
      //return result 

      ss -> sendBytes(&buffer_ope, sizeof(aggropeItem));
      ss->close();
      //free(Rk_buffer);
      free(TupleList);
    }  
    else{
      return;
    }
    
  }

}

int main(int argc, char** argv){
  if(argc != 2){
    printf("%s thread_num\n", argv[0]);
    exit(1);
  }

  daemon_loop(atoi(argv[1]));  
}
  
  
