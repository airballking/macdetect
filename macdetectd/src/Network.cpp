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


#include <macdetectd/Network.h>


namespace macdetect {
  Network::Network() : m_bShouldRun(true), m_dMaxMACAge(300.0), m_dUpdateInterval(10.0), m_dPingBroadcastInterval(10.0), m_nSocketFDControl(socket(AF_INET, SOCK_STREAM, 0)), m_wbmDevices(Blacklist), m_bIgnoreDeviceMACs(true) {
    m_dtData.readVendors();
  }
  
  Network::~Network() {
    close(m_nSocketFDControl);
  }
  
  bool Network::privilegesSuffice() {
    Wire wrTest("", ETH_FRAME_LEN);
    bool bSuffices = (wrTest.socket() > -1);
    
    return bSuffices;
  }
  
  void Network::setIgnoreDeviceMACs(bool bIgnore) {
    m_bIgnoreDeviceMACs = bIgnore;
  }
  
  bool Network::ignoreDeviceMACs() {
    return m_bIgnoreDeviceMACs;
  }
  
  void Network::clearDeviceWhiteBlacklist() {
    m_lstDeviceWhiteBlackList.clear();
  }
  
  void Network::setDeviceWhiteBlacklistMode(WhiteBlackListMode wbmSet) {
    m_wbmDevices = wbmSet;
  }
  
  void Network::addDeviceWhiteBlacklistEntry(std::string strPattern) {
    m_lstDeviceWhiteBlackList.push_back(strPattern);
    std::list<std::string> lstRemoveDevices;
    
    for(std::shared_ptr<Device> dvDevice : this->knownDevices()) {
      if(!this->deviceAllowed(dvDevice->deviceName())) {
	lstRemoveDevices.push_back(dvDevice->deviceName());
      }
    }
    
    for(std::string strDeviceName : lstRemoveDevices) {
      this->removeDevice(strDeviceName);
    }
  }
  
  bool Network::deviceAllowed(std::string strDeviceName) {
    bool bDeviceInList = false;
    
    for(std::string strPattern : m_lstDeviceWhiteBlackList) {
      std::regex rgxRegex(strPattern);
      
      if(std::regex_match(strDeviceName, rgxRegex)) {
	bDeviceInList = true;
      }
    }
    
    return (m_wbmDevices == Whitelist && bDeviceInList) ||
      (m_wbmDevices == Blacklist && !bDeviceInList);
  }
  
  bool Network::cycle() {
    bool bResult = true;
    double dTime = this->time();
    
    m_lstEvents.clear();
    
    this->detectNetworkActivity();
    
    m_lstSystemDeviceNames = this->systemDeviceNames();
    
    if(this->autoManageDevices()) {
      std::list<std::string> lstAddedDevices;
      std::list<std::string> lstRemovedDevices;
      
      for(std::string strDeviceName : m_lstSystemDeviceNames) {
	if(this->knownDevice(strDeviceName) == NULL) {
	  lstAddedDevices.push_back(strDeviceName);
	}
      }
      
      for(std::shared_ptr<Device> dvCurrent : this->knownDevices()) {
	if(std::find(m_lstSystemDeviceNames.begin(), m_lstSystemDeviceNames.end(), dvCurrent->deviceName()) == m_lstSystemDeviceNames.end()) {
	  lstRemovedDevices.push_back(dvCurrent->deviceName());
	}
      }
      
      for(std::string strDeviceName : lstRemovedDevices) {
	this->removeDevice(strDeviceName);
      }
      
      for(std::string strDeviceName : lstAddedDevices) {
	this->addDevice(strDeviceName);
      }
    }
    
    for(std::shared_ptr<Device> dvMaintain : m_lstDevices) {
      this->maintainDeviceStatus(dvMaintain);
    }
    
    bool bChanged = true;
    while(bChanged) {
      bChanged = false;
      
      for(std::list<MACEntity>::iterator itMAC = m_lstMACSeen.begin();
	  itMAC != m_lstMACSeen.end(); itMAC++) {
	if(dTime - (*itMAC).dLastSeen > m_dMaxMACAge) {
	  this->scheduleEvent(std::make_shared<MACEvent>(Event::MACAddressDisappeared, (*itMAC).strDeviceName, (*itMAC).strMAC));
	  
	  m_lstMACSeen.erase(itMAC);
	  bChanged = true;
	  
	  break;
	}
      }
    }
    
    // Try MAC entity update
    for(std::list<MACEntity>::iterator itMAC = m_lstMACSeen.begin();
	itMAC != m_lstMACSeen.end(); itMAC++) {
      if((*itMAC).strHostName == "") {
	if(dTime - (*itMAC).dLastUpdate > m_dUpdateInterval) {
	  (*itMAC).dLastUpdate = dTime;
	  // ...
	}
      }
    }
    
    // Send broadcasts
    for(std::shared_ptr<Device> dvCurrent : m_lstDevices) {
      dvCurrent->sendPingBroadcast(dTime, m_dPingBroadcastInterval);
    }
    
    return (bResult && m_bShouldRun);
  }
  
