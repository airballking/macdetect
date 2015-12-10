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


#include <macdetectd/Wire.h>


namespace macdetect {
  Wire::Wire(std::string strDeviceName, int nDefaultReadingLength, unsigned short usProtocol, bool bSilent) : m_strDeviceName(strDeviceName), m_usProtocol(usProtocol) {
    this->setSocket(this->createSocket(strDeviceName, usProtocol, bSilent));
    this->setDefaultReadingLength(nDefaultReadingLength);
  }
  
  Wire::~Wire() {
  }
  
  int Wire::wrapInEthernetFrame(std::string strSourceMAC, std::string strDestinationMAC, unsigned short usEtherType, void* vdPayload, unsigned int unPayloadLength, void* vdBuffer) {
    int nBytes = sizeof(struct ethhdr) + unPayloadLength /*+ 4*/ /* FCS */;
    
    struct ethhdr ehFrame;
    unsigned short usEtherTypeN = htons(usEtherType);
    memcpy(&(ehFrame.h_proto), &usEtherTypeN, 2);
    
    memset(vdBuffer, 0, nBytes);
    struct ether_addr* eaSource = ether_aton(strSourceMAC.c_str());
    memcpy(ehFrame.h_source, eaSource, sizeof(struct ether_addr));
    struct ether_addr* eaDestination = ether_aton(strDestinationMAC.c_str());
    memcpy(ehFrame.h_dest, eaDestination, sizeof(struct ether_addr));
    
    memcpy(vdBuffer, &ehFrame, sizeof(struct ethhdr));
    memcpy(&(((unsigned char*)vdBuffer)[sizeof(struct ethhdr)]), vdPayload, unPayloadLength);
    
    // TODO: Calculate and set the FCS; right now, nulling it out.
    // NOTE: Just leave it out for now.
    //memset(&(((unsigned char*)vdBuffer)[sizeof(struct ethhdr) + unPayloadLength]), 0, 4);
    
    return nBytes;
  }
  
  int Wire::createSocket(std::string strDeviceName, unsigned short usProtocol, bool bSilent) {
    int nSocket = ::socket(PF_PACKET, SOCK_RAW, htons(usProtocol));
    std::string strErrorSource = "socket";
    
    if(nSocket > -1 && strDeviceName != "") {
      struct ifreq ifr;
      memset(&ifr, 0, sizeof(ifr));
      
      strncpy(ifr.ifr_name, strDeviceName.c_str(), sizeof(ifr.ifr_name) - 1);
      ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = '\0';
      
      if(ioctl(nSocket, SIOCGIFINDEX, &ifr) > -1) {
	struct sockaddr_ll socket_address;
	memset(&socket_address, 0, sizeof(socket_address));
	socket_address.sll_family = AF_PACKET;
	socket_address.sll_ifindex = ifr.ifr_ifindex;
	socket_address.sll_protocol = htons(usProtocol);
	socket_address.sll_halen = ETH_ALEN;
      
	struct ifreq ifrTemp;
	memset(&ifrTemp, 0, sizeof(ifrTemp));
	strcpy(ifrTemp.ifr_name, strDeviceName.c_str());
      
	if(ioctl(nSocket, SIOCGIFHWADDR, &ifrTemp) > -1) {
	  std::string strDeviceMAC = "";
	  char carrBuffer[17];
	  int nOffset = 0;
    
	  for(int nI = 0; nI < 6; nI++) {
	    sprintf(&(carrBuffer[nI + nOffset]), "%.2x", (unsigned char)ifrTemp.ifr_hwaddr.sa_data[nI]);
	    nOffset++;
      
	    if(nI < 5) {
	      nOffset++;
	      carrBuffer[nI + nOffset] = ':';
	    }
	  }
      
	  strDeviceMAC = std::string(carrBuffer, 17);
      
	  struct ether_addr* eaSource = ether_aton(strDeviceMAC.c_str());
	  memcpy(socket_address.sll_addr, eaSource, sizeof(struct ether_addr));
	  
	  if(bind(nSocket, (sockaddr*)&socket_address, sizeof(socket_address)) == -1) {
	    strErrorSource = "bind";
	    
	    ::close(nSocket);
	    nSocket = -1;
	  }
	} else {
	  strErrorSource = "ioctl/ifhwaddr";
	  
	  ::close(nSocket);
	  nSocket = -1;
	}
      } else {
	strErrorSource = "ioctl/ifindex";
	
	::close(nSocket);
	nSocket = -1;
      }
    }
    
    if(nSocket == -1) {
      if(!bSilent) {
	log(Error, "Error while creating socket (device '%s', source '%s'): %s", strDeviceName.c_str(), strErrorSource.c_str(), strerror(errno));
      }
    }
    
    return nSocket;
  }
  
  bool Wire::write(void* vdBuffer, unsigned int unLength) {
    return this->write(vdBuffer, unLength, m_usProtocol);
  }
  
  bool Wire::write(void* vdBuffer, unsigned int unLength, unsigned short usProtocol) {
    bool bSuccess = true;
    
    int nWritten = 0;
    int nSocket = m_nSocket;
    
    if(usProtocol != m_usProtocol) {
      nSocket = this->createSocket(m_strDeviceName, usProtocol);
      
      std::stringstream sts;
      sts << std::hex;
      sts << usProtocol;
      
      log(Normal, "Sending to non-default protocol 0x%s", sts.str().c_str());
    }
    
    if((nWritten = ::write(nSocket, vdBuffer, unLength)) == -1) {
      log(Error, "Failure: %s", strerror(errno));
      
      bSuccess = false;
    }
    
    if(usProtocol != m_usProtocol) {
      ::close(nSocket);
    }
    
    return bSuccess;
  }
}
