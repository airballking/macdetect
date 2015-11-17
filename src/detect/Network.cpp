#include <detect/Network.h>


namespace detect {
  Network::Network() {
    m_nSocketFDControl = socket(AF_INET, SOCK_STREAM, 0);
  }
  
  Network::~Network() {
    for(Device* dvDelete : m_lstDevices) {
      delete dvDelete;
    }
    
    m_lstDevices.clear();
    
    close(m_nSocketFDControl);
  }
  
  bool Network::cycle() {
    bool bResult = true;
    
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
	std::cout << "Removed device '" << strDeviceName << "'" << std::endl;
      }
      
      for(std::string strDeviceName : lstAddedDevices) {
	this->addDevice(strDeviceName);
	std::cout << "Added device '" << strDeviceName << "'" << std::endl;
	
	switch(this->knownDevice(strDeviceName)->hardwareType()) {
	case Device::Loopback: std::cout << " - Loopback" << std::endl; break;
	case Device::Wireless: std::cout << " - Wireless" << std::endl; break;
	case Device::Wired: std::cout << " - Wired" << std::endl; break;
	}
      }
    }
    
    return bResult;
  }
  
  bool Network::addDevice(std::string strDeviceName) {
    bool bResult = false;
    
    if(this->systemDeviceNameExists(strDeviceName)) {
      m_lstDevices.push_back(new Device(strDeviceName, this->deviceHardwareType(strDeviceName)));
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
      
      dvMaintain->setUp(ifrTemp.ifr_flags & IFF_UP);
      dvMaintain->setRunning(ifrTemp.ifr_flags & IFF_RUNNING);
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
}
