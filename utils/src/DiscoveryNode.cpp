#include <macdetect-utils/DiscoveryNode.h>


namespace macdetect {
  DiscoveryNode::DiscoveryNode() : PacketEntity(-1), m_meMulticast("224.0.0.1", 2077) {
    this->setSocket(m_meMulticast.socket());
    this->socket();
  }
  
  DiscoveryNode::~DiscoveryNode() {
  }
  
  int DiscoveryNode::recv(int nSocketFD, void* vdBuffer, int nLength, int nFlags, std::string& strIP) {
    return m_meMulticast.read((unsigned char*)vdBuffer, nLength, nFlags, strIP);
  }
  
  std::shared_ptr<Value> DiscoveryNode::receive(bool& bSuccess, std::string& strIP) {
    std::shared_ptr<Value> valReceived = NULL;
    bSuccess = false;
    
    bool bDisconnected = false;
    std::shared_ptr<Value> valReceivedTemp = this->PacketEntity::receive(bDisconnected, strIP);
    
    if(valReceivedTemp != NULL) {
      if(!bDisconnected) {
	bSuccess = true;
	valReceived = valReceivedTemp;
      }
    }
    
    return valReceived;
  }
  
  bool DiscoveryNode::cycle() {
    bool bSuccess = false;
    std::string strIP = "";
    std::shared_ptr<Value> valReceived = this->receive(bSuccess, strIP);
    
    if(bSuccess) {
      this->processReceivedValue(valReceived, strIP);
    }
    
    return true;
  }
  
  int DiscoveryNode::write(int nSocketFD, void* vdBuffer, unsigned int unLength) {
    return m_meMulticast.write(vdBuffer, unLength);
  }
}
