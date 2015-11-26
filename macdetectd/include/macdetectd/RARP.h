#ifndef __RARP_H__
#define __RARP_H__


// System
#include <net/ethernet.h>
#include <netinet/ether.h>

// MAC Detect
#include <macdetectd/Device.h>


namespace macdetect {
  typedef struct {
    uint16_t un16HardwareType;
    uint16_t un16ProtocolType;
    uint8_t un8HardwareAddressLength;
    uint8_t un8ProtocolAddressLength;
    uint16_t un16Opcode;
    unsigned char uc6SourceHWAddr[6];
    unsigned char uc4SourceProtocolAddr[4];
    unsigned char uc6DestinationHWAddr[6];
    unsigned char uc4DestinationProtocolAddr[4];
  } RARPPacket;
  
  
  class RARP {
  private:
  protected:
  public:
    RARP();
    ~RARP();
    
    void requestIP(std::string strMAC, Device* dvDevice);
    std::string ipFromResponse(RARPPacket rpResponse);
  };
}


#endif /* __RARP_H__ */
