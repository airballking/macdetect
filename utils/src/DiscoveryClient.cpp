#include <macdetect-utils/DiscoveryClient.h>


namespace macdetect {
  DiscoveryClient::DiscoveryClient() {
  }
  
  DiscoveryClient::~DiscoveryClient() {
  }
  
  void DiscoveryClient::processReceivedValue(std::shared_ptr<Value> valReceived) {
    if(valReceived->content() == "server-info") {
      valReceived->print();
    }
  }
  
  void DiscoveryClient::detectServers() {
    std::shared_ptr<Value> valResponse = std::make_shared<Value>("request", "server-info");
    
    this->send(valResponse);
  }
}
