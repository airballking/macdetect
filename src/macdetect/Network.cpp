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


#include <macdetect/Network.h>


namespace macdetect {
  Network::Network() : m_bShouldRun(true), m_dMaxMACAge(300.0), m_nSocketFDControl(socket(AF_INET, SOCK_STREAM, 0)) {
    m_dtData.readVendors();
  }
  
  Network::~Network() {
    for(Device* dvDelete : m_lstDevices) {
      delete dvDelete;
    }
    
    m_lstDevices.clear();
    
    for(Event* evDelete : m_lstEvents) {
      delete evDelete;
    }
    
    m_lstEvents.clear();
    
    close(m_nSocketFDControl);
  }
  
  bool Network::privilegesSuffice() {
    Wire wrTest("", ETH_FRAME_LEN);
    bool bSuffices = (wrTest.socket() > -1);
    
    return bSuffices;
  }
  
  bool Network::cycle() {
    bool bResult = true;
    
    for(Event* evDelete : m_lstEvents) {
      delete evDelete;
    }
    
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
      
      for(Device* dvCurrent : this->knownDevices()) {
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
    
    for(Device* dvMaintain : m_lstDevices) {
      this->maintainDeviceStatus(dvMaintain);
    }
    
    double dTime = this->time();
    bool bChanged = true;
    while(bChanged) {
      bChanged = false;
      
      for(std::list<MACEntity>::iterator itMAC = m_lstMACSeen.begin();
	  itMAC != m_lstMACSeen.end(); itMAC++) {
	if(dTime - (*itMAC).dLastSeen > m_dMaxMACAge) {
	  this->scheduleEvent(new MACEvent(Event::MACAddressDisappeared, (*itMAC).strDeviceName, (*itMAC).strMAC));
	  
	  m_lstMACSeen.erase(itMAC);
	  bChanged = true;
	  
	  break;
	}
      }
    }
    
    return (bResult && m_bShouldRun);
  }
  
  void Network::shutdown() {
    m_bShouldRun = false;
  }
  
  bool Network::addDevice(std::string strDeviceName) {
    bool bResult = false;
    
    if(this->knownDevice(strDeviceName) == NULL) {
      if(this->systemDeviceNameExists(strDeviceName)) {
	m_lstDevices.push_back(new Device(strDeviceName, this->deviceHardwareType(strDeviceName)));
      }
      
      this->scheduleEvent(new DeviceEvent(Event::DeviceAdded, strDeviceName));
    }
    
    return bResult;
  }
  
  bool Network::removeDevice(std::string strDeviceName) {
    bool bResult = false;
    
    for(std::list<Device*>::iterator itDevice = m_lstDevices.begin();
	itDevice != m_lstDevices.end(); itDevice++) { 
      if((*itDevice)->deviceName() == strDeviceName) {
	delete *itDevice;
	m_lstDevices.erase(itDevice);
	
	break;
      }
    }
    
    this->scheduleEvent(new DeviceEvent(Event::DeviceRemoved, strDeviceName));
    
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
  
  Device* Network::knownDevice(std::string strDeviceName) {
    Device* dvReturn = NULL;
    
    for(Device* dvCurrent : m_lstDevices) {
      if(dvCurrent->deviceName() == strDeviceName) {
	dvReturn = dvCurrent;
	break;
      }
    }
    
    return dvReturn;
  }
  
  std::list<Device*> Network::knownDevices() {
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
  
  void Network::maintainDeviceStatus(Device* dvMaintain) {
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
	DeviceEvent* evUp = new DeviceEvent(Event::DeviceStateChanged, dvMaintain->deviceName());
	evUp->setStateChangeUp(true);
	
	this->scheduleEvent(evUp);
      }
      
      if(dvMaintain->running() != bRunning) {
	DeviceEvent* evRunning = new DeviceEvent(Event::DeviceStateChanged, dvMaintain->deviceName());
	evRunning->setStateChangeRunning(true);
	
	this->scheduleEvent(evRunning);
      }
      
      dvMaintain->setUp(bUp);
      dvMaintain->setRunning(bRunning);
    }
  }
  
  Device::HardwareType Network::deviceHardwareType(std::string strDeviceName) {
    Device::HardwareType hwtReturn;
    
    Device* dvDevice = this->knownDevice(strDeviceName);
    
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
  
  void Network::scheduleEvent(Event* evSchedule) {
    m_lstEvents.push_back(evSchedule);
  }
  
  std::list<Event*> Network::events() {
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
    
    for(Device* dvDevice : this->knownDevices()) {
      ucBuffer = dvDevice->read(nLengthRead);
      
      if(nLengthRead >= sizeof(EthernetFrameHeader)) {
	EthernetFrameHeader efhHeader;
	memcpy(&efhHeader, ucBuffer, sizeof(efhHeader));
	
	// if(ntohs(efhHeader.usFrameType) == 0x0800) { // EtherType
	//   IPHeader iphHeader;
	//   memcpy(&iphHeader, &(ucBuffer[sizeof(EthernetFrameHeader)]), sizeof(IPHeader));
	  
	//   // Do something with IPv4 here?
	// }
	
	this->addMAC(this->mac(efhHeader.ucSenderHW), dvDevice->deviceName());
	this->addMAC(this->mac(efhHeader.ucReceiverHW), dvDevice->deviceName());
      } else if(nLengthRead == -1) {
	std::cerr << "Error while reading on device '" << dvDevice->deviceName() << "'" << std::endl;
      }
    }
  }
  
  void Network::addMAC(std::string strMACAddress, std::string strDeviceName) {
    if(strMACAddress != "00:00:00:00:00:00" && strMACAddress != "ff:ff:ff:ff:ff:ff") {
      if(this->macLastSeen(strMACAddress, strDeviceName) == -1) {
	this->scheduleEvent(new MACEvent(Event::MACAddressDiscovered, strDeviceName, strMACAddress));
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
	m_lstMACSeen.push_back({strMACAddress, strDeviceName, this->time()});
      }
    }
  }
  
  void Network::removeMAC(std::string strDeviceName, std::string strMAC) {
    for(std::list<MACEntity>::iterator itMAC = m_lstMACSeen.begin();
	itMAC != m_lstMACSeen.end(); itMAC++) {
      if((*itMAC).strDeviceName == strDeviceName && (*itMAC).strMAC == strMAC) {
	m_lstMACSeen.erase(itMAC);
	
	this->scheduleEvent(new MACEvent(Event::MACAddressDisappeared, strDeviceName, strMAC));
	
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
  
  std::string Network::readableMACIdentifier(std::string strMAC) {
    std::string strReturn = "";
    Data::Vendor vdVendor = m_dtData.vendorForMAC(strMAC);
    
    if(!vdVendor.bValid) {
      strReturn = strMAC;
    } else {
      strReturn = "\"" + vdVendor.strVendor + "\"" + strMAC.substr(8);
    }
    
    return strReturn;
  }
}
