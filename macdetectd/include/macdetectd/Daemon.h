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
