#include <macdetect/Daemon.h>


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
  
  Packet* Daemon::responsePacket(Packet* pktPacket) {
    Packet* pktResponse = NULL;
    
    if(pktPacket->key() == "request") {
      pktResponse = new Packet("response", pktPacket->value());
    }
    
    return pktResponse;
  }
  
  Packet* Daemon::confirmationPacket(Packet* pktPacket) {
    Packet* pktConfirmation = new Packet("info", "confirmation");
    pktConfirmation->add(pktPacket->copy());
    
    return pktConfirmation;
  }
  
  bool Daemon::cycle() {
    bool bSuccess = false;
    
    if(m_nwNetwork.cycle() && m_srvServer.cycle()) {
      std::list<Server::QueuedPacket> lstPackets = m_srvServer.queuedPackets();
      
      for(Server::QueuedPacket qpPacket : lstPackets) {
	Packet* pktPacket = qpPacket.pktPacket;
	
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
	  }
	} else if(pktPacket->key() == "enable") {
	  if(pktPacket->value() == "stream") {
	    
	  }
	}
	
	delete qpPacket.pktPacket;
      }
      
      std::list<Event*> lstEvents = m_nwNetwork.events();
      
      for(Event* evEvent : lstEvents) {
	Packet* pktSend = new Packet();
	
	switch(evEvent->type()) {
	case Event::DeviceAdded: {
	  pktSend->set("info", "device-added");
	} break;
	  
	case Event::DeviceRemoved: {
	  pktSend->set("info", "device-removed");
	} break;
	  
	case Event::DeviceStateChanged: {
	  pktSend->set("info", "device-state-changed");
	} break;
	  
	case Event::DeviceEvidenceChanged: {
	  pktSend->set("info", "device-evidence-changed");
	} break;
	  
	case Event::MACAddressDiscovered: {
	  pktSend->set("info", "mac-address-discovered");
	} break;
	  
	case Event::MACAddressDisappeared: {
	  pktSend->set("info", "mac-address-disappeared");
	} break;
	  
	default: {
	  delete pktSend;
	  pktSend = NULL;
	} break;
	}
	
	if(pktSend) {
	  m_srvServer.distributeStreamPacket(pktSend);
	}
      }
      
      bSuccess = true;
    }
    
    return bSuccess;
  }
  
  void Daemon::shutdown() {
    m_srvServer.shutdown();
    m_nwNetwork.shutdown();
  }
}
