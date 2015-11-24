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
#include <fcntl.h>
#include <iostream>

// MAC Detect
#include <macdetect/Served.h>


namespace macdetect {
  class Server {
  public:
    typedef struct {
      typedef enum {
	Invalid = 0,
	Started = 1,
	Stopped = 2
      } Status;
      
      Status stStatus;
      int nID;
      int nSocketFD;
      std::string strDeviceName;
      unsigned short usPort;
    } Serving;
    
  private:
    int m_nSocketFDControl;
    bool m_bRun;
    std::list<Serving> m_lstServings;
    std::list< std::pair<Served*, int> > m_lstServed;
    
  protected:
  public:
    Server();
    ~Server();
    
    bool serve(std::string strDeviceName, unsigned short usPort);
    
    bool cycle();
    void shutdown();
    
    std::list<std::string> systemDeviceNames();
    
    int freeServingID();
    Server::Serving servingByID(int nID);
  };
}


#endif /* __SERVER_H__ */
