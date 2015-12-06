// macdetect, a daemon and clients for detecting MAC addresses
// Copyright (C) 2015 Jan Winkler <jan.winkler.84@gmail.com>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

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
