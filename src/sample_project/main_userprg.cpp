#include <iostream>

#include <Poco/Net/Socket.h>
#include <Poco/Net/StreamSocket.h>
#include "tmmapx-swhj.hpp"

#define DEBUG
#define TIME_BENCH

int main(){
  int port = 33039;
  std::string host = "localhost";
  
  joinOpeItem daemonOpe;
  daemonOpe.dsize = 4096*8;
  daemonOpe.wsize = 4096;

  while (true) {
    try {
      Poco::Net::StreamSocket *ss;
      auto address = Poco::Net::SocketAddress(host, port);
      ss = new Poco::Net::StreamSocket(address.family());
      ss->setNoDelay(true);
      ss->connect(address);
      
      try {

        while (true) {
          int ret = ss -> sendBytes(&daemonOpe, sizeof(joinOpeItem)-1);
          if (ret < 0){
            throw std::exception();
          }
          daemonOpe.dsize ++;
        }
      }
      catch (...) {
        ss->close();
      }
    }
    catch (...) {
      sleep(1);
      std::cout << "Waiting for server to startup\n";
    }
  }
}
  
  
