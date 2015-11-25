#ifndef __MDCLIENT_H__
#define __MDCLIENT_H__


// System
#include <list>
#include <string>

// MAC detect
#include <macdetect/Client.h>


namespace macdetect_client {
  class MDClient {
  private:
    std::list<macdetect::Packet*> m_lstReceivedPackets;
    macdetect::Client m_cliClient;
    
  protected:
    macdetect::Packet* getPacket(std::string strKey, std::string strValue, bool bBlock = true);
    
  public:
    MDClient();
    ~MDClient();
    
    bool connect(std::string strIP);
    
    macdetect::Packet* requestResponse(std::string strRequest);
    macdetect::Packet* requestResponse(macdetect::Packet* pktRequest, std::string strKey = "response");
    
    std::list<std::string> deviceNames();
    bool enableStream(std::string strDeviceName);
    bool disableStream(std::string strDeviceName);
    
    macdetect::Packet* info();
  };
}


#endif /* __MDCLIENT_H__ */
