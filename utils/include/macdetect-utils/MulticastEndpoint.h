#ifndef __MULTICAST_ENDPOINT_H__
#define __MULTICAST_ENDPOINT_H__


// System
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

// macdetect
#include <macdetect-utils/SocketReader.h>


namespace macdetect {
  class MulticastEndpoint : public SocketReader {
  private:
    std::string m_strMulticastGroup;
    unsigned short m_usPort;
    struct sockaddr_in m_saAddrGroup;
    struct sockaddr_in m_saAddrAny;
    
  protected:
  public:
    MulticastEndpoint(std::string strMulticastGroup, unsigned short usPort);
    ~MulticastEndpoint();
    
    bool ok();
    
    bool cycle();
    
    int write(void* vdBuffer, unsigned int unLength);
    int read(unsigned char* ucBuffer, unsigned int unLength, int nFlags) override;
  };
}


#endif /* __MULTICAST_ENDPOINT_H__ */
