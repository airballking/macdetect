#include <macdetect-client/MDClient.h>


namespace macdetect_client {
  MDClient::MDClient() {
  }
  
  MDClient::~MDClient() {
  }
  
  macdetect::Packet* MDClient::getPacket(std::string strKey, std::string strValue, bool bBlock) {
    macdetect::Packet* pktReturn = NULL;
    bool bReceived = false;
    
    while(!bReceived) {
      while(m_lstReceivedPackets.size() == 0) {
	macdetect::Packet* pktReceived = m_cliClient.receivePacket();
	
	if(pktReceived) {
	  m_lstReceivedPackets.push_back(pktReceived);
	}
      }
      
      for(std::list<macdetect::Packet*>::iterator itPacket = m_lstReceivedPackets.begin(); itPacket != m_lstReceivedPackets.end(); itPacket++) {
	macdetect::Packet* pktPacket = *itPacket;
	
	if((strKey == "" || pktPacket->key() == strKey) &&
	   (strValue == "" || pktPacket->value() == strValue)) {
	  pktReturn = pktPacket;
	  bReceived = true;
	  m_lstReceivedPackets.erase(itPacket);
	  
	  break;
	}
      }
      
      if(pktReturn == NULL) {
	if(!bBlock) {
	  break;
	}
      }
    }
    
    return pktReturn;
  }
  
  bool MDClient::connect(std::string strIP) {
    return m_cliClient.connect(strIP);
  }
  
  std::list<std::string> MDClient::deviceNames() {
    std::list<std::string> lstDeviceNames;
    
    macdetect::Packet* pktSend = new macdetect::Packet("request", "devices-list");
    m_cliClient.sendPacket(pktSend);
    
    macdetect::Packet* pktDevices = this->getPacket("response", "devices-list");
    for(macdetect::Packet* pktSub : pktDevices->subPackets()) {
      lstDeviceNames.push_back(pktSub->value());
    }
    
    return lstDeviceNames;
  }
  
  macdetect::Packet* MDClient::info() {
    return this->getPacket(""/*info*/, "", false);
  }
}
