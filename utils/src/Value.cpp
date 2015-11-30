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


#include <macdetect-utils/Value.h>


namespace macdetect {
  Value::Value(std::string strKey, std::string strContent, std::list< std::pair<std::string, std::string> > lstSubValues) : m_strKey(strKey), m_strContent(strContent) {
    for(std::pair<std::string, std::string> prPair : lstSubValues) {
      this->add(new Value(prPair.first, prPair.second));
    }
  }
  
  Value::~Value() {
    for(Value* valDelete : m_lstSubValues) {
      delete valDelete;
    }
  }
  
  void Value::add(Value* valAdd) {
    m_lstSubValues.push_back(valAdd);
  }
  
  unsigned int Value::serialize(void* vdBuffer, unsigned int unLength) {
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
    unStringLength = m_strContent.length();
    memcpy(&(ucData[unOffset]), &unStringLength, sizeof(unsigned int));
    unOffset += sizeof(unsigned int);
    memcpy(&(ucData[unOffset]), m_strContent.c_str(), unStringLength);
    unOffset += unStringLength;
    
    // Add subpackets
    unsigned int unSubValueCount = m_lstSubValues.size();
    memcpy(&(ucData[unOffset]), &unSubValueCount, sizeof(unsigned int));
    unOffset += sizeof(unsigned int);
    
    for(Value* valSerialize : m_lstSubValues) {
      unOffset += valSerialize->serialize(&(ucData[unOffset]), unLength - unOffset);
    }
    
    return unOffset;
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
      Value* valSub = new Value();
      unOffset += valSub->deserialize(&(ucBuffer[unOffset]), unLength - unOffset);
      
      m_lstSubValues.push_back(valSub);
    }
    
    return unOffset;
  }
  
  std::list<Value*> Value::subValues() {
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
    
    for(Value* valSub : m_lstSubValues) {
      valSub->print(unIndent + 1);
    }
  }
  
  void Value::set(std::string strKey, std::string strContent) {
    m_strKey = strKey;
    m_strContent = strContent;
  }
  
  Value* Value::copy() {
    Value* valCopy = new Value(m_strKey, m_strContent);
    
    for(Value* valSub : m_lstSubValues) {
      valCopy->add(valSub->copy());
    }
    
    return valCopy;
  }
  
  Value* Value::sub(std::string strSubKey) {
    Value* valReturn = NULL;
    
    for(Value* valSub : m_lstSubValues) {
      if(valSub->key() == strSubKey) {
	valReturn = valSub;
	
	break;
      }
    }
    
    return valReturn;
  }
}
