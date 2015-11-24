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
    Packet(std::string strKey = "", std::string strValue = "");
    ~Packet();
    
    void add(Packet* pktAdd);
    
    unsigned int serialize(void* vdBuffer, unsigned int unLength);
    unsigned int deserialize(void* vdBuffer, unsigned int unLength);
    
    void set(std::string strKey, std::string strValue);
    
    std::list<Packet*> subPackets();
    std::string key();
    std::string value();
    
    void print(unsigned int unIndent = 0);
  };
}


#endif /* __PACKET_H__ */
