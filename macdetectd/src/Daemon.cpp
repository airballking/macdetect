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


#include <macdetectd/Daemon.h>


namespace macdetect {
  Daemon::Daemon() : m_dLastKeepAlive(0.0), m_dKeepAliveInterval(1.0) {
    m_nwNetwork.setAutoManageDevices(true);
    m_nwNetwork.setDeviceWhiteBlacklistMode(macdetect::Network::Whitelist);
    
    // TODO: This is too narrow; re-specify
    m_nwNetwork.addDeviceWhiteBlacklistEntry("(wlan)(.*)");
    m_nwNetwork.addDeviceWhiteBlacklistEntry("(wifi)(.*)");
    m_nwNetwork.addDeviceWhiteBlacklistEntry("(eth)(.*)");
    m_nwNetwork.addDeviceWhiteBlacklistEntry("(en)(.*)");
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
	std::shared_ptr<Value> valResponseOuter = this->response(valValue);
	std::shared_ptr<Value> valResponse = std::make_shared<Value>();
	valValue->print();
	if(valValue->key() == "request") {
	  if(valValue->content() == "devices-list") {
	    std::list< std::shared_ptr<Device> > lstDevices = m_nwNetwork.knownDevices();
	    
	    valResponse->set("devices-list", "");
	    for(std::shared_ptr<Device> dvDevice : lstDevices) {
	      std::shared_ptr<Value> valAdd = std::make_shared<Value>(dvDevice->deviceName(), "");
	      valAdd->add("mac", dvDevice->mac());
	      valAdd->add("ip", dvDevice->ip());
	      valAdd->add("broadcast-ip", dvDevice->broadcastIP());
	      
	      std::string strHWType = "";
	      switch(dvDevice->hardwareType()) {
	      case Device::Loopback:
		strHWType = "loopback";
		break;
		
	      case Device::Wired:
		strHWType = "wired";
		break;
		
	      case Device::Wireless:
		strHWType = "wireless";
		break;
	      }
	      
	      valAdd->add("hardware-type", strHWType);
	      
	      valResponse->add(valAdd);
	    }
	  } else if(valValue->content() == "known-mac-addresses") {
	    std::list<Network::MACEntity> lstMACs = m_nwNetwork.knownMACs();
	    
	    valResponse->set("known-mac-addresses", "");
	    for(Network::MACEntity meMAC : lstMACs) {
	      std::shared_ptr<Value> valMAC = std::make_shared<Value>(meMAC.strMAC, "");
	      valMAC->add(std::make_shared<Value>("device-name", meMAC.strDeviceName));
	      
	      std::stringstream sts;
	      sts << meMAC.dLastSeen;
	      valMAC->add(std::make_shared<Value>("last-seen", sts.str()));
	      sts.str("");
	      sts << meMAC.dFirstSeen;
	      valMAC->add(std::make_shared<Value>("first-seen", sts.str()));
	      valMAC->add(std::make_shared<Value>("vendor", m_nwNetwork.readableMACIdentifier(meMAC.strMAC, false)));
	      
	      valResponse->add(valMAC);
	    }
	  } else if(valValue->content() == "enable-stream") {
	    std::shared_ptr<Value> valDeviceName = valValue->sub("device-name");
	    
	    if(valDeviceName) {
	      std::string strDeviceName = valDeviceName->content();
	      
	      if(this->streamEnabled(qvValue.svrServed, sviServing.strDeviceName)) {
		valResponse->set("result", "already-enabled");
		valResponse->add(std::make_shared<Value>("device-name", strDeviceName));
	      } else {
		this->enableStream(qvValue.svrServed, strDeviceName);
		
		valResponse->set("result", "success");
		valResponse->add(std::make_shared<Value>("device-name", strDeviceName));
	      }
	    } else {
	      valResponse->set("result", "device-name-missing");
	    }
	  } else if(valValue->content() == "disable-stream") {
	    std::shared_ptr<Value> valDeviceName = valValue->sub("device-name");
	    
	    if(valDeviceName) {
	      std::string strDeviceName = valDeviceName->content();
	      
	      if(this->streamEnabled(qvValue.svrServed, sviServing.strDeviceName)) {
		this->disableStream(qvValue.svrServed, sviServing.strDeviceName);
		
		valResponse->set("result", "success");
		valResponse->add(std::make_shared<Value>("device-name", strDeviceName));
	      } else {
		valResponse->set("result", "already-disabled");
		valResponse->add(std::make_shared<Value>("device-name", strDeviceName));
	      }
	    } else {
	      valResponse->set("result", "device-name-missing");
	    }
	  }
	}
	
	valResponseOuter->add(valResponse);
	valResponseOuter->print();
	qvValue.svrServed->send(valResponseOuter);
      }
      
