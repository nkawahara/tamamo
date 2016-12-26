#include <iostream>

#include <Poco/Net/Socket.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Net/StreamSocket.h>

#include "tmmapx-swhj.hpp"

#define DEBUG
#define TIME_BENCH

typedef struct {
  int number;
  char name[20];
} test_opeItem;


int main(){
  Poco::Net::ServerSocket *serv;
  Poco::Net::StreamSocket *ss;

  serv = new Poco::Net::ServerSocket(33039);
  serv -> listen();
  
  test_opeItem buffer_ope;

  for(;;){
    ss = new Poco::Net::StreamSocket(serv -> acceptConnection());
    ss -> setNoDelay(true);
    std::cout << "Waiting for connection";

    ss -> receiveBytes( &buffer_ope, sizeof(buffer_ope)-1 );    
    fprintf(stderr, "%d %s\n",buffer_ope.number, buffer_ope.name);

    ss->close();
    //serv->close();        
  }
}
  
  
