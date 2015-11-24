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
    bool bPresent = false;
    
    for(Serving srvServing : m_lstServings) {
      if(srvServing.strDeviceName == strDeviceName &&
	 srvServing.usPort == usPort) {
	bPresent = true;
      }
    }
    
    if(!bPresent) {
      Serving srvServing;
      srvServing.nSocketFD = ::socket(AF_INET, SOCK_STREAM, 0);
      
      if(srvServing.nSocketFD != -1) {
	struct ifreq ifrTemp;
	
	memset(&ifrTemp, 0, sizeof(struct ifreq));
	memcpy(ifrTemp.ifr_name, strDeviceName.c_str(), strDeviceName.length());
	ifrTemp.ifr_name[strDeviceName.length()] = 0;
	
	ioctl(srvServing.nSocketFD, SIOCGIFADDR, &ifrTemp);
	fcntl(srvServing.nSocketFD, F_SETFL, O_NONBLOCK);
	
	std::string strIP = inet_ntoa(((struct sockaddr_in*)&ifrTemp.ifr_addr)->sin_addr);
	
	struct sockaddr_in sinAddress;
	memset(&sinAddress, 0, sizeof(struct sockaddr_in));
	
	sinAddress.sin_family = AF_INET;
	sinAddress.sin_port = htons(usPort);
	inet_pton(AF_INET, strIP.c_str(), &(sinAddress.sin_addr));
	
	if(::bind(srvServing.nSocketFD, (struct sockaddr*)&sinAddress, sizeof(struct sockaddr)) == 0) {
	  srvServing.stStatus = Serving::Stopped;
	  srvServing.strDeviceName = strDeviceName;
	  srvServing.usPort = usPort;
	  srvServing.nID = this->freeServingID();
	  
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
      for(std::list<Serving>::iterator itServing = m_lstServings.begin(); itServing != m_lstServings.end(); itServing++) {
	if((*itServing).stStatus == Serving::Stopped) {
	  if(::listen((*itServing).nSocketFD, 3) == 0) {
	    (*itServing).stStatus = Serving::Started;
	  } else {
	    (*itServing).stStatus = Serving::Invalid;
	  }
	}
	
	struct sockaddr saAddress;
	socklen_t slLength = sizeof(struct sockaddr);
	int nSocketFDAccepted = ::accept4((*itServing).nSocketFD, &saAddress, &slLength, SOCK_NONBLOCK);
	
	if(nSocketFDAccepted == -1) {
	  if(errno != EAGAIN && errno != EWOULDBLOCK) {
	    (*itServing).stStatus = Serving::Invalid;
	  }
	} else {
	  fcntl(nSocketFDAccepted, F_SETFL, O_NONBLOCK);
	  m_lstServed.push_back(std::make_pair(new Served(nSocketFDAccepted), (*itServing).nID));
	}
      }
      
      bool bChanged = true;
      while(bChanged) {
	bChanged = false;
	
	for(std::list<Serving>::iterator itServing = m_lstServings.begin(); itServing != m_lstServings.end(); itServing++) {
	  if((*itServing).stStatus == Serving::Invalid || (*itServing).stStatus == Serving::Stopped) {
	    ::close((*itServing).nSocketFD);
	    m_lstServings.erase(itServing);
	    
	    bChanged = true;
	    
	    break;
	  }
	}
      }
      
      for(std::pair<Served*, int> prServed : m_lstServed) {
	Served* svrServed = prServed.first;
	//Serving srvServing = this->servingByID(prServed.second);
	
	//this->serve(svrServed);
      }
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
  
  int Server::freeServingID() {
    int nID = -1;
    bool bFound;
    
    do {
      bFound = false;
      nID++;
      
      for(Serving srvServing : m_lstServings) {
	if(srvServing.nID == nID) {
	  bFound = true;
	  
	  break;
	}
      }
    } while(bFound);
    
    return nID;
  }
  
  Server::Serving Server::servingByID(int nID) {
    for(Serving srvServing : m_lstServings) {
      if(srvServing.nID == nID) {
	return srvServing;
      }
    }
    
    return {Serving::Invalid, -1, -1, "", 0};
  }
}