      // Handle network events
      std::list< std::shared_ptr<Event> > lstEvents = m_nwNetwork.events();
      
      for(std::shared_ptr<Event> evEvent : lstEvents) {
	std::shared_ptr<Value> valSend = std::make_shared<Value>();
	std::string strDeviceName = "";
	
	switch(evEvent->type()) {
	case Event::DeviceAdded: {
	  std::shared_ptr<macdetect::DeviceEvent> devEvent = std::dynamic_pointer_cast<macdetect::DeviceEvent>(evEvent);
	  
	  valSend->set("info", "device-added");
	  valSend->add(std::make_shared<Value>("device-name", devEvent->deviceName()));
	  
	  std::string strHWType = "";
	  switch(m_nwNetwork.deviceHardwareType(devEvent->deviceName())) {
	  case Device::Loopback:
	    strHWType = "loopback";
	    break;
	    
	  case Device::Wired:
	    strHWType = "wired";
	    break;
	    
	  case Device::Wireless:
	    strHWType = "wireless";
	    break;
	  }
	  
	  valSend->add("hardware-type", strHWType);
	} break;
	  
	case Event::DeviceRemoved: {
	  std::shared_ptr<macdetect::DeviceEvent> devEvent = std::dynamic_pointer_cast<macdetect::DeviceEvent>(evEvent);
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
	  std::shared_ptr<macdetect::DeviceEvent> devEvent = std::dynamic_pointer_cast<macdetect::DeviceEvent>(evEvent);
	  valSend->set("info", "device-state-changed");
	  
	  if(devEvent->stateChangeUp()) {
	    valSend->add(std::make_shared<Value>("changed-state", "up"));
	    valSend->add(std::make_shared<Value>("state", (devEvent->up() ? "true" : "false")));
	  } else if(devEvent->stateChangeRunning()) {
	    valSend->add(std::make_shared<Value>("changed-state", "running"));
	    valSend->add(std::make_shared<Value>("state", (devEvent->running() ? "true" : "false")));
	  }
	} break;
	  
	case Event::DeviceEvidenceChanged: {
	  std::shared_ptr<macdetect::DeviceEvent> devEvent = std::dynamic_pointer_cast<macdetect::DeviceEvent>(evEvent);
	  
	  valSend->set("info", "device-evidence-changed");
	  valSend->add(std::make_shared<Value>("field", devEvent->evidenceField()));
	  valSend->add(std::make_shared<Value>("value", devEvent->evidenceValue()));
	  valSend->add(std::make_shared<Value>("value-former", devEvent->evidenceValueFormer()));
	} break;
	  
	case Event::MACAddressDiscovered: {
	  std::shared_ptr<macdetect::MACEvent> mvEvent = std::dynamic_pointer_cast<macdetect::MACEvent>(evEvent);
	  
	  valSend->set("info", "mac-address-discovered");
	  valSend->add(std::make_shared<Value>("mac", mvEvent->macAddress()));
	  valSend->add(std::make_shared<Value>("device-name", mvEvent->deviceName()));
	  
	  strDeviceName = mvEvent->deviceName();
	} break;
	  
	case Event::MACAddressDisappeared: {
	  std::shared_ptr<macdetect::MACEvent> mvEvent = std::dynamic_pointer_cast<macdetect::MACEvent>(evEvent);
	  
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
	  for(std::pair< std::shared_ptr<Served>, int> prServed : m_srvServer.served()) {
	    std::shared_ptr<Served> svrServed = prServed.first;
	    
	    if(strDeviceName == "" || this->streamEnabled(svrServed, strDeviceName)) {
	      svrServed->send(valSend);
	    }
	  }
	}
      }
      
      // Last but not least, send the keepalive packages if necessary
      double dTime = m_nwNetwork.time();
      if(dTime > m_dLastKeepAlive + m_dKeepAliveInterval) {
	m_dLastKeepAlive = dTime;
	std::shared_ptr<Value> valKeepAlive = std::make_shared<Value>("info", "keepalive");
	
	for(std::pair< std::shared_ptr<Served>, int> prServed : m_srvServer.served()) {
	  std::shared_ptr<Served> svrServed = prServed.first;
	  
	  svrServed->send(valKeepAlive);
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
  
  bool Daemon::privilegesSuffice() {
    return m_nwNetwork.privilegesSuffice();
  }
  
  bool Daemon::serve(std::string strDeviceName, unsigned short usPort) {
    return m_srvServer.serve(strDeviceName, usPort);
  }
}
