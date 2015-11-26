#ifndef __CLIENT_H__
#define __CLIENT_H__


// System
#include <string>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

// MAC detect
#include <macdetect-utils/PacketEntity.h>


namespace macdetect {
  class Client : public PacketEntity {
  private:
  protected:
  public:
    Client();
    ~Client();
    
    bool connect(std::string strIP, unsigned short usPort = 7090);
    bool disconnect();
    
    bool connected();
  };
}


#endif /* __CLIENT_H__ */
