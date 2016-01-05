#ifndef __DISCOVERY_CLIENT_H__
#define __DISCOVERY_CLIENT_H__


// macdetect
#include <macdetect-utils/DiscoveryNode.h>


namespace macdetect {
  class DiscoveryClient : public DiscoveryNode {
  private:
  protected:
  public:
    DiscoveryClient();
    ~DiscoveryClient();
    
    void processReceivedValue(std::shared_ptr<Value> valReceived) override;
    void detectServers();
  };
}


#endif /* __DISCOVERY_CLIENT_H__ */
