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


#ifndef __PACKET_H__
#define __PACKET_H__


// System
#include <list>
#include <string>
#include <string.h>
#include <iostream>


namespace macdetect {
  class Packet {
  public:
  private:
    std::list<Packet*> m_lstSubPackets;
    
    std::string m_strKey;
    std::string m_strValue;
    
  protected:
  public:
    Packet(std::string strKey = "", std::string strValue = "", std::list< std::pair<std::string, std::string> > lstSubPackets = {});
    ~Packet();
    
    void add(Packet* pktAdd);
    
    unsigned int serialize(void* vdBuffer, unsigned int unLength);
    unsigned int deserialize(void* vdBuffer, unsigned int unLength);
    
    void set(std::string strKey, std::string strValue);
    
    std::list<Packet*> subPackets();
    std::string key();
    std::string value();
    
    void print(unsigned int unIndent = 0);
    
    Packet* copy();
    
    Packet* sub(std::string strSubKey);
  };
}


#endif /* __PACKET_H__ */
