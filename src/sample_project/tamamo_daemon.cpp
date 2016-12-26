#include <iostream>

#include <Poco/Net/Socket.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Net/StreamSocket.h>

#include "tmmapx-swhj.hpp"

#define DEBUG
#define TIME_BENCH

int main(){
  Poco::Net::ServerSocket *serv;
  Poco::Net::StreamSocket *ss;

  serv = new Poco::Net::ServerSocket(33039);
  serv -> listen();
  
  joinOpeItem buffer_ope;

  for(;;){
    ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
    ss -> setNoDelay(true);
    std::cout << "Waiting for connection";

    ss -> receiveBytes( &buffer_ope, sizeof(joinOpeItem)-1 );    
    fprintf(stderr, "dsize(%d) wsize(%d)\n",buffer_ope.dsize, buffer_ope.wsize);

    ss->close();
    //serv->close();        
  }
}
  
  
