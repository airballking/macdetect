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


#ifndef __MDCLIENT_H__
#define __MDCLIENT_H__


// System
#include <list>
#include <string>

// MAC detect
#include <macdetect-client/Client.h>
#include <macdetect-utils/DiscoveryClient.h>


namespace macdetect_client {
  class MDClient {
  private:
    macdetect::Client m_cliClient;
    macdetect::DiscoveryClient m_dcClient;
    
  protected:
  public:
    MDClient();
    ~MDClient();
    
    bool connect(std::string strIP);
    bool connected();
    bool disconnect();
    
    bool requestKnownMACAddresses();
    bool requestDeviceNames();
    
    bool requestEnableStream(std::string strDeviceName);
    bool requestDisableStream(std::string strDeviceName);
    
    std::shared_ptr<macdetect::Value> receive(bool& bDisconnected, bool bBlock = false);
    
    bool send(std::shared_ptr<macdetect::Value> valSend);
    
    void detectServers();
    
    void cycle();
    
    std::list<macdetect::DiscoveryClient::ServerInfo> detectedServers();
  };
}


#endif /* __MDCLIENT_H__ */
