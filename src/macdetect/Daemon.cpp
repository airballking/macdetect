#include <macdetect/Daemon.h>


namespace macdetect {
  Daemon::Daemon() {
  }
  
  Daemon::~Daemon() {
  }
  
  bool Daemon::cycle() {
    bool bSuccess = false;
    
    if(m_nwNetwork.cycle() && m_srvServer.cycle()) {
      std::list<Server::QueuedPacket> lstPackets = m_srvServer.queuedPackets();
      
      for(Server::QueuedPacket qpPacket : lstPackets) {
	// ...
      }
      
      bSuccess = true;
    }
    
    return bSuccess;
  }
  
  void Daemon::shutdown() {
    m_srvServer.shutdown();
    m_nwNetwork.shutdown();
  }
}
