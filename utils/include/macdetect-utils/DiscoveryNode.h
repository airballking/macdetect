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
    
    Value receive(bool bSuccess);
    bool send(Value valSend);
    
    bool cycle();
  };
}


#endif /* __DISCOVERY_H__ */
