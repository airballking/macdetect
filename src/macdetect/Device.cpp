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


#include <macdetect/Device.h>


namespace macdetect {
  Device::Device(std::string strDeviceName, HardwareType hwtType) : m_strDeviceName(strDeviceName), m_hwtType(hwtType), m_bUp(false), m_bRunning(false), m_wrWire(strDeviceName, ETH_FRAME_LEN), m_strMAC(""), m_strIP(""), m_strBroadcastIP(""), m_dPingBroadcastLastSent(0) {
  }
  
  Device::~Device() {
  }
  
  std::string Device::deviceName() {
    return m_strDeviceName;
  }
  
  Device::HardwareType Device::hardwareType() {
    return m_hwtType;
  }
  
  std::string Device::mac() {
    if(m_strMAC == "") {
      struct ifreq ifrTemp;
      
      memset(&ifrTemp, 0, sizeof(ifrTemp));
      strcpy(ifrTemp.ifr_name, this->deviceName().c_str());
      ioctl(m_wrWire.socket(), SIOCGIFHWADDR, &ifrTemp);
      
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
      
      m_strMAC = strDeviceMAC;
    }
    
    return m_strMAC;
  }
  
  void Device::setIP(std::string strIP) {
    m_strIP = strIP;
  }
  
  std::string Device::ip() {
    return m_strIP;
  }
  
  void Device::setBroadcastIP(std::string strIP) {
    m_strBroadcastIP = strIP;
  }
  
  std::string Device::broadcastIP() {
    return m_strBroadcastIP;
  }
  
  void Device::setUp(bool bUp) {
    m_bUp = bUp;
  }
  
  void Device::setRunning(bool bRunning) {
    m_bRunning = bRunning;
  }
  
  bool Device::up() {
    return m_bUp;
  }
  
  bool Device::running() {
    return m_bRunning;
  }
  
  unsigned char* Device::read(int& nLengthRead) {
    unsigned char* ucBuffer;
    nLengthRead = m_wrWire.defaultReadingLength();
    nLengthRead = m_wrWire.bufferedRead(&ucBuffer, nLengthRead);
    
    return ucBuffer;
  }
  
  bool Device::sendPingBroadcast(double dTime, double dInterval) {
    if(dTime >= m_dPingBroadcastLastSent + dInterval) {
      m_dPingBroadcastLastSent = dTime;
      
      return this->sendPing("255.255.255.255"/*this->broadcastIP()*/, "ff:ff:ff:ff:ff:ff");
    }
    
    return false;
  }
  
  bool Device::sendPing(std::string strDestinationIP, std::string strDestinationMAC) {
    if(this->ip() != "" && this->broadcastIP() != "") {
      // IP
      struct iphdr iphHeader;
      memset(&iphHeader, 0, sizeof(struct iphdr));
      
      iphHeader.version = 4;
      iphHeader.ihl = 21;
      iphHeader.tot_len = htons(84);
      iphHeader.frag_off = 0x0040;
      iphHeader.ttl = 64;
      iphHeader.protocol = 1; // ICMP
      
      struct in_addr iaAddr;
      inet_aton(this->ip().c_str(), &iaAddr); // The device's own IP
      iphHeader.saddr = iaAddr.s_addr;
      inet_aton(strDestinationIP.c_str(), &iaAddr);
      iphHeader.daddr = iaAddr.s_addr;
      
      iphHeader.check = this->ipHeaderChecksum(&iphHeader, sizeof(struct iphdr));
      
      // ICMP
      struct icmphdr icmphHeader;
      memset(&icmphHeader, 0, sizeof(struct icmphdr));
      
      icmphHeader.type = ICMP_ECHO;
      icmphHeader.code = 0;
      icmphHeader.checksum = 0;
      icmphHeader.un.echo.id = 0xb60b;
      icmphHeader.un.echo.sequence = 0x0100;
      
      icmphHeader.checksum = this->icmpHeaderChecksum((uint16_t*)&icmphHeader, sizeof(struct icmphdr));
      
      // struct timeval tvorig;
      // long tsorig;
      // long tsdiff;
      // gettimeofday(&tvorig, (struct timezone *)NULL);
      // tsorig = (tvorig.tv_sec % (24*60*60)) * 1000 + tvorig.tv_usec / 1000;
      
      // icmphHeader.icmp_otime = htonl(tsorig);
      
      int nLen = sizeof(struct iphdr) + sizeof(struct icmphdr) + 48;
      unsigned char carrBufferPre[nLen];
      memcpy(carrBufferPre, &iphHeader, sizeof(struct iphdr));
      memcpy(&(carrBufferPre[sizeof(struct iphdr)]), &icmphHeader, sizeof(struct icmphdr));
      
      for(int nI = sizeof(struct iphdr) + sizeof(struct icmphdr); nI < nLen; nI++) {
	carrBufferPre[nI] = 'Z';
      }
      
      int nLength = nLen + sizeof(struct ethhdr);
      unsigned char carrBuffer[nLength];
      
      int nL = m_wrWire.wrapInEthernetFrame(this->mac(), strDestinationMAC, ETH_P_IP, carrBufferPre, sizeof(struct icmphdr) + sizeof(struct iphdr) + 48, carrBuffer);
      
      return m_wrWire.write(carrBuffer, nL);
    }
    
    return false;
  }
  
  Wire* Device::wire() {
    return &m_wrWire;
  }
  
  uint16_t Device::icmpHeaderChecksum(uint16_t* buffer, uint32_t size) 
  {
    unsigned long cksum=0;
    while(size >1) 
      {
	cksum+=*buffer++;
	size -= sizeof(unsigned short);
      }
    if(size ) 
      {
	cksum += *(unsigned char*)buffer;
      }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (uint16_t)(~cksum);
  }
  
  uint16_t Device::ipHeaderChecksum(void* vdData, int nLength) {
    char* carrData = (char*)vdData;
    
    // Initialise the accumulator.
    uint32_t acc = 0xffff;
    
    // Handle complete 16-bit blocks.
    for(size_t i = 0; i + 1 < nLength; i += 2) {
      uint16_t word;
      memcpy(&word, carrData + i, 2);
      acc += ntohs(word);
      
      if(acc > 0xffff) {
	acc -= 0xffff;
      }
    }
    
    // Handle any partial block at the end of the data.
    if(nLength & 1) {
      uint16_t word = 0;
      memcpy(&word, carrData + nLength - 1, 1);
      acc += ntohs(word);
      
      if(acc > 0xffff) {
	acc -= 0xffff;
      }
    }
    
    // Return the checksum in network byte order.
    return htons(~acc);
  }
}
