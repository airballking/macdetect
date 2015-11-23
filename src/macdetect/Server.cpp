#include <macdetect/Server.h>


namespace macdetect {
  Server::Server() : m_bRun(true), m_nSocketFDControl(socket(AF_INET, SOCK_STREAM, 0)) {
  }
  
  Server::~Server() {
    for(Serving srvServing : m_lstServings) {
      ::close(srvServing.nSocketFD);
    }
    
    m_lstServings.clear();
  }
  
  bool Server::serve(std::string strDeviceName, unsigned short usPort) {
    bool bResult = false;
    bool bPreset = false;
    
    for(Serving srvServing : m_lstServings) {
      if(srvServing.strDeviceName == strDeviceName &&
	 srvServing.usPort == usPort) {
	bPreset = true;
      }
    }
    
    if(!bPreset) {
      Serving srvServing;
      srvServing.nSocketFD = ::socket(AF_INET, SOCK_STREAM, 0);
      
      if(srvServing.nSocketFD != -1) {
	struct ifreq ifrTemp;
	
	memset(&ifrTemp, 0, sizeof(struct ifreq));
	memcpy(ifrTemp.ifr_name, strDeviceName.c_str(), strDeviceName.length());
	ifrTemp.ifr_name[strDeviceName.length()] = 0;
	
	ioctl(srvServing.nSocketFD, SIOCGIFADDR, &ifrTemp);
	
	std::string strIP = inet_ntoa(((struct sockaddr_in*)&ifrTemp.ifr_addr)->sin_addr);
	
	struct sockaddr_in sinAddress;
	memset(&sinAddress, 0, sizeof(struct sockaddr_in));
      
	sinAddress.sin_family = AF_INET;
	sinAddress.sin_port = htons(usPort);
	inet_pton(AF_INET, strIP.c_str(), &(sinAddress.sin_addr));
	
	if(::bind(srvServing.nSocketFD, (struct sockaddr*)&sinAddress, sizeof(struct sockaddr)) == 0) {
	  srvServing.strDeviceName = strDeviceName;
	  srvServing.usPort = usPort;
	  
	  m_lstServings.push_back(srvServing);
	  
	  bResult = true;
	}
      }
    }
    
    return bResult;
  }
  
  bool Server::cycle() {
    bool bReturn = m_bRun;
    
    if(m_bRun) {
      // TODO: Implement main cycle function.
    }
    
    return bReturn;
  }
  
  void Server::shutdown() {
    m_bRun = false;
  }
  
  std::list<std::string> Server::systemDeviceNames() {
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
}
