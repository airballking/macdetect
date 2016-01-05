#include <macdetect-utils/DiscoveryNode.h>


// System
#include <string>


namespace macdetect {
  class DiscoveryServer : public DiscoveryNode {
  private:
    std::string m_strIdentifier;
    
  protected:
  public:
    DiscoveryServer(std::string strIdentifier);
    ~DiscoveryServer();
  };
}
