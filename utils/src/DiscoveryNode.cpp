#include <macdetect-utils/DiscoveryNode.h>


namespace macdetect {
  DiscoveryNode::DiscoveryNode() : PacketEntity(-1), m_meMulticast("225.0.0.37", 2077) {
    this->setSocket(m_meMulticast.socket());
  }
  
  DiscoveryNode::~DiscoveryNode() {
  }
  
  std::shared_ptr<Value> DiscoveryNode::receive(bool bSuccess) {
    std::shared_ptr<Value> valReceived;
    bSuccess = true;
    
    bool bDisconnected;
    std::shared_ptr<Value> valReceivedTemp = this->PacketEntity::receive(bDisconnected);
    
    if(valReceivedTemp != NULL) {
      if(!bDisconnected) {
	bSuccess = true;
	valReceived = valReceivedTemp;
      }
    }
    
    return valReceived;
  }
  
  bool DiscoveryNode::cycle() {
    bool bSuccess;
    std::shared_ptr<Value> valReceived = this->receive(bSuccess);
    
    if(bSuccess) {
      this->processReceivedValue(valReceived);
    }
    
    return true;
  }
}
