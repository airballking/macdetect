#include <macdetect-utils/PacketEntity.h>


namespace macdetect {
  PacketEntity::PacketEntity(int nSocketFD) : m_nSocketFD(nSocketFD), m_bFailureState(false) {
  }
  
  PacketEntity::~PacketEntity() {
    ::close(m_nSocketFD);
  }
  
  int PacketEntity::socket() {
    return m_nSocketFD;
  }
  
  bool PacketEntity::sendPacket(Packet* pktSend) {
    unsigned char ucarrBuffer[2048];
    unsigned int unLength = pktSend->serialize(ucarrBuffer, 2048);
    
    int nResult = ::write(m_nSocketFD, ucarrBuffer, unLength);
    
    return nResult == unLength;
  }
  
  Packet* PacketEntity::receivePacket() {
    Packet* pktReceived = NULL;
    
    unsigned char ucarrBuffer[2048];
    int nLength = ::read(m_nSocketFD, ucarrBuffer, 2048);
    
    if(nLength == -1) {
      if(errno != EAGAIN && errno != EWOULDBLOCK) {
	m_bFailureState = true;
      }
    } else if(nLength == 0) {
      m_bFailureState = true;
    } else {
      pktReceived = new Packet();
      pktReceived->deserialize(ucarrBuffer, nLength);
    }
    
    return pktReceived;
  }
  
  bool PacketEntity::failureState() {
    return m_bFailureState;
  }
}
