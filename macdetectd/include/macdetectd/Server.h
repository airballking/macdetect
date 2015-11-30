// Copyright 2016 Jan Winkler <jan.winkler.84@gmail.com>
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

// MAC detect
#include <macdetectd/Served.h>


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
    std::list< std::pair< std::shared_ptr<Served>, int> > m_lstServed;
    
    std::list<QueuedValue> m_lstValueQueue;
    std::list< std::shared_ptr<Served> > m_lstRemoved;
    
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
    
    std::list< std::pair< std::shared_ptr<Served>, int> > served();
    void distributeStreamValue(std::shared_ptr<Value> valStream);
    
    std::list< std::shared_ptr<Served> > removed();
  };
}


#endif /* __SERVER_H__ */
