#include <macdetect-utils/DiscoveryServer.h>


namespace macdetect {
  DiscoveryServer::DiscoveryServer(std::string strIdentifier) : m_strIdentifier(strIdentifier) {
  }
  
  DiscoveryServer::~DiscoveryServer() {
  }
  
  void DiscoveryServer::processReceivedValue(std::shared_ptr<Value> valReceived, std::string strSenderIP) {
    if(valReceived->key() == "request" && valReceived->content() == "server-info") {
      std::shared_ptr<Value> valResponse = std::make_shared<Value>("response", "server-info");
      valResponse->add("server-name", m_strIdentifier);
      valResponse->print();
      // TODO(winkler): Set the received sender IP as new receiver of
      // this information
      this->send(valResponse);
    }
  }
}
