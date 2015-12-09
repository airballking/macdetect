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
  
  std::shared_ptr<macdetect::Value> MDClient::get(std::string strKey, std::string strValue, bool bBlock) {
    std::shared_ptr<macdetect::Value> valReturn = NULL;
    bool bReceived = false;
    
    while(!bReceived) {
      while(m_lstReceivedValues.size() == 0) {
	std::shared_ptr<macdetect::Value> valReceived = m_cliClient.receive();
	
	if(valReceived) {
	  m_lstReceivedValues.push_back(valReceived);
	}
      }
      
      for(std::list<std::shared_ptr<macdetect::Value>>::iterator itValue = m_lstReceivedValues.begin(); itValue != m_lstReceivedValues.end(); itValue++) {
	std::shared_ptr<macdetect::Value> valValue = *itValue;
	
	if((strKey == "" || valValue->key() == strKey) &&
	   (strValue == "" || valValue->content() == strValue)) {
	  valReturn = valValue;
	  bReceived = true;
	  m_lstReceivedValues.erase(itValue);
	  
	  break;
	}
      }
      
      if(valReturn == NULL) {
	if(!bBlock) {
	  break;
	}
      }
    }
    
    return valReturn;
  }
  
  bool MDClient::connect(std::string strIP) {
    return m_cliClient.connect(strIP);
  }
  
  bool MDClient::disconnect() {
    return m_cliClient.disconnect();
  }
  
  std::shared_ptr<macdetect::Value> MDClient::requestResponse(std::string strRequest) {
    std::shared_ptr<macdetect::Value> pktRequest = std::make_shared<macdetect::Value>("request", strRequest);
    std::shared_ptr<macdetect::Value> pktResponse = this->requestResponse(pktRequest);
    
    return pktResponse;
  }
  
  std::shared_ptr<macdetect::Value> MDClient::requestResponse(std::shared_ptr<macdetect::Value> pktRequest, std::string strKey) {
    m_cliClient.send(pktRequest);
    
    return this->get(strKey, pktRequest->content());
  }
  
  std::list<std::string> MDClient::deviceNames() {
    std::list<std::string> lstDeviceNames;
    std::shared_ptr<macdetect::Value> pktDevices = this->requestResponse("devices-list");
    
    for(std::shared_ptr<macdetect::Value> pktSub : pktDevices->subValues()) {
      lstDeviceNames.push_back(pktSub->content());
    }
    
    return lstDeviceNames;
  }
  
  std::list<std::shared_ptr<macdetect::Value>> MDClient::devicesList() {
    std::list<std::shared_ptr<macdetect::Value>> lstDevices;
    std::shared_ptr<macdetect::Value> pktResponse = this->requestResponse("devices-list");
    
    for(std::shared_ptr<macdetect::Value> pktDevice : pktResponse->subValues()) {
      if(pktDevice->key() == "device") {
	lstDevices.push_back(pktDevice->copy());
      }
    }
    
    return lstDevices;
  }
  
  std::list<std::string> MDClient::knownMACAddresses() {
    std::list<std::string> lstMACAddresses;
    std::shared_ptr<macdetect::Value> pktMACAddresses = this->requestResponse("known-mac-addresses");
    
    for(std::shared_ptr<macdetect::Value> pktSub : pktMACAddresses->subValues()) {
      lstMACAddresses.push_back(pktSub->content());
    }
    
    return lstMACAddresses;
  }
  
  bool MDClient::enableStream(std::string strDeviceName) {
    bool bSuccess = false;
    
    std::shared_ptr<macdetect::Value> pktRequest = std::make_shared<macdetect::Value>("request", "enable-stream");
    pktRequest->add(std::make_shared<macdetect::Value>("device-name", strDeviceName));
    
    std::shared_ptr<macdetect::Value> pktResult = this->requestResponse(pktRequest);
    
    if(pktResult->sub("result")) {
      if(pktResult->sub("result")->content() == "success") {
	bSuccess = true;
      }
    }
    
    return bSuccess;
  }
  
  bool MDClient::disableStream(std::string strDeviceName) {
    bool bSuccess = false;
    
    std::shared_ptr<macdetect::Value> pktRequest = std::make_shared<macdetect::Value>("request", "disable-stream");
    pktRequest->add(std::make_shared<macdetect::Value>("device-name", strDeviceName));
    
    std::shared_ptr<macdetect::Value> pktResult = this->requestResponse(pktRequest);
    
    if(pktResult->sub("result")) {
      if(pktResult->sub("result")->content() == "success") {
	bSuccess = true;
      }
    }
    
    return bSuccess;
  }
  
  std::shared_ptr<macdetect::Value> MDClient::receive() {
    return this->get("", "", false);
  }
}
