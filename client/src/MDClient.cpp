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


#include <macdetect-client/MDClient.h>


namespace macdetect_client {
  MDClient::MDClient() {
  }
  
  MDClient::~MDClient() {
  }
  
  std::shared_ptr<macdetect::Value> MDClient::receive(bool& bDisconnected, bool bBlock) {
    std::shared_ptr<macdetect::Value> valReturn = NULL;
    bool bReceived = false;
    bDisconnected = false;
    
    while(!bReceived && !bDisconnected) {
      valReturn = m_cliClient.receive(bDisconnected);
      
      if(bDisconnected == false) {
	if(valReturn) {
	  bReceived = true;
	} else {
	  if(!bBlock) {
	    break;
	  }
	}
      }
    }
    
    return valReturn;
  }
  
  bool MDClient::connect(std::string strIP) {
    return m_cliClient.connect(strIP);
  }
  
  bool MDClient::connected() {
    return m_cliClient.connected();
  }
  
  bool MDClient::disconnect() {
    return m_cliClient.disconnect();
  }
  
  bool MDClient::requestDeviceNames() {
    return this->send(std::make_shared<macdetect::Value>("request", "devices-list"));
  }
  
  bool MDClient::requestKnownMACAddresses() {
    return this->send(std::make_shared<macdetect::Value>("request", "known-mac-addresses"));
  }
  
  bool MDClient::requestEnableStream(std::string strDeviceName) {
    std::shared_ptr<macdetect::Value> valRequest = std::make_shared<macdetect::Value>("request", "enable-stream");
    valRequest->add(std::make_shared<macdetect::Value>("device-name", strDeviceName));
    
    return this->send(valRequest);
  }
  
  bool MDClient::requestDisableStream(std::string strDeviceName) {
    std::shared_ptr<macdetect::Value> valRequest = std::make_shared<macdetect::Value>("request", "disable-stream");
    valRequest->add(std::make_shared<macdetect::Value>("device-name", strDeviceName));
    
    return this->send(valRequest);
  }
  
  bool MDClient::send(std::shared_ptr<macdetect::Value> valSend) {
    return m_cliClient.send(valSend);
  }
  
  void MDClient::detectServers() {
    m_dcClient.detectServers();
  }
  
  void MDClient::cycle() {
    m_dcClient.cycle();
  }
  
  std::list<macdetect::DiscoveryClient::ServerInfo> MDClient::detectedServers() {
    return m_dcClient.detectedServers();
  }
}
