#include <Poco/Net/Socket.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Net/StreamSocket.h>

void tmm_recv(Poco::Net::StreamSocket *ss, void *data, int dsize){
  bool continue_flag = true;
  int usedSpace = 0;
  int freeSpace = dsize;

  std::cerr << "[tamamo DEBUG]Waiting for connection" << std::endl;
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
      //ss->setBlocking(true);
      ss->connect(address);

      try {
        
        while (true) {
          int ret = ss -> sendBytes(data, dsize);
          if (ret < 0){
            throw std::exception();
          }
          ss->close();
          return;
        }
      }
      catch (...) {
        ss->close();
      }
    }
    catch (...) {
      usleep(1000);
      std::cerr << "[tamamo DEBUG]Waiting for server to startup\n";
    }
  }
}


// Connection pull from slave
void tmm_pull(int port, std::string host, void *data, int dsize){
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
          int ret = ss -> receiveBytes(data, dsize);
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
      usleep(1000);
      std::cerr << "[tamamo DEBUG]Waiting for server to startup\n";
    }
  }
}




// ***************************************
// test version for HJ
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
          ss->close();
          return;
        }
      }
      catch (...) {
        ss->close();
      }
    }
    catch (...) {
      usleep(1000);
      std::cerr << "[tamamo DEBUG]Waiting for server to startup\n";
    }
  }
}


// Connection pull from slave
void tmm_pullOpe(int port, std::string host, joinOpeItem *daemonOpe){
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
          int ret = ss -> receiveBytes(daemonOpe, sizeof(joinOpeItem));
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
      usleep(1000);
      std::cerr << "[tamamo DEBUG]Waiting for server to startup\n";
    }
  }
}

// ***************************************
