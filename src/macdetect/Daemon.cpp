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
  
  bool Daemon::cycle() {
    bool bSuccess = false;
    
    if(m_nwNetwork.cycle() && m_srvServer.cycle()) {
      std::list<Server::QueuedPacket> lstPackets = m_srvServer.queuedPackets();
      
      for(Server::QueuedPacket qpPacket : lstPackets) {
	Packet* pktPacket = qpPacket.pktPacket;
	
	if(pktPacket->key() == "request") {
	  if(pktPacket->value() == "devices-list") {
	    std::list<Device*> lstDevices = m_nwNetwork.knownDevices();
	    
	    Packet* pktDevices = new Packet("response", "devices-list");
	    for(Device* dvDevice : lstDevices) {
	      Packet* pktDevice = new Packet("device", dvDevice->deviceName());
	      pktDevice->add(new Packet("mac", dvDevice->mac()));
	      pktDevice->add(new Packet("ip", dvDevice->ip()));
	      pktDevice->add(new Packet("broadcast-ip", dvDevice->broadcastIP()));
	      
	      pktDevices->add(pktDevice);
	    }
	    
	    qpPacket.svrServed->sendPacket(pktDevices);
	    delete pktDevices;
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