  void Network::shutdown() {
    m_bShouldRun = false;
  }
  
  bool Network::addDevice(std::string strDeviceName) {
    bool bResult = false;
    
    if(this->deviceAllowed(strDeviceName)) {
      if(this->knownDevice(strDeviceName) == NULL) {
	if(this->systemDeviceNameExists(strDeviceName)) {
	  std::shared_ptr<Device> dvNew = std::make_shared<Device>(strDeviceName, this->deviceHardwareType(strDeviceName));
	  m_lstDevices.push_back(dvNew);
	  
	  this->scheduleEvent(std::make_shared<DeviceEvent>(Event::DeviceAdded, strDeviceName));
	  
	  Devicestd::shared_ptr<Event> devEvidenceMAC = std::make_shared<DeviceEvent>(Event::DeviceEvidenceChanged, strDeviceName);
	  devEvidenceMAC->setEvidence("mac", dvNew->mac(), "");
	  this->scheduleEvent(devEvidenceMAC);
	  
	  bResult = true;
	}
      }
    }
    
    return bResult;
  }
  
  bool Network::removeDevice(std::string strDeviceName) {
    bool bResult = false;
    
    for(std::list< std::shared_ptr<Device> >::iterator itDevice = m_lstDevices.begin();
	itDevice != m_lstDevices.end(); itDevice++) { 
      if((*itDevice)->deviceName() == strDeviceName) {
	m_lstDevices.erase(itDevice);
	
	break;
      }
    }
    
    this->scheduleEvent(std::make_shared<DeviceEvent>(Event::DeviceRemoved, strDeviceName));
    
    bool bChanged = true;
    while(bChanged) {
      bChanged = false;
      
      for(std::list<MACEntity>::iterator itMAC = m_lstMACSeen.begin();
	  itMAC != m_lstMACSeen.end(); itMAC++) {
	if((*itMAC).strDeviceName == strDeviceName) {
	  this->removeMAC(strDeviceName, (*itMAC).strMAC);
	  bChanged = true;
	  
	  break;
	}
      }
    }
    
    return bResult;
  }
  
  void Network::setAutoManageDevices(bool bAutoManageDevices) {
    m_bAutoManageDevices = bAutoManageDevices;
  }
  
  bool Network::autoManageDevices() {
    return m_bAutoManageDevices;
  }
  
  std::shared_ptr<Device> Network::knownDevice(std::string strDeviceName) {
    std::shared_ptr<Device> dvReturn = NULL;
    
    for(std::shared_ptr<Device> dvCurrent : m_lstDevices) {
      if(dvCurrent->deviceName() == strDeviceName) {
	dvReturn = dvCurrent;
	break;
      }
    }
    
    return dvReturn;
  }
  
  std::list< std::shared_ptr<Device> > Network::knownDevices() {
    return m_lstDevices;
  }
  
  bool Network::systemDeviceNameExists(std::string strDeviceName) {
    return std::find(m_lstSystemDeviceNames.begin(), m_lstSystemDeviceNames.end(), strDeviceName) != m_lstSystemDeviceNames.end();
  }
  
