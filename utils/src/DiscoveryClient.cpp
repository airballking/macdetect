#include <macdetect-utils/DiscoveryClient.h>


namespace macdetect {
  DiscoveryClient::DiscoveryClient() {
  }
  
  DiscoveryClient::~DiscoveryClient() {
  }
  
  void DiscoveryClient::processReceivedValue(std::shared_ptr<Value> valReceived, std::string strSenderIP) {
    if(valReceived) {
      if(valReceived->key() == "response" && valReceived->content() == "server-info") {
	if(valReceived->sub("server-name")) {
	  std::string strName = valReceived->sub("server-name")->content();
	  
	  m_lstDetectedServers.push_back({strName, strSenderIP});
	}
      }
    }
  }
  
  void DiscoveryClient::detectServers() {
    std::shared_ptr<Value> valResponse = std::make_shared<Value>("request", "server-info");
    
    this->send(valResponse);
  }
  
  std::list<DiscoveryClient::ServerInfo> DiscoveryClient::detectedServers() {
    std::list<ServerInfo> lstReturn = m_lstDetectedServers;
    m_lstDetectedServers.clear();
    
    return lstReturn;
  }
}
