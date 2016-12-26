#include <iostream>

#include <Poco/Net/Socket.h>
#include <Poco/Net/StreamSocket.h>
#include "tmmapx-swhj.hpp"

#define DEBUG
#define TIME_BENCH

typedef struct {
  int number;
  char name[20];
} test_opeItem;


int main(){
  int port = 33039;
  std::string host = "localhost";
  
  test_opeItem daemonOpe;
  daemonOpe.number = 3939;
  strcpy(daemonOpe.name, "tamamo");

  while (true) {
    try {
      Poco::Net::StreamSocket *ss;
      auto address = Poco::Net::SocketAddress(host, port);
      ss = new Poco::Net::StreamSocket(address.family());
      ss->setNoDelay(true);
      ss->connect(address);
      
      try {

        while (true) {
          int ret = ss -> sendBytes(&daemonOpe, sizeof(test_opeItem)-1);
          if (ret < 0){
            throw std::exception();
          }
          daemonOpe.number++;
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
  
  