  std::list<std::string> Network::systemDeviceNames() {
    std::list<std::string> lstDevices;
    
    struct ifconf ifcConf;
    struct ifreq ifrReq[100];
    
    if(m_nSocketFDControl >= 0) {
      ifcConf.ifc_buf = (char*)ifrReq;
      ifcConf.ifc_len = sizeof(ifrReq);
      
      if(ioctl(m_nSocketFDControl, SIOCGIFCONF, &ifcConf) != -1) {
	unsigned int unInterfaces = ifcConf.ifc_len / sizeof(ifrReq[0]);
	
	for(unsigned int unInterface = 0; unInterface < unInterfaces; unInterface++) {
	  struct ifreq ifrReqCurrent = ifrReq[unInterface];
	  std::string strInterfaceName = ifrReqCurrent.ifr_name;
	  
	  lstDevices.push_back(strInterfaceName);
	}
      }
    }
    
    return lstDevices;
  }
  
  void Network::maintainDeviceStatus(std::shared_ptr<Device> dvMaintain) {
    if(m_nSocketFDControl >= 0) {
      struct ifreq ifrTemp;
      
      unsigned int unLength = dvMaintain->deviceName().length();
      
      unLength = (unLength > 15 ? 15 : unLength);
      memset(ifrTemp.ifr_name, 0, 16);
      memcpy(ifrTemp.ifr_name, dvMaintain->deviceName().c_str(), unLength);
      
      ioctl(m_nSocketFDControl, SIOCGIFFLAGS, &ifrTemp);
      
      bool bUp = ifrTemp.ifr_flags & IFF_UP;
      bool bRunning = ifrTemp.ifr_flags & IFF_RUNNING;
      
      if(dvMaintain->up() != bUp) {
	std::shared_ptr<DeviceEvent> evUp = std::make_shared<DeviceEvent>(Event::DeviceStateChanged, dvMaintain->deviceName());
	evUp->setStateChangeUp(true);
	
	this->scheduleEvent(evUp);
      }
      
      if(dvMaintain->running() != bRunning) {
	std::shared_ptr<DeviceEvent> evRunning = std::make_shared<DeviceEvent>(Event::DeviceStateChanged, dvMaintain->deviceName());
	evRunning->setStateChangeRunning(true);
	
	this->scheduleEvent(evRunning);
      }
      
      dvMaintain->setUp(bUp);
      dvMaintain->setRunning(bRunning);
      
      // Check IP address
      memset(&ifrTemp, 0, sizeof(struct ifreq));
      memcpy(ifrTemp.ifr_name, dvMaintain->deviceName().c_str(), dvMaintain->deviceName().length());
      ifrTemp.ifr_name[dvMaintain->deviceName().length()] = 0;
      
      ioctl(dvMaintain->wire()->socket(), SIOCGIFADDR, &ifrTemp);
      
      std::string strIP = inet_ntoa(((struct sockaddr_in*)&ifrTemp.ifr_addr)->sin_addr);
      
      if(strIP != dvMaintain->ip()) {
	std::shared_ptr<DeviceEvent> devEvidenceIP = std::make_shared<DeviceEvent>(Event::DeviceEvidenceChanged, dvMaintain->deviceName());
	devEvidenceIP->setEvidence("ip", strIP, dvMaintain->ip());
	this->scheduleEvent(devEvidenceIP);
	
	dvMaintain->setIP(strIP);
      }
      
      // Check Broadcast IP address
      memset(&ifrTemp, 0, sizeof(struct ifreq));
      memcpy(ifrTemp.ifr_name, dvMaintain->deviceName().c_str(), dvMaintain->deviceName().length());
      ifrTemp.ifr_name[dvMaintain->deviceName().length()] = 0;
      
      ioctl(dvMaintain->wire()->socket(), SIOCGIFBRDADDR, &ifrTemp);
      
      strIP = inet_ntoa(((struct sockaddr_in*)&ifrTemp.ifr_addr)->sin_addr);
      
      if(strIP != dvMaintain->broadcastIP()) {
	std::shared_ptr<DeviceEvent> devEvidenceIP = std::make_shared<DeviceEvent>(Event::DeviceEvidenceChanged, dvMaintain->deviceName());
	devEvidenceIP->setEvidence("broadcast-ip", strIP, dvMaintain->broadcastIP());
	this->scheduleEvent(devEvidenceIP);
	
	dvMaintain->setBroadcastIP(strIP);
      }
    }
  }
  
