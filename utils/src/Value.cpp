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


#include <macdetect-utils/Value.h>


namespace macdetect {
  Value::Value(std::string strKey, std::string strContent, std::list< std::pair<std::string, std::string> > lstSubValues) : m_strKey(strKey), m_strContent(strContent) {
    for(std::pair<std::string, std::string> prPair : lstSubValues) {
      this->add(std::make_shared<Value>(prPair.first, prPair.second));
    }
  }
  
  Value::~Value() {
  }
  
  void Value::add(std::shared_ptr<Value> valAdd) {
    m_lstSubValues.push_back(valAdd);
  }
  
  void Value::add(std::string strKey, std::string strContent) {
    this->add(std::make_shared<Value>(strKey, strContent));
  }
  
  int Value::serialize(void* vdBuffer, unsigned int unLength) {
    int nOffset = 0;
    unsigned char* ucData = (unsigned char*)vdBuffer;
    
    unsigned int unStringLength;
    
    // Add key
    unStringLength = m_strKey.length();
    if(nOffset + unStringLength + sizeof(unsigned int) > unLength) {
      return -1;
    } else {
      memcpy(&(ucData[nOffset]), &unStringLength, sizeof(unsigned int));
      nOffset += sizeof(unsigned int);
      memcpy(&(ucData[nOffset]), m_strKey.c_str(), unStringLength);
      nOffset += unStringLength;
      
      // Add value
      unStringLength = m_strContent.length();
      if(nOffset + unStringLength + sizeof(unsigned int) > unLength) {
	return -1;
      } else {
	memcpy(&(ucData[nOffset]), &unStringLength, sizeof(unsigned int));
	nOffset += sizeof(unsigned int);
	memcpy(&(ucData[nOffset]), m_strContent.c_str(), unStringLength);
	nOffset += unStringLength;
	
	// Add subpackets
	if(nOffset + sizeof(unsigned int) > unLength) {
	  return -1;
	} else {
	  unsigned int unSubValueCount = m_lstSubValues.size();
	  memcpy(&(ucData[nOffset]), &unSubValueCount, sizeof(unsigned int));
	  nOffset += sizeof(unsigned int);
	  
	  for(std::shared_ptr<Value> valSerialize : m_lstSubValues) {
	    unsigned int nBytes = valSerialize->serialize(&(ucData[nOffset]), unLength - nOffset);
	    
	    if(nBytes == -1) {
	      return -1;
	    } else {
	      nOffset += nBytes;
	    }
	  }
	  
	  return nOffset;
	}
      }
    }
  }
  
  unsigned int Value::deserialize(void* vdBuffer, unsigned int unLength) {
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
    m_strContent = std::string((char*)ucValue, unStringLength);
    unOffset += unStringLength;
    
    // Get subpackets
    unsigned int unSubValueCount;
    memcpy(&unSubValueCount, &(ucBuffer[unOffset]), sizeof(unsigned int));
    unOffset += sizeof(unsigned int);
    
    for(unsigned int unI = 0; unI < unSubValueCount; unI++) {
      std::shared_ptr<Value> valSub = std::make_shared<Value>();
      unOffset += valSub->deserialize(&(ucBuffer[unOffset]), unLength - unOffset);
      
      m_lstSubValues.push_back(valSub);
    }
    
    return unOffset;
  }
  
  std::list< std::shared_ptr<Value> > Value::subValues() {
    return m_lstSubValues;
  }
  
  std::string Value::key() {
    return m_strKey;
  }
  
  std::string Value::content() {
    return m_strContent;
  }
  
  void Value::print(unsigned int unIndent) {
    std::string strIndentation = "";
    for(unsigned int unI = 0; unI < unIndent; unI++) {
      strIndentation += "  ";
    }
    
    std::cout << strIndentation << " - " << m_strKey << " = " << m_strContent << std::endl;
    
    for(std::shared_ptr<Value> valSub : m_lstSubValues) {
      valSub->print(unIndent + 1);
    }
  }
  
  void Value::setKey(std::string strKey) {
    m_strKey = strKey;
  }
  
  void Value::setContent(std::string strContent) {
    m_strContent = strContent;
  }
  
  void Value::set(std::string strKey, std::string strContent) {
    this->setKey(strKey);
    this->setContent(strContent);
  }
  
  std::shared_ptr<Value> Value::copy() {
    std::shared_ptr<Value> valCopy(new Value(m_strKey, m_strContent));
    
    for(std::shared_ptr<Value> valSub : m_lstSubValues) {
      valCopy->add(valSub->copy());
    }
    
    return valCopy;
  }
  
  std::shared_ptr<Value> Value::sub(std::string strSubKey) {
    std::shared_ptr<Value> valReturn = NULL;
    
    for(std::shared_ptr<Value> valSub : m_lstSubValues) {
      if(valSub->key() == strSubKey) {
	valReturn = valSub;
	
	break;
      }
    }
    
    return valReturn;
  }
}
