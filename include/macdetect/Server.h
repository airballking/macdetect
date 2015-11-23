#ifndef __SERVER_H__
#define __SERVER_H__


// System
#include <list>
#include <string>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

// MAC Detect
#include <macdetect/Served.h>


namespace macdetect {
  class Server {
  private:
    typedef struct {
      int nSocketFD;
      std::string strDeviceName;
      unsigned short usPort;
    } Serving;
    
    int m_nSocketFDControl;
    bool m_bRun;
    std::list<Serving> m_lstServings;
    std::list<Served*> m_lstServed;
    
  protected:
  public:
    Server();
    ~Server();
    
    bool serve(std::string strDeviceName, unsigned short usPort);
    
    bool cycle();
    void shutdown();
    
    std::list<std::string> systemDeviceNames();
  };
}


#endif /* __SERVER_H__ */
