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


#include <macdetectd/Daemon.h>


namespace macdetect {
  Daemon::Daemon() {
    m_srvServer.serve("lo", 7090);
    
    m_nwNetwork.setAutoManageDevices(true);
    m_nwNetwork.setDeviceWhiteBlacklistMode(macdetect::Network::Whitelist);
    
    // TODO: This is too narrow; re-specify
    m_nwNetwork.addDeviceWhiteBlacklistEntry("(wlan)(.*)");
    m_nwNetwork.addDeviceWhiteBlacklistEntry("(eth)(.*)");
  }
  
  Daemon::~Daemon() {
  }
  
  std::shared_ptr<Value> Daemon::response(std::shared_ptr<Value> valValue, std::list< std::pair<std::string, std::string> > lstSubValues) {
    std::shared_ptr<Value> valResponse = NULL;
    
    if(valValue->key() == "request") {
      std::shared_ptr<Value> valPre(std::make_shared<Value>("response", valValue->content(), lstSubValues));
      valResponse = std::move(valPre);
    }
    
    return valResponse;
  }
  
  bool Daemon::cycle() {
    bool bSuccess = false;
    
    if(m_nwNetwork.cycle() && m_srvServer.cycle()) {
      // First, handle removed Served instances.
      std::list< std::shared_ptr<Served> > lstRemovedServed = m_srvServer.removed();
      for(std::shared_ptr<Served> svrServed : lstRemovedServed) {
	bool bChanged = true;
	
	while(bChanged) {
	  bChanged = false;
	  
	  for(std::list<Stream>::iterator itStream = m_lstStreams.begin(); itStream != m_lstStreams.end(); itStream++) {
	    if((*itStream).svrServed == svrServed) {
	      m_lstStreams.erase(itStream);
	      bChanged = true;
	      
	      break;
	    }
	  }
	}
      }
      
      // Handle queued data packets.
      std::list<Server::QueuedValue> lstValues = m_srvServer.queuedValues();
      
      for(Server::QueuedValue qvValue : lstValues) {
	std::shared_ptr<Value> valValue = qvValue.valValue;
	Server::Serving sviServing = m_srvServer.servingByID(qvValue.nServingID);
	
	if(valValue->key() == "request") {
	  if(valValue->content() == "devices-list") {
	    std::list< std::shared_ptr<Device> > lstDevices = m_nwNetwork.knownDevices();
	    
	    std::shared_ptr<Value> valDevices = this->response(valValue);
	    for(std::shared_ptr<Device> dvDevice : lstDevices) {
	      std::shared_ptr<Value> valAdd = std::make_shared<Value>("device", dvDevice->deviceName());
	      valAdd->add("mac", dvDevice->mac());
	      valAdd->add("ip", dvDevice->ip());
	      valAdd->add("broadcast-ip", dvDevice->broadcastIP());
	      
	      valDevices->add(valAdd);
	    }
	    
	    qvValue.svrServed->send(valDevices);
	  } else if(valValue->content() == "known-mac-addresses") {
	    std::list<Network::MACEntity> lstMACs = m_nwNetwork.knownMACs();
	    
	    std::shared_ptr<Value> valMACs = this->response(valValue);
	    for(Network::MACEntity meMAC : lstMACs) {
	      std::shared_ptr<Value> valMAC = std::make_shared<Value>("mac", meMAC.strMAC);
	      valMAC->add(std::make_shared<Value>("device-name", meMAC.strDeviceName));
	      
	      std::stringstream sts;
	      sts << meMAC.dLastSeen;
	      valMAC->add(std::make_shared<Value>("last-seen", sts.str()));
	      sts.str("");
	      sts << meMAC.dFirstSeen;
	      valMAC->add(std::make_shared<Value>("first-seen", sts.str()));
	      valMAC->add(std::make_shared<Value>("vendor", m_nwNetwork.readableMACIdentifier(meMAC.strMAC, false)));
	      
	      valMACs->add(valMAC);
	    }
	    
	    qvValue.svrServed->send(valMACs);
	  } else if(valValue->content() == "enable-stream") {
	    std::shared_ptr<Value> valDeviceName = valValue->sub("device-name");
	    
	    if(valDeviceName) {
	      std::string strDeviceName = valDeviceName->content();
	      
	      if(this->streamEnabled(qvValue.svrServed, sviServing.strDeviceName)) {
		std::shared_ptr<Value> valResponse = this->response(valValue, {{"result", "already-enabled"}});
		valResponse->add(std::make_shared<Value>("device-name", strDeviceName));
		qvValue.svrServed->send(valResponse);
	      } else {
		this->enableStream(qvValue.svrServed, sviServing.strDeviceName);
		
		std::shared_ptr<Value> valResponse = this->response(valValue, {{"result", "success"}});
		valResponse->add(std::make_shared<Value>("device-name", strDeviceName));
		qvValue.svrServed->send(valResponse);
	      }
	    } else {
	      std::shared_ptr<Value> valResponse = this->response(valValue, {{"result", "device-name-missing"}});
	      qvValue.svrServed->send(valResponse);
	    }
	  } else if(valValue->content() == "disable-stream") {
	    std::shared_ptr<Value> valDeviceName = valValue->sub("device-name");
	    
	    if(valDeviceName) {
	      std::string strDeviceName = valDeviceName->content();
	      
	      if(this->streamEnabled(qvValue.svrServed, sviServing.strDeviceName)) {
		this->disableStream(qvValue.svrServed, sviServing.strDeviceName);
		
		std::shared_ptr<Value> valResponse = this->response(valValue, {{"result", "success"}});
		valResponse->add(std::make_shared<Value>("device-name", strDeviceName));
		qvValue.svrServed->send(valResponse);
	      } else {
		std::shared_ptr<Value> valResponse = this->response(valValue, {{"result", "already-disabled"}});
		valResponse->add(std::make_shared<Value>("device-name", strDeviceName));
		qvValue.svrServed->send(valResponse);
	      }
	    } else {
	      std::shared_ptr<Value> valResponse = this->response(valValue, {{"result", "device-name-missing"}});
	      qvValue.svrServed->send(valResponse);
	    }
	  }
	}
      }
      
      // Handle network events
      std::list< std::shared_ptr<Event> > lstEvents = m_nwNetwork.events();
      
      for(std::shared_ptr<Event> evEvent : lstEvents) {
	std::shared_ptr<Value> valSend = std::make_shared<Value>();
	std::string strDeviceName = "";
	
	switch(evEvent->type()) {
	case Event::DeviceAdded: {
	  std::shared_ptr<macdetect::DeviceEvent> devEvent = std::static_pointer_cast<macdetect::DeviceEvent>(evEvent);
	  
	  valSend->set("info", "device-added");
	  valSend->add(std::make_shared<Value>("device-name", devEvent->deviceName()));
	} break;
	  
	case Event::DeviceRemoved: {
	  std::shared_ptr<macdetect::DeviceEvent> devEvent = std::static_pointer_cast<macdetect::DeviceEvent>(evEvent);
	  
	  bool bChanged = true;
	  while(bChanged) {
	    bChanged = false;
	    
	    for(std::list<Stream>::iterator itStream = m_lstStreams.begin(); itStream != m_lstStreams.end(); itStream++) {
	      if((*itStream).strDeviceName == devEvent->deviceName()) {
		std::shared_ptr<Value> valDisabled = std::make_shared<Value>("info", "stream-disabled");
		valDisabled->add(std::make_shared<Value>("device-name", devEvent->deviceName()));
		(*itStream).svrServed->send(valDisabled);
		
		m_lstStreams.erase(itStream);
		bChanged = true;
		
		break;
	      }
	    }
	  }
	  
	  valSend->set("info", "device-removed");
	  valSend->add(std::make_shared<Value>("device-name", devEvent->deviceName()));
	} break;
	  
	case Event::DeviceStateChanged: {
	  std::shared_ptr<macdetect::DeviceEvent> devEvent = std::static_pointer_cast<macdetect::DeviceEvent>(evEvent);
	  
	  valSend->set("info", "device-state-changed");
	} break;
	  
	case Event::DeviceEvidenceChanged: {
	  std::shared_ptr<macdetect::DeviceEvent> devEvent = std::static_pointer_cast<macdetect::DeviceEvent>(evEvent);
	  
	  valSend->set("info", "device-evidence-changed");
	} break;
	  
	case Event::MACAddressDiscovered: {
	  std::shared_ptr<macdetect::MACEvent> mvEvent = std::static_pointer_cast<macdetect::MACEvent>(evEvent);
	  
	  valSend->set("info", "mac-address-discovered");
	  valSend->add(std::make_shared<Value>("mac", mvEvent->macAddress()));
	  valSend->add(std::make_shared<Value>("device-name", mvEvent->deviceName()));
	  
	  strDeviceName = mvEvent->deviceName();
	} break;
	  
	case Event::MACAddressDisappeared: {
	  std::shared_ptr<macdetect::MACEvent> mvEvent = std::static_pointer_cast<macdetect::MACEvent>(evEvent);
	  
	  valSend->set("info", "mac-address-disappeared");
	  valSend->add(std::make_shared<Value>("mac", mvEvent->macAddress()));
	  valSend->add(std::make_shared<Value>("device-name", mvEvent->deviceName()));
	  
	  strDeviceName = mvEvent->deviceName();
	} break;
	  
	default: {
	  valSend = NULL;
	} break;
	}
	
	if(valSend) {
	  std::list< std::pair< std::shared_ptr<Served>, int> > lstServed = m_srvServer.served();
	  
	  for(std::pair< std::shared_ptr<Served>, int> prServed : lstServed) {
	    std::shared_ptr<Served> svrServed = prServed.first;
	    
	    if(strDeviceName == "" || this->streamEnabled(svrServed, strDeviceName)) {
	      svrServed->send(valSend);
	    }
	  }
	}
      }
      
      bSuccess = true;
    }
    
    return bSuccess;
  }
  
