// Copyright 2016 Jan Winkler <jan.winkler.84@gmail.com>
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** \author Jan Winkler */


#include <macdetectd/RARP.h>


namespace macdetect {
  RARP::RARP() {
  }
  
  RARP::~RARP() {
  }
  
  void RARP::requestIP(std::string strMAC, std::shared_ptr<Device> dvDevice) {
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
