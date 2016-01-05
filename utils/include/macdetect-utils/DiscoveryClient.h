#ifndef __DISCOVERY_CLIENT_H__
#define __DISCOVERY_CLIENT_H__


// macdetect
#include <macdetect-utils/DiscoveryNode.h>


namespace macdetect {
  class DiscoveryClient : public DiscoveryNode {
  public:
    typedef struct {
      std::string strName;
      std::string strIP;
    } ServerInfo;
    
  private:
    std::list<ServerInfo> m_lstDetectedServers;
    
  protected:
  public:
    DiscoveryClient();
    ~DiscoveryClient();
    
    void processReceivedValue(std::shared_ptr<Value> valReceived, std::string strSenderIP) override;
    void detectServers();
    
    std::list<ServerInfo> detectedServers();
  };
}


#endif /* __DISCOVERY_CLIENT_H__ */
