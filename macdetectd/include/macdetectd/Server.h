// macdetect, a daemon and clients for detecting MAC addresses
// Copyright (C) 2015 Jan Winkler <jan.winkler.84@gmail.com>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

/** \author Jan Winkler */


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
#include <syslog.h>
#include <sstream>

// MAC detect
#include <macdetectd/Served.h>
#include <macdetectd/Globals.h>


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
    
    typedef struct {
      std::shared_ptr<Value> valValue;
      std::shared_ptr<Served> svrServed;
      int nServingID;
    } QueuedValue;
    
  private:
    int m_nSocketFDControl;
    bool m_bRun;
    std::list<Serving> m_lstServings;
    std::list<std::pair<std::shared_ptr<Served>, int>> m_lstServed;
    
    std::list<QueuedValue> m_lstValueQueue;
    std::list<std::shared_ptr<Served>> m_lstRemoved;
    
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
    
    void handleValue(std::shared_ptr<Served> svrServed, int nServingID, std::shared_ptr<Value> valReceived);
    std::list<QueuedValue> queuedValues();
    
    std::list<std::pair<std::shared_ptr<Served>, int>> served();
    void distributeStreamValue(std::shared_ptr<Value> valStream);
    
    std::list<std::shared_ptr<Served>> removed();
  };
}


#endif /* __SERVER_H__ */
