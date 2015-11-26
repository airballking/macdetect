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


#include <macdetect-utils/Packet.h>


namespace macdetect {
  Packet::Packet(std::string strKey, std::string strValue, std::list< std::pair<std::string, std::string> > lstSubPackets) : m_strKey(strKey), m_strValue(strValue) {
    for(std::pair<std::string, std::string> prPair : lstSubPackets) {
      this->add(new Packet(prPair.first, prPair.second));
    }
  }
  
  Packet::~Packet() {
    for(Packet* pktDelete : m_lstSubPackets) {
      delete pktDelete;
    }
  }
  
  void Packet::add(Packet* pktAdd) {
    m_lstSubPackets.push_back(pktAdd);
  }
  
  unsigned int Packet::serialize(void* vdBuffer, unsigned int unLength) {
    unsigned int unOffset = 0;
    unsigned char* ucData = (unsigned char*)vdBuffer;
    
    unsigned int unStringLength;
    
    // Add key
    unStringLength = m_strKey.length();
    memcpy(&(ucData[unOffset]), &unStringLength, sizeof(unsigned int));
    unOffset += sizeof(unsigned int);
    memcpy(&(ucData[unOffset]), m_strKey.c_str(), unStringLength);
    unOffset += unStringLength;
    
    // Add value
    unStringLength = m_strValue.length();
    memcpy(&(ucData[unOffset]), &unStringLength, sizeof(unsigned int));
    unOffset += sizeof(unsigned int);
    memcpy(&(ucData[unOffset]), m_strValue.c_str(), unStringLength);
    unOffset += unStringLength;
    
    // Add subpackets
    unsigned int unSubPacketCount = m_lstSubPackets.size();
    memcpy(&(ucData[unOffset]), &unSubPacketCount, sizeof(unsigned int));
    unOffset += sizeof(unsigned int);
    
    for(Packet* pktSerialize : m_lstSubPackets) {
      unOffset += pktSerialize->serialize(&(ucData[unOffset]), unLength - unOffset);
    }
    
    return unOffset;
  }
  
  unsigned int Packet::deserialize(void* vdBuffer, unsigned int unLength) {
    unsigned int unOffset = 0;
    unsigned int unStringLength;
    unsigned char* ucBuffer = (unsigned char*)vdBuffer;
    
    // Get key
    memcpy(&unStringLength, &(ucBuffer[unOffset]), sizeof(unsigned int));
    unOffset += sizeof(unsigned int);
    unsigned char ucKey[unStringLength];
    memcpy(ucKey, &(ucBuffer[unOffset]), unStringLength);
    m_strKey = std::string((char*)ucKey, unStringLength);
    unOffset += unStringLength;
    
    // Get value
    memcpy(&unStringLength, &(ucBuffer[unOffset]), sizeof(unsigned int));
    unOffset += sizeof(unsigned int);
    unsigned char ucValue[unStringLength];
    memcpy(ucValue, &(ucBuffer[unOffset]), unStringLength);
    m_strValue = std::string((char*)ucValue, unStringLength);
    unOffset += unStringLength;
    
    // Get subpackets
    unsigned int unSubPacketCount;
    memcpy(&unSubPacketCount, &(ucBuffer[unOffset]), sizeof(unsigned int));
    unOffset += sizeof(unsigned int);
    
    for(unsigned int unI = 0; unI < unSubPacketCount; unI++) {
      Packet* pktSub = new Packet();
      unOffset += pktSub->deserialize(&(ucBuffer[unOffset]), unLength - unOffset);
      
      m_lstSubPackets.push_back(pktSub);
    }
    
    return unOffset;
  }
  
  std::list<Packet*> Packet::subPackets() {
    return m_lstSubPackets;
  }
  
  std::string Packet::key() {
    return m_strKey;
  }
  
  std::string Packet::value() {
    return m_strValue;
  }
  
  void Packet::print(unsigned int unIndent) {
    std::string strIndentation = "";
    for(unsigned int unI = 0; unI < unIndent; unI++) {
      strIndentation += "  ";
    }
    
    std::cout << strIndentation << " - " << m_strKey << " = " << m_strValue << std::endl;
    
    for(Packet* pktSub : m_lstSubPackets) {
      pktSub->print(unIndent + 1);
    }
  }
  
  void Packet::set(std::string strKey, std::string strValue) {
    m_strKey = strKey;
    m_strValue = strValue;
  }
  
  Packet* Packet::copy() {
    Packet* pktCopy = new Packet(m_strKey, m_strValue);
    
    for(Packet* pktSub : m_lstSubPackets) {
      pktCopy->add(pktSub->copy());
    }
    
    return pktCopy;
  }
  
  Packet* Packet::sub(std::string strSubKey) {
    Packet* pktReturn = NULL;
    
    for(Packet* pktSub : m_lstSubPackets) {
      if(pktSub->key() == strSubKey) {
	pktReturn = pktSub;
	
	break;
      }
    }
    
    return pktReturn;
  }
}