  Device::HardwareType Network::deviceHardwareType(std::string strDeviceName) {
    Device::HardwareType hwtReturn;
    
    std::shared_ptr<Device> dvDevice = this->knownDevice(strDeviceName);
    
    if(dvDevice) {
      hwtReturn = dvDevice->hardwareType();
    } else {
      struct ifconf ifcConf;
      struct ifreq ifrReq[100];
      
      if(m_nSocketFDControl >= 0) {
	ifcConf.ifc_buf = (char*)ifrReq;
	ifcConf.ifc_len = sizeof(ifrReq);
	
	if(ioctl(m_nSocketFDControl, SIOCGIFCONF, &ifcConf) != -1) {
	  unsigned int unInterfaces = ifcConf.ifc_len / sizeof(ifrReq[0]);
	  
	  for(unsigned int unInterface = 0; unInterface < unInterfaces; unInterface++) {
	    struct ifreq ifrReqCurrent = ifrReq[unInterface];
	    std::string strInterfaceName = ifrReqCurrent.ifr_name;
	    
	    if(strInterfaceName == strDeviceName) {
	      struct ifreq ifrTemp;
	      
	      strcpy(ifrTemp.ifr_name, ifrReqCurrent.ifr_name);
	      ioctl(m_nSocketFDControl, SIOCGIFFLAGS, &ifrTemp);
	      
	      if(ifrTemp.ifr_flags & IFF_LOOPBACK) {
		hwtReturn = Device::Loopback;
	      } else if(ioctl(m_nSocketFDControl, SIOCGIWNAME, &ifrTemp) == -1) {
		hwtReturn = Device::Wired;
	      } else {
		hwtReturn = Device::Wireless;
	      }
	      
	      break;
	    }
	  }
	}
      }
    }
    
    return hwtReturn;
  }
  
  void Network::scheduleEvent(std::shared_ptr<Event> evSchedule) {
    m_lstEvents.push_back(evSchedule);
  }
  
  std::list< std::shared_ptr<Event> > Network::events() {
    return m_lstEvents;
  }
  
  std::string Network::mac(unsigned char* ucMAC) {
    std::stringstream sts;
    
    for(int nI = 0; nI < 6; nI++) {
      int nByte = ucMAC[nI];
      sts << std::setfill('0') << std::setw(2) << std::hex << nByte;
	  
      if(nI < 5) {
	sts << ":";
      }
    }
    
    return sts.str();
  }
  
  double Network::time() {
    struct timespec tsTime;
    clock_gettime(CLOCK_MONOTONIC, &tsTime);
    
    return tsTime.tv_sec + double(tsTime.tv_nsec) / NSEC_PER_SEC;
  }
  
  void Network::detectNetworkActivity() {
    unsigned char* ucBuffer = NULL;
    int nLengthRead;
    
    for(std::shared_ptr<Device> dvDevice : this->knownDevices()) {
      ucBuffer = dvDevice->read(nLengthRead);
      
      if(nLengthRead >= sizeof(struct ethhdr)) {
	struct ethhdr efhHeader;
	memcpy(&efhHeader, ucBuffer, sizeof(efhHeader));
	std::string strMAC = this->mac(efhHeader.h_source);
	
	// TEST
	//dvDevice->sendPingBroadcast
	//this->sendPing(dvDevice, "255.255.255.255");
	
	this->addMAC(strMAC, dvDevice->deviceName());
	
	switch(ntohs(efhHeader.h_proto)) {
	case 0x0800: { // IP
	  struct iphdr iphHeader;
	  memcpy(&iphHeader, &(ucBuffer[sizeof(ethhdr)]), sizeof(struct iphdr));
	  
	  std::string strSenderIP = inet_ntoa(((struct sockaddr_in*)&iphHeader.saddr)->sin_addr);
	  
	  if(this->ipAllowed(strSenderIP)) {
	    // TODO: Fully implement IP address support.
	  }
	  
	  if(iphHeader.protocol == 0x1) { // ICMP
	    // TODO: Received a ping. Do something?
	  }
	  
	  //m_rpRARP.requestIP(strMAC, dvDevice);
	} break;
	  
	case 0x8035: { // RARP
	  std::cout << "Received RARP packet" << std::endl;
	} break;
	  
	default: {
	} break;
	}
      } else if(nLengthRead == -1) {
	std::cerr << "Error while reading on device '" << dvDevice->deviceName() << "'" << std::endl;
      }
    }
  }
  
