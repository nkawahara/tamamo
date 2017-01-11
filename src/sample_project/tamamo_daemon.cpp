#include <iostream>
#include<omp.h>


#include <Poco/Net/Socket.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Net/StreamSocket.h>

#include "tmmapx-swhj.hpp"
#include "tmmapx-net.hpp"

#define DEBUG
#define TIME_BENCH

void daemon_loop(int n_threads){

  Poco::Net::ServerSocket *serv;
  Poco::Net::StreamSocket *ss;

  serv = new Poco::Net::ServerSocket(33039);
  serv -> listen();
  
  aggrOpeItem buffer_ope;
  //aggrOpeItem TupleList;

  UINT32 *Sk_buffer;//, *Rk_buffer;
  
  for(;;){
    ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
    ss -> setNoDelay(true);
    ss -> setBlocking(true);
    tmm_recv(ss, &buffer_ope, sizeof(aggrOpeItem));
    ss->close();
    tmm_aggrOpePrint(&daemonOpe);

    if( buffer_ope.dsize > 0){ 
      //データを2つ受信する
      //Rk_buffer = (unsigned int*)malloc(buffer_ope.dsize);
      Sk_buffer = (unsigned int*)malloc(buffer_ope.dsize);
      
      ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
      ss -> setNoDelay(true);
      tmm_recv(ss, Sk_buffer, buffer_ope.dsize);
      ss->close();
      /*ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
      ss -> setNoDelay(true);
      tmm_recv(ss, Sk_buffer, buffer_ope.dsize);
      ss->close();*/


      ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
      ss -> setNoDelay(true);
      ss -> setBlocking(true);      
      //swhj
      tamamo_aggr(&buffer_ope, Sk_buffer, n_threads);
      //return result 
      ss -> sendBytes(&buffer_ope, sizeof(aggrOpeItem));
      ss->close();
      //free(Rk_buffer);
      free(Sk_buffer);
      
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
  
  