  bool Daemon::enableStream(std::shared_ptr<Served> svrServed, std::string strDeviceName) {
    if(!this->streamEnabled(svrServed, strDeviceName)) {
      m_lstStreams.push_back({svrServed, strDeviceName});
    }
  }
  
  bool Daemon::disableStream(std::shared_ptr<Served> svrServed, std::string strDeviceName) {
    bool bResult = false;
    
    for(std::list<Stream>::iterator itStream = m_lstStreams.begin(); itStream != m_lstStreams.end(); itStream++) {
      if((*itStream).svrServed == svrServed && (*itStream).strDeviceName == strDeviceName) {
	m_lstStreams.erase(itStream);
	bResult = true;
	
	break;
      }
    }
    
    return bResult;
  }
  
  bool Daemon::streamEnabled(std::shared_ptr<Served> svrServed, std::string strDeviceName) {
    bool bResult = false;
    
    for(std::list<Stream>::iterator itStream = m_lstStreams.begin(); itStream != m_lstStreams.end(); itStream++) {
      if((*itStream).svrServed == svrServed && (*itStream).strDeviceName == strDeviceName) {
	bResult = true;
	
	break;
      }
    }
    
    return bResult;
  }
  
  void Daemon::shutdown() {
    m_srvServer.shutdown();
    m_nwNetwork.shutdown();
  }
}
