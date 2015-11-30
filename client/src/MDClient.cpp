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
  
  std::shared_ptr<macdetect::Value> MDClient::info() {
    return this->get(""/*info*/, "", false);
  }
}
