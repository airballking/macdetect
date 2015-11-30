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
  
  Value* Daemon::response(Value* valValue, std::list< std::pair<std::string, std::string> > lstSubValues) {
    Value* valResponse = NULL;
    
    if(valValue->key() == "request") {
      valResponse = new Value("response", valValue->content(), lstSubValues);
    }
    
    return valResponse;
  }
  
  bool Daemon::cycle() {
    bool bSuccess = false;
    
    if(m_nwNetwork.cycle() && m_srvServer.cycle()) {
      // First, handle removed Served instances.
      std::list<Served*> lstRemovedServed = m_srvServer.removed();
      for(Served* svrServed : lstRemovedServed) {
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
	Value* valValue = qvValue.valValue;
	Server::Serving sviServing = m_srvServer.servingByID(qvValue.nServingID);
	
	if(valValue->key() == "request") {
	  if(valValue->content() == "devices-list") {
	    std::list<Device*> lstDevices = m_nwNetwork.knownDevices();
	    
	    Value* valDevices = this->response(valValue);
	    for(Device* dvDevice : lstDevices) {
	      valDevices->add(new Value("device", dvDevice->deviceName(),
				        {{"mac", dvDevice->mac()},
				         {"ip", dvDevice->ip()},
					 {"broadcast-ip", dvDevice->broadcastIP()}}));
	    }
	    
	    qvValue.svrServed->send(valDevices);
	    delete valDevices;
	  } else if(valValue->content() == "known-mac-addresses") {
	    std::list<Network::MACEntity> lstMACs = m_nwNetwork.knownMACs();
	    
	    Value* valMACs = this->response(valValue);
	    for(Network::MACEntity meMAC : lstMACs) {
	      Value* valMAC = new Value("mac", meMAC.strMAC);
	      valMAC->add(new Value("device-name", meMAC.strDeviceName));
	      
	      std::stringstream sts;
	      sts << meMAC.dLastSeen;
	      valMAC->add(new Value("last-seen", sts.str()));
	      sts.str("");
	      sts << meMAC.dFirstSeen;
	      valMAC->add(new Value("first-seen", sts.str()));
	      valMAC->add(new Value("vendor", m_nwNetwork.readableMACIdentifier(meMAC.strMAC, false)));
	      
	      valMACs->add(valMAC);
	    }
	    
	    qvValue.svrServed->send(valMACs);
	    delete valMACs;
	  } else if(valValue->content() == "enable-stream") {
	    Value* valDeviceName = valValue->sub("device-name");
	    
	    if(valDeviceName) {
	      std::string strDeviceName = valDeviceName->content();
	      
	      if(this->streamEnabled(qvValue.svrServed, sviServing.strDeviceName)) {
		Value* valResponse = this->response(valValue, {{"result", "already-enabled"}});
		valResponse->add(new Value("device-name", strDeviceName));
		qvValue.svrServed->send(valResponse);
		
		delete valResponse;
	      } else {
		this->enableStream(qvValue.svrServed, sviServing.strDeviceName);
		
		Value* valResponse = this->response(valValue, {{"result", "success"}});
		valResponse->add(new Value("device-name", strDeviceName));
		qvValue.svrServed->send(valResponse);
		
		delete valResponse;
	      }
	    } else {
	      Value* valResponse = this->response(valValue, {{"result", "device-name-missing"}});
	      qvValue.svrServed->send(valResponse);
	      
	      delete valResponse;
	    }
	  } else if(valValue->content() == "disable-stream") {
	    Value* valDeviceName = valValue->sub("device-name");
	    
	    if(valDeviceName) {
	      std::string strDeviceName = valDeviceName->content();
	      
	      if(this->streamEnabled(qvValue.svrServed, sviServing.strDeviceName)) {
		this->disableStream(qvValue.svrServed, sviServing.strDeviceName);
		
		Value* valResponse = this->response(valValue, {{"result", "success"}});
		valResponse->add(new Value("device-name", strDeviceName));
		qvValue.svrServed->send(valResponse);
		
		delete valResponse;
	      } else {
		Value* valResponse = this->response(valValue, {{"result", "already-disabled"}});
		valResponse->add(new Value("device-name", strDeviceName));
		qvValue.svrServed->send(valResponse);
		
		delete valResponse;
	      }
	    } else {
	      Value* valResponse = this->response(valValue, {{"result", "device-name-missing"}});
	      qvValue.svrServed->send(valResponse);
	      
	      delete valResponse;
	    }
	  }
	}
	
	delete qvValue.valValue;
      }
      
      // Handle network events
      std::list<Event*> lstEvents = m_nwNetwork.events();
      
      for(Event* evEvent : lstEvents) {
	Value* valSend = new Value();
	std::string strDeviceName = "";
	
	switch(evEvent->type()) {
	case Event::DeviceAdded: {
	  macdetect::DeviceEvent* devEvent = (macdetect::DeviceEvent*)evEvent;
	  
	  valSend->set("info", "device-added");
	  valSend->add(new Value("device-name", devEvent->deviceName()));
	} break;
	  
	case Event::DeviceRemoved: {
	  macdetect::DeviceEvent* devEvent = (macdetect::DeviceEvent*)evEvent;
	  
	  bool bChanged = true;
	  while(bChanged) {
	    bChanged = false;
	    
	    for(std::list<Stream>::iterator itStream = m_lstStreams.begin(); itStream != m_lstStreams.end(); itStream++) {
	      if((*itStream).strDeviceName == devEvent->deviceName()) {
		Value* valDisabled = new Value("info", "stream-disabled");
		valDisabled->add(new Value("device-name", devEvent->deviceName()));
		(*itStream).svrServed->send(valDisabled);
		delete valDisabled;
		
		m_lstStreams.erase(itStream);
		bChanged = true;
		
		break;
	      }
	    }
	  }
	  
	  valSend->set("info", "device-removed");
	  valSend->add(new Value("device-name", devEvent->deviceName()));
	} break;
	  
	case Event::DeviceStateChanged: {
	  macdetect::DeviceEvent* devEvent = (macdetect::DeviceEvent*)evEvent;
	  
	  valSend->set("info", "device-state-changed");
	} break;
	  
	case Event::DeviceEvidenceChanged: {
	  macdetect::DeviceEvent* devEvent = (macdetect::DeviceEvent*)evEvent;
	  
	  valSend->set("info", "device-evidence-changed");
	} break;
	  
	case Event::MACAddressDiscovered: {
	  macdetect::MACEvent* mvEvent = (macdetect::MACEvent*)evEvent;
	  
	  valSend->set("info", "mac-address-discovered");
	  valSend->add(new Value("mac", mvEvent->macAddress()));
	  valSend->add(new Value("device-name", mvEvent->deviceName()));
	  
	  strDeviceName = mvEvent->deviceName();
	} break;
	  
	case Event::MACAddressDisappeared: {
	  macdetect::MACEvent* mvEvent = (macdetect::MACEvent*)evEvent;
	  
	  valSend->set("info", "mac-address-disappeared");
	  valSend->add(new Value("mac", mvEvent->macAddress()));
	  valSend->add(new Value("device-name", mvEvent->deviceName()));
	  
	  strDeviceName = mvEvent->deviceName();
	} break;
	  
	default: {
	  delete valSend;
	  valSend = NULL;
	} break;
	}
	
	if(valSend) {
	  std::list< std::pair<Served*, int> > lstServed = m_srvServer.served();
	  
	  for(std::pair<Served*, int> prServed : lstServed) {
	    Served* svrServed = prServed.first;
	    
	    if(strDeviceName == "" || this->streamEnabled(svrServed, strDeviceName)) {
	      svrServed->send(valSend);
	    }
	  }
	  
	  delete valSend;
	}
      }
      
      bSuccess = true;
    }
    
    return bSuccess;
  }
  
  bool Daemon::enableStream(Served* svrServed, std::string strDeviceName) {
    if(!this->streamEnabled(svrServed, strDeviceName)) {
      m_lstStreams.push_back({svrServed, strDeviceName});
    }
  }
  
  bool Daemon::disableStream(Served* svrServed, std::string strDeviceName) {
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
  
  bool Daemon::streamEnabled(Served* svrServed, std::string strDeviceName) {
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
