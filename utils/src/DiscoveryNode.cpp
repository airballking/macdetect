#include <macdetect-utils/DiscoveryNode.h>


namespace macdetect {
  DiscoveryNode::DiscoveryNode() : PacketEntity(-1), m_meMulticast("225.0.0.37", 2077) {
    this->setSocket(m_meMulticast.socket());
  }
  
  DiscoveryNode::~DiscoveryNode() {
  }
  
  Value DiscoveryNode::receive(bool bSuccess) {
    Value valReceived;
    bSuccess = false;
    
    // TODO(winkler): Implement this.
    
    return valReceived;
  }
  
  bool DiscoveryNode::send(Value valSend) {
    // TODO(winkler): Implement this.
    
    return true;
  }
  
  bool DiscoveryNode::cycle() {
    // TODO(winkler): Implement this.
    
    return true;
  }
}