  bool Network::ipAllowed(std::string strIP) {
    bool bAllowed = false;
    
    if(strIP != "255.255.255.255" &&
       strIP.substr(0, 3) != "224") {
      bAllowed = true;
    }
    
    return bAllowed;
  }
  
  bool Network::macAllowed(std::string strMAC) {
    bool bAllowed = false;
    
    if(strMAC != "00:00:00:00:00:00" && strMAC != "ff:ff:ff:ff:ff:ff") {
      bAllowed = true;
      
      if(m_bIgnoreDeviceMACs) {
	for(std::shared_ptr<Device> dvDevice : this->knownDevices()) {
	  if(dvDevice->mac() == strMAC) {
	    bAllowed = false;
	    
	    break;
	  }
	}
      }
    }
    
    return bAllowed;
  }
  
  bool Network::addMAC(std::string strMACAddress, std::string strDeviceName) {
    bool bResult = false;
    
    if(this->macAllowed(strMACAddress)) {
      if(this->macLastSeen(strMACAddress, strDeviceName) == -1) {
	this->scheduleEvent(std::make_shared<MACEvent>(Event::MACAddressDiscovered, strDeviceName, strMACAddress));
      }
      
      bool bWasPresent = false;
      for(std::list<MACEntity>::iterator itMAC = m_lstMACSeen.begin();
	  itMAC != m_lstMACSeen.end(); itMAC++) {
	if((*itMAC).strMAC == strMACAddress && (*itMAC).strDeviceName == strDeviceName) {
	  (*itMAC).dLastSeen = this->time();
	  bWasPresent = true;
	}
      }
      
      if(!bWasPresent) {
	double dTime = this->time();
	m_lstMACSeen.push_back({strMACAddress, strDeviceName, "", "", dTime, dTime, 0});
	bResult = true;
      }
    }
    
    return bResult;
  }
  
  void Network::removeMAC(std::string strDeviceName, std::string strMAC) {
    for(std::list<MACEntity>::iterator itMAC = m_lstMACSeen.begin();
	itMAC != m_lstMACSeen.end(); itMAC++) {
      if((*itMAC).strDeviceName == strDeviceName && (*itMAC).strMAC == strMAC) {
	m_lstMACSeen.erase(itMAC);
	
	this->scheduleEvent(std::make_shared<MACEvent>(Event::MACAddressDisappeared, strDeviceName, strMAC));
	
	break;
      }
    }
  }
  
  double Network::macLastSeen(std::string strMAC, std::string strDeviceName) {
    double dTime = -1;
    
    for(MACEntity meEntity : m_lstMACSeen) {
      if(meEntity.strMAC == strMAC && meEntity.strDeviceName == strDeviceName) {
	dTime = meEntity.dLastSeen;
	
	break;
      }
    }
    
    return dTime;
  }
  
  void Network::setMACMaxAge(double dMaxAge) {
    m_dMaxMACAge = dMaxAge;
  }
  
  double Network::macMaxAge() {
    return m_dMaxMACAge;
  }
  
  std::string Network::readableMACIdentifier(std::string strMAC, bool bDefaultToMAC) {
    std::string strReturn = "";
    Data::Vendor vdVendor = m_dtData.vendorForMAC(strMAC);
    
    if(!vdVendor.bValid) {
      if(bDefaultToMAC) {
	strReturn = strMAC;
      }
    } else {
      strReturn = "\"" + vdVendor.strVendor + "\"" + strMAC.substr(8);
    }
    
    return strReturn;
  }
  
  std::list<Network::MACEntity> Network::knownMACs() {
    return m_lstMACSeen;
  }
}
