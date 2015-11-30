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


#include <macdetectd/Server.h>


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
	  std::shared_ptr<Served> svrServed = std::make_shared<Served>(nSocketFDAccepted);
	  
	  // Initialize connection?
	  
	  m_lstServed.push_back(std::make_pair(svrServed, (*itServing).nID));
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
      
      std::list< std::shared_ptr<Served> > lstRemoveServed;
      
      for(std::pair< std::shared_ptr<Served>, int> prServed : m_lstServed) {
	std::shared_ptr<Served> svrServed = prServed.first;
	std::shared_ptr<Value> valReceived = svrServed->receive();
	
	if(valReceived) {
	  this->handleValue(svrServed, prServed.second, valReceived);
	} else {
	  if(svrServed->failureState()) {
	    lstRemoveServed.push_back(svrServed);
	  }
	}
      }
      
      for(std::shared_ptr<Served> svrRemove : lstRemoveServed) {
	for(std::list< std::pair< std::shared_ptr<Served>, int> >::iterator itServed = m_lstServed.begin(); itServed != m_lstServed.end(); itServed++) {
	  if((*itServed).first == svrRemove) {
	    m_lstRemoved.push_back((*itServed).first);
	    m_lstServed.erase(itServed);
	    
	    break;
	  }
	}
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
  
  void Server::handleValue(std::shared_ptr<Served> svrServed, int nServingID, std::shared_ptr<Value> valReceived) {
    m_lstValueQueue.push_back({valReceived, svrServed, nServingID});
  }
  
  std::list<Server::QueuedValue> Server::queuedValues() {
    std::list<QueuedValue> lstValues = m_lstValueQueue;
    m_lstValueQueue.clear();
    
    return lstValues;
  }
  
  std::list< std::pair< std::shared_ptr<Served>, int> > Server::served() {
    return m_lstServed;
  }
  
  void Server::distributeStreamValue(std::shared_ptr<Value> valStream) {
    for(std::pair<std::shared_ptr<Served>, int> prServed : m_lstServed) {
      prServed.first->send(valStream);
    }
  }
  
  std::list< std::shared_ptr<Served> > Server::removed() {
    std::list< std::shared_ptr<Served> > lstReturn = m_lstRemoved;
    m_lstRemoved.clear();
    
    return lstReturn;
  }
}
