#include <Poco/Net/Socket.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Net/StreamSocket.h>

void tmm_recv(Poco::Net::StreamSocket *ss, void *data, int dsize){
  
  bool continue_flag = true;
  int usedSpace = 0;
  int freeSpace = dsize;

  std::cout << "Waiting for connection" << std::endl;
  while (continue_flag){
    void* bufferPtr = data + usedSpace;
    int bytesRecv = ss -> receiveBytes(bufferPtr, freeSpace);

    if (bytesRecv > 0){      
      usedSpace += bytesRecv;
      freeSpace -= bytesRecv;
    }
    else{
      
      return;
    }
  }

}


void tmm_send(int port, std::string host, void *data, int dsize){
  bool continue_flag = true;

  while (continue_flag) {
    try {
      Poco::Net::StreamSocket *ss;
      auto address = Poco::Net::SocketAddress(host, port);
      ss = new Poco::Net::StreamSocket(address.family());
      ss->setNoDelay(true);
      ss->setBlocking(true);
      ss->connect(address);

      try {

        while (true) {
          int ret = ss -> sendBytes(data, dsize);
          if (ret < 0){
            throw std::exception();
          }
          return;
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



void send_ope(int port, std::string host,joinOpeItem *daemonOpe){
  while (true) {
    try {
      Poco::Net::StreamSocket *ss;
      auto address = Poco::Net::SocketAddress(host, port);
      ss = new Poco::Net::StreamSocket(address.family());
      ss->setNoDelay(true);
      ss->setBlocking(true);
      ss->connect(address);

      try {

        while (true) {
          int ret = ss -> sendBytes(daemonOpe, sizeof(joinOpeItem));
          if (ret < 0){
            throw std::exception();
          }
          return;
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
