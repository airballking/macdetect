#ifndef __DAEMON_H__
#define __DAEMON_H__


// System
#include <algorithm>

// MAC detect
#include <macdetectd/Network.h>
#include <macdetectd/Server.h>


namespace macdetect {
  class Daemon {
  private:
    typedef struct {
      Served* svrServed;
      std::string strDeviceName;
    } Stream;
    
    Server m_srvServer;
    Network m_nwNetwork;
    
    std::list<Stream> m_lstStreams;
    
  protected:
  public:
    Daemon();
    ~Daemon();
    
    Packet* responsePacket(Packet* pktPacket, std::list< std::pair<std::string, std::string> > lstSubPackets = {});
    
    bool enableStream(Served* svrServed, std::string strDeviceName);
    bool disableStream(Served* svrServed, std::string strDeviceName);
    bool streamEnabled(Served* svrServed, std::string strDeviceName);
    
    void shutdown();
    bool cycle();
  };
}


#endif /* __DAEMON_H__ */
