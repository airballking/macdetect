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
  
  macdetect::Value* MDClient::get(std::string strKey, std::string strValue, bool bBlock) {
    macdetect::Value* valReturn = NULL;
    bool bReceived = false;
    
    while(!bReceived) {
      while(m_lstReceivedValues.size() == 0) {
	macdetect::Value* valReceived = m_cliClient.receive();
	
	if(valReceived) {
	  m_lstReceivedValues.push_back(valReceived);
	}
      }
      
      for(std::list<macdetect::Value*>::iterator itValue = m_lstReceivedValues.begin(); itValue != m_lstReceivedValues.end(); itValue++) {
	macdetect::Value* valValue = *itValue;
	
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
  
  macdetect::Value* MDClient::requestResponse(std::string strRequest) {
    macdetect::Value* pktRequest = new macdetect::Value("request", strRequest);
    macdetect::Value* pktResponse = this->requestResponse(pktRequest);
    delete pktRequest;
    
    return pktResponse;
  }
  
  macdetect::Value* MDClient::requestResponse(macdetect::Value* pktRequest, std::string strKey) {
    m_cliClient.send(pktRequest);
    
    return this->get(strKey, pktRequest->content());
  }
  
  std::list<std::string> MDClient::deviceNames() {
    std::list<std::string> lstDeviceNames;
    macdetect::Value* pktDevices = this->requestResponse("devices-list");
    
    for(macdetect::Value* pktSub : pktDevices->subValues()) {
      lstDeviceNames.push_back(pktSub->content());
    }
    
    delete pktDevices;
    
    return lstDeviceNames;
  }
  
  std::list<macdetect::Value*> MDClient::devicesList() {
    std::list<macdetect::Value*> lstDevices;
    macdetect::Value* pktResponse = this->requestResponse("devices-list");
    
    for(macdetect::Value* pktDevice : pktResponse->subValues()) {
      if(pktDevice->key() == "device") {
	lstDevices.push_back(pktDevice->copy());
      }
    }
    
    return lstDevices;
  }
  
  std::list<std::string> MDClient::knownMACAddresses() {
    std::list<std::string> lstMACAddresses;
    macdetect::Value* pktMACAddresses = this->requestResponse("known-mac-addresses");
    
    for(macdetect::Value* pktSub : pktMACAddresses->subValues()) {
      lstMACAddresses.push_back(pktSub->content());
    }
    
    delete pktMACAddresses;
    
    return lstMACAddresses;
  }
  
  bool MDClient::enableStream(std::string strDeviceName) {
    bool bSuccess = false;
    
    macdetect::Value* pktRequest = new macdetect::Value("request", "enable-stream");
    pktRequest->add(new macdetect::Value("device-name", strDeviceName));
    
    macdetect::Value* pktResult = this->requestResponse(pktRequest);
    delete pktRequest;
    
    if(pktResult->sub("result")) {
      if(pktResult->sub("result")->content() == "success") {
	bSuccess = true;
      }
    }
    
    delete pktResult;
    
    return bSuccess;
  }
  
  bool MDClient::disableStream(std::string strDeviceName) {
    bool bSuccess = false;
    
    macdetect::Value* pktRequest = new macdetect::Value("request", "disable-stream");
    pktRequest->add(new macdetect::Value("device-name", strDeviceName));
    
    macdetect::Value* pktResult = this->requestResponse(pktRequest);
    delete pktRequest;
    
    if(pktResult->sub("result")) {
      if(pktResult->sub("result")->content() == "success") {
	bSuccess = true;
      }
    }
    
    delete pktResult;
    
    return bSuccess;
  }
  
  macdetect::Value* MDClient::info() {
    return this->get(""/*info*/, "", false);
  }
}
