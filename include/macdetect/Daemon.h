#ifndef __DAEMON_H__
#define __DAEMON_H__


// System

// MAC detect
#include <macdetect/Network.h>
#include <macdetect/Server.h>


namespace macdetect {
  class Daemon {
  private:
    Server m_srvServer;
    Network m_nwNetwork;
    
  protected:
  public:
    Daemon();
    ~Daemon();
    
    void shutdown();
    bool cycle();
  };
}


#endif /* __DAEMON_H__ */
