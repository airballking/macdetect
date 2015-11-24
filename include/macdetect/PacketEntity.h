#ifndef __PACKETENTITY_H__
#define __PACKETENTITY_H__


// System
#include <unistd.h>
#include <sys/socket.h>

// MAC detect
#include <macdetect/Packet.h>


namespace macdetect {
  class PacketEntity {
  private:
  protected:
    int m_nSocketFD;
    bool m_bFailureState;
    
  public:
    PacketEntity(int nSocketFD);
    ~PacketEntity();
    
    bool sendPacket(Packet* pktSend);
    Packet* receivePacket();
    
    int socket();
    bool failureState();
  };
}


#endif /* __PACKETENTITY_H__ */
