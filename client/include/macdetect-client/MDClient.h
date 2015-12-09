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


namespace macdetect_client {
  class MDClient {
  private:
    std::list< std::shared_ptr<macdetect::Value> > m_lstReceivedValues;
    macdetect::Client m_cliClient;
    
  protected:
    std::shared_ptr<macdetect::Value> get(std::string strKey, std::string strContent, bool bBlock = true);
    
  public:
    MDClient();
    ~MDClient();
    
    bool connect(std::string strIP);
    bool disconnect();
    
    std::shared_ptr<macdetect::Value> requestResponse(std::string strRequest);
    std::shared_ptr<macdetect::Value> requestResponse(std::shared_ptr<macdetect::Value> valRequest, std::string strKey = "response");
    
    std::list<std::string> knownMACAddresses();
    std::list<std::string> deviceNames();
    
    std::list< std::shared_ptr<macdetect::Value> > devicesList();
    
    bool enableStream(std::string strDeviceName);
    bool disableStream(std::string strDeviceName);
    
    std::shared_ptr<macdetect::Value> receive();
  };
}


#endif /* __MDCLIENT_H__ */
