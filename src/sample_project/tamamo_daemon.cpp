#include <iostream>

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
  
  joinOpeItem buffer_ope;
  
  for(;;){
    ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
    ss -> setNoDelay(true);
    tmm_recv(ss, &buffer_ope, sizeof(joinOpeItem));
    ss->close();
    tmm_joinOpePrint(&buffer_ope);
    
    //データを1つ受信する
    UINT32 *Rk_buffer, *Sk_buffer;
    Rk_buffer = (unsigned int*)malloc(buffer_ope.dsize);
    Sk_buffer = (unsigned int*)malloc(buffer_ope.dsize);


    ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
    ss -> setNoDelay(true);
    tmm_recv(ss, Rk_buffer, buffer_ope.dsize);
    ss->close();
    
    ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
    ss -> setNoDelay(true);
    tmm_recv(ss, Sk_buffer, buffer_ope.dsize);
    ss->close();

    //swhj
    tamamo_swhj(&buffer_ope, Rk_buffer, Sk_buffer, n_threads);

    //return result 
    ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
    ss -> setNoDelay(true);
    ss -> setBlocking(true);
    //tmm_recv(ss, &buffer_ope, sizeof(joinOpeItem));
    ss -> sendBytes(&buffer_ope, sizeof(joinOpeItem));
    ss->close();
  }  

}

int main(int argc, char** argv){
  if(argc != 2){
    printf("%s thread_num\n", argv[0]);
    exit(1);
  }

  daemon_loop(atoi(argv[1]));  
}
  
  
