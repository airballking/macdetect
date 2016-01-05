#include <macdetect-utils/DiscoveryServer.h>


namespace macdetect {
  DiscoveryServer::DiscoveryServer(std::string strIdentifier) : m_strIdentifier(strIdentifier) {
  }
  
  DiscoveryServer::~DiscoveryServer() {
  }
  
  void DiscoveryServer::processReceivedValue(std::shared_ptr<Value> valReceived) {
    if(valReceived->key() == "request" && valReceived->content() == "server-info") {
      std::shared_ptr<Value> valResponse = std::make_shared<Value>("server-info");
      valResponse->add("server-name", m_strIdentifier);
      
      this->send(valResponse);
    }
  }
}
