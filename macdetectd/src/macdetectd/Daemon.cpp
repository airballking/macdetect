#include <macdetectd/Daemon.h>


namespace macdetect {
  Daemon::Daemon() {
    m_srvServer.serve("lo", 7090);
    
    m_nwNetwork.setAutoManageDevices(true);
    m_nwNetwork.setDeviceWhiteBlacklistMode(macdetect::Network::Whitelist);
    m_nwNetwork.addDeviceWhiteBlacklistEntry("(wlan)(.*)");
    m_nwNetwork.addDeviceWhiteBlacklistEntry("(eth)(.*)");
  }
  
  Daemon::~Daemon() {
  }
  
  Packet* Daemon::responsePacket(Packet* pktPacket, std::list< std::pair<std::string, std::string> > lstSubPackets) {
    Packet* pktResponse = NULL;
    
    if(pktPacket->key() == "request") {
      pktResponse = new Packet("response", pktPacket->value(), lstSubPackets);
    }
    
    return pktResponse;
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
      std::list<Server::QueuedPacket> lstPackets = m_srvServer.queuedPackets();
      
      for(Server::QueuedPacket qpPacket : lstPackets) {
	Packet* pktPacket = qpPacket.pktPacket;
	Server::Serving sviServing = m_srvServer.servingByID(qpPacket.nServingID);
	
	if(pktPacket->key() == "request") {
	  if(pktPacket->value() == "devices-list") {
	    std::list<Device*> lstDevices = m_nwNetwork.knownDevices();
	    
	    Packet* pktDevices = this->responsePacket(pktPacket);
	    for(Device* dvDevice : lstDevices) {
	      pktDevices->add(new Packet("device", dvDevice->deviceName(),
					 {{"mac", dvDevice->mac()},
					  {"ip", dvDevice->ip()},
					  {"broadcast-ip", dvDevice->broadcastIP()}}));
	    }
	    
	    qpPacket.svrServed->sendPacket(pktDevices);
	    delete pktDevices;
	  } else if(pktPacket->value() == "known-mac-addresses") {
	    std::list<Network::MACEntity> lstMACs = m_nwNetwork.knownMACs();
	    
	    Packet* pktMACs = this->responsePacket(pktPacket);
	    for(Network::MACEntity meMAC : lstMACs) {
	      Packet* pktMAC = new Packet("mac", meMAC.strMAC);
	      pktMAC->add(new Packet("device-name", meMAC.strDeviceName));
	      
	      std::stringstream sts;
	      sts << meMAC.dLastSeen;
	      pktMAC->add(new Packet("last-seen", sts.str()));
	      sts.str("");
	      sts << meMAC.dFirstSeen;
	      pktMAC->add(new Packet("first-seen", sts.str()));
	      
	      pktMACs->add(pktMAC);
	    }
	    
	    qpPacket.svrServed->sendPacket(pktMACs);
	    delete pktMACs;
	  } else if(pktPacket->value() == "enable-stream") {
	    Packet* pktDeviceName = pktPacket->sub("device-name");
	    
	    if(pktDeviceName) {
	      std::string strDeviceName = pktDeviceName->value();
	      
	      if(this->streamEnabled(qpPacket.svrServed, sviServing.strDeviceName)) {
		Packet* pktResponse = this->responsePacket(pktPacket, {{"result", "already-enabled"}});
		pktResponse->add(new Packet("device-name", strDeviceName));
		qpPacket.svrServed->sendPacket(pktResponse);
		
		delete pktResponse;
	      } else {
		this->enableStream(qpPacket.svrServed, sviServing.strDeviceName);
		
		Packet* pktResponse = this->responsePacket(pktPacket, {{"result", "success"}});
		pktResponse->add(new Packet("device-name", strDeviceName));
		qpPacket.svrServed->sendPacket(pktResponse);
		
		delete pktResponse;
	      }
	    } else {
	      Packet* pktResponse = this->responsePacket(pktPacket, {{"result", "device-name-missing"}});
	      qpPacket.svrServed->sendPacket(pktResponse);
	      
	      delete pktResponse;
	    }
	  } else if(pktPacket->value() == "disable-stream") {
	    Packet* pktDeviceName = pktPacket->sub("device-name");
	    
	    if(pktDeviceName) {
	      std::string strDeviceName = pktDeviceName->value();
	      
	      if(this->streamEnabled(qpPacket.svrServed, sviServing.strDeviceName)) {
		this->disableStream(qpPacket.svrServed, sviServing.strDeviceName);
		
		Packet* pktResponse = this->responsePacket(pktPacket, {{"result", "success"}});
		pktResponse->add(new Packet("device-name", strDeviceName));
		qpPacket.svrServed->sendPacket(pktResponse);
		
		delete pktResponse;
	      } else {
		Packet* pktResponse = this->responsePacket(pktPacket, {{"result", "already-disabled"}});
		pktResponse->add(new Packet("device-name", strDeviceName));
		qpPacket.svrServed->sendPacket(pktResponse);
		
		delete pktResponse;
	      }
	    } else {
	      Packet* pktResponse = this->responsePacket(pktPacket, {{"result", "device-name-missing"}});
	      qpPacket.svrServed->sendPacket(pktResponse);
	      
	      delete pktResponse;
	    }
	  }
	}
	
	delete qpPacket.pktPacket;
      }
      
      // Handle network events
      std::list<Event*> lstEvents = m_nwNetwork.events();
      
      for(Event* evEvent : lstEvents) {
	Packet* pktSend = new Packet();
	std::string strDeviceName = "";
	
	switch(evEvent->type()) {
	case Event::DeviceAdded: {
	  macdetect::DeviceEvent* devEvent = (macdetect::DeviceEvent*)evEvent;
	  
	  pktSend->set("info", "device-added");
	  pktSend->add(new Packet("device-name", devEvent->deviceName()));
	} break;
	  
	case Event::DeviceRemoved: {
	  macdetect::DeviceEvent* devEvent = (macdetect::DeviceEvent*)evEvent;
	  
	  bool bChanged = true;
	  while(bChanged) {
	    bChanged = false;
	    
	    for(std::list<Stream>::iterator itStream = m_lstStreams.begin(); itStream != m_lstStreams.end(); itStream++) {
	      if((*itStream).strDeviceName == devEvent->deviceName()) {
		Packet* pktDisabled = new Packet("info", "stream-disabled");
		pktDisabled->add(new Packet("device-name", devEvent->deviceName()));
		(*itStream).svrServed->sendPacket(pktDisabled);
		delete pktDisabled;
		
		m_lstStreams.erase(itStream);
		bChanged = true;
		
		break;
	      }
	    }
	  }
	  
	  pktSend->set("info", "device-removed");
	  pktSend->add(new Packet("device-name", devEvent->deviceName()));
	} break;
	  
	case Event::DeviceStateChanged: {
	  macdetect::DeviceEvent* devEvent = (macdetect::DeviceEvent*)evEvent;
	  
	  pktSend->set("info", "device-state-changed");
	} break;
	  
	case Event::DeviceEvidenceChanged: {
	  macdetect::DeviceEvent* devEvent = (macdetect::DeviceEvent*)evEvent;
	  
	  pktSend->set("info", "device-evidence-changed");
	} break;
	  
	case Event::MACAddressDiscovered: {
	  macdetect::MACEvent* mvEvent = (macdetect::MACEvent*)evEvent;
	  
	  pktSend->set("info", "mac-address-discovered");
	  pktSend->add(new Packet("mac", mvEvent->macAddress()));
	  pktSend->add(new Packet("device-name", mvEvent->deviceName()));
	  
	  strDeviceName = mvEvent->deviceName();
	} break;
	  
	case Event::MACAddressDisappeared: {
	  macdetect::MACEvent* mvEvent = (macdetect::MACEvent*)evEvent;
	  
	  pktSend->set("info", "mac-address-disappeared");
	  pktSend->add(new Packet("mac", mvEvent->macAddress()));
	  pktSend->add(new Packet("device-name", mvEvent->deviceName()));
	  
	  strDeviceName = mvEvent->deviceName();
	} break;
	  
	default: {
	  delete pktSend;
	  pktSend = NULL;
	} break;
	}
	
	if(pktSend) {
	  std::list< std::pair<Served*, int> > lstServed = m_srvServer.served();
	  
	  for(std::pair<Served*, int> prServed : lstServed) {
	    Served* svrServed = prServed.first;
	    
	    if(strDeviceName == "" || this->streamEnabled(svrServed, strDeviceName)) {
	      svrServed->sendPacket(pktSend);
	    }
	  }
	  
	  delete pktSend;
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
