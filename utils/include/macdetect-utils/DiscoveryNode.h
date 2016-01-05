#ifndef __DISCOVERY_NODE_H__
#define __DISCOVERY_NODE_H__


// System
#include <string>

// macdetect
#include <macdetect-utils/Value.h>
#include <macdetect-utils/MulticastEndpoint.h>
#include <macdetect-utils/PacketEntity.h>


namespace macdetect {
  class DiscoveryNode : public PacketEntity {
  private:
    MulticastEndpoint m_meMulticast;
    
  protected:
  public:
    DiscoveryNode();
    ~DiscoveryNode();
    
    int recv(int nSocketFD, void* vdBuffer, int nLength, int nFlags, std::string& strIP) override;
    std::shared_ptr<Value> receive(bool& bSuccess, std::string& strIP);
    
    bool cycle();
    
    virtual void processReceivedValue(std::shared_ptr<Value> valReceived, std::string strSenderIP) = 0;
    int write(int nSocketFD, void* vdBuffer, unsigned int unLength) override;
  };
}


#endif /* __DISCOVERY_H__ */
