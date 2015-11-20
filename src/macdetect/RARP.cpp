#include <macdetect/RARP.h>


namespace macdetect {
  RARP::RARP() {
  }
  
  RARP::~RARP() {
  }
  
  void RARP::requestIP(std::string strMAC, Device* dvDevice) {
    // TODO: Send request for IP
    RARPPacket rpRequest;
    rpRequest.un16HardwareType = 0x0100; // Ethernet
    rpRequest.un16ProtocolType = 0x0008; // IP
    rpRequest.un8HardwareAddressLength = 6;
    rpRequest.un8ProtocolAddressLength = 4;
    rpRequest.un16Opcode = 0x0300; // Request
    
    struct ether_addr* eaSource = ether_aton(dvDevice->mac().c_str());
    memcpy(&rpRequest.uc6SourceHWAddr, eaSource, sizeof(rpRequest.uc6SourceHWAddr));
    
    struct ether_addr* eaDestination = ether_aton(strMAC.c_str());
    memcpy(&rpRequest.uc6DestinationHWAddr, eaDestination, sizeof(rpRequest.uc6DestinationHWAddr));
    
    memset(&rpRequest.uc4SourceProtocolAddr, 0, sizeof(rpRequest.uc4SourceProtocolAddr));
    memset(&rpRequest.uc4DestinationProtocolAddr, 0, sizeof(rpRequest.uc4DestinationProtocolAddr));
    
    int nPacketLength = sizeof(struct ethhdr) + sizeof(RARPPacket);
    char carrBuffer[nPacketLength];
    memset(carrBuffer, 0, nPacketLength);
    Wire::wrapInEthernetFrame(dvDevice->mac().c_str(), "ff:ff:ff:ff:ff:ff", 0x0806, &rpRequest, sizeof(RARPPacket), carrBuffer);
    
    dvDevice->wire()->write(carrBuffer, nPacketLength);
  }
  
  std::string RARP::ipFromResponse(RARPPacket rpResponse) {
    std::string strIP = "";
    
    // TODO: Extract IP from rpResponse
    
    return strIP;
  }
}
