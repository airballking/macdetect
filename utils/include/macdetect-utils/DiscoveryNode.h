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
    
    std::shared_ptr<Value> receive(bool bSuccess);
    
    bool cycle();
    
    virtual void processReceivedValue(std::shared_ptr<Value> valReceived) = 0;
  };
}


#endif /* __DISCOVERY_H__ */
