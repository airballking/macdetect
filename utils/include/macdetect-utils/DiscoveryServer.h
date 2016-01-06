#ifndef __DISCOVERY_SERVER_H__
#define __DISCOVERY_SERVER_H__


// System
#include <string>

// macdetect
#include <macdetect-utils/DiscoveryNode.h>


namespace macdetect {
  class DiscoveryServer : public DiscoveryNode {
  private:
    std::string m_strIdentifier;
    
  protected:
  public:
    DiscoveryServer(std::string strIdentifier);
    ~DiscoveryServer();
    
    void processReceivedValue(std::shared_ptr<Value> valReceived, std::string strSenderIP) override;
    
    void setIdentifier(std::string strIdentifier);
  };
}

#endif /* __DISCOVERY_SERVER_H__ */
