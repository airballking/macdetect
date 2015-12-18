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


#ifndef __DAEMON_H__
#define __DAEMON_H__


// System
#include <algorithm>

// MAC detect
#include <macdetectd/Network.h>
#include <macdetectd/Server.h>
#include <macdetect-utils/Config.h>


namespace macdetect {
  class Daemon {
  private:
    typedef struct {
      std::shared_ptr<Served> svrServed;
      std::string strDeviceName;
    } Stream;
    
    Server m_srvServer;
    Network m_nwNetwork;
    
    std::list<Stream> m_lstStreams;
    
    double m_dLastKeepAlive;
    double m_dKeepAliveInterval;
    
  protected:
  public:
    Daemon();
    ~Daemon();
    
    bool parseConfigFile(std::string strFilepath);
    
    std::shared_ptr<Value> response(std::shared_ptr<Value> valValue, std::list< std::pair<std::string, std::string> > lstSubValues = {});
    
    bool enableStream(std::shared_ptr<Served> svrServed, std::string strDeviceName);
    bool disableStream(std::shared_ptr<Served> svrServed, std::string strDeviceName);
    bool streamEnabled(std::shared_ptr<Served> svrServed, std::string strDeviceName);
    
    void shutdown();
    bool cycle();
    
    bool privilegesSuffice();
    
    bool serve(std::string strDeviceName, unsigned short usPort);
  };
}


#endif /* __DAEMON_H__ */
