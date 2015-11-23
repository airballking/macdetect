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


#include <macdetect/Wire.h>


namespace macdetect {
  Wire::Wire(std::string strDeviceName, int nDefaultReadingLength, unsigned short usProtocol) : m_strDeviceName(strDeviceName), m_usProtocol(usProtocol) {
    this->setSocket(this->createSocket(strDeviceName, usProtocol));
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
  
  int Wire::createSocket(std::string strDeviceName, unsigned short usProtocol) {
    int nSocket = ::socket(PF_PACKET, SOCK_RAW, htons(usProtocol));
    
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
	    ::close(nSocket);
	    nSocket = -1;
	  }
	} else {
	  ::close(nSocket);
	  nSocket = -1;
	}
      } else {
	::close(nSocket);
	nSocket = -1;
      }
    }
    
    if(nSocket == -1) {
      std::cout << "Error while creating socket (device " << strDeviceName << "): " << strerror(errno) << std::endl;
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
      std::cout << "Sending to non-default protocol 0x" << std::hex << usProtocol << std::endl;
    }
    
    if((nWritten = ::write(nSocket, vdBuffer, unLength)) == -1) {
      std::cerr << "Failure: " << strerror(errno) << std::endl;
      
      bSuccess = false;
    }
    
    if(usProtocol != m_usProtocol) {
      ::close(nSocket);
    }
    
    return bSuccess;
  }
}
