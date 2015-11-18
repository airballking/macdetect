#ifndef __SOCKET_READER_H__
#define __SOCKET_READER_H__


#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netpacket/packet.h>


namespace detect {
  class SocketReader {
  private:
    int m_nDefaultReadingLength;
    unsigned char* m_ucInternalBuffer;
    
  protected:
    int m_nSocket;
    
  public:
    SocketReader();
    ~SocketReader();
    
    int read(unsigned char* ucBuffer, unsigned int unLength);
    int bufferedRead(unsigned char** ucBuffer);
    int bufferedRead(unsigned char** ucBuffer, int nReadingLength);
    void setDefaultReadingLength(int nDefaultReadingLength);
    int defaultReadingLength();
    
    int forward(unsigned int unBytes);
    
    void setSocket(int nSocket);
    int socket();
  };
}


#endif /* __SOCKET_READER_H__ */
