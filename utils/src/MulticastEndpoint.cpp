#include <macdetect-utils/MulticastEndpoint.h>


namespace macdetect {
  MulticastEndpoint::MulticastEndpoint(std::string strMulticastGroup, unsigned short usPort) : m_strMulticastGroup(strMulticastGroup), m_usPort(usPort) {
    this->setSocket(::socket(AF_INET, SOCK_DGRAM, 0));
    
    memset(&m_saAddrGroup, 0, sizeof(m_saAddrGroup));
    m_saAddrGroup.sin_family = AF_INET;
    m_saAddrGroup.sin_addr.s_addr = inet_addr(m_strMulticastGroup.c_str());
    m_saAddrGroup.sin_port = htons(m_usPort);
    
    unsigned int unYes = 1;
    bool bOK = false;
    
    if(setsockopt(this->socket(), SOL_SOCKET, SO_REUSEADDR, &unYes, sizeof(unYes)) >= 0) {
      if(setsockopt(this->socket(), IPPROTO_IP, IP_MULTICAST_LOOP, &unYes, sizeof(unYes)) >= 0) {
	memset(&m_saAddrAny, 0, sizeof(m_saAddrAny));
	m_saAddrAny.sin_family = AF_INET;
	m_saAddrAny.sin_addr.s_addr = htonl(INADDR_ANY);
	m_saAddrAny.sin_port = htons(m_usPort);
	
	if(bind(this->socket(), (struct sockaddr*)&m_saAddrAny, sizeof(m_saAddrAny)) >= 0) {
	  struct ip_mreq ipmReq;
	  ipmReq.imr_multiaddr.s_addr = inet_addr(m_strMulticastGroup.c_str());
	  ipmReq.imr_interface.s_addr = htonl(INADDR_ANY);
	  
	  if(setsockopt(this->socket(), IPPROTO_IP, IP_ADD_MEMBERSHIP, &ipmReq, sizeof(ipmReq)) >= 0) {
	    bOK = true;
	  }
	}
      }
    }
    
    if(!bOK) {
      ::close(this->socket());
      this->setSocket(-1);
      
      // TODO(winkler): Solve problem with multicast group not being
      // joined when only the loopback interface is available.
      
      //std::cerr << strerror(errno) << std::endl;
    }
  }
  
  MulticastEndpoint::~MulticastEndpoint() {
  }
  
  bool MulticastEndpoint::ok() {
    return this->socket() != -1;
  }
  
  bool MulticastEndpoint::cycle() {
    return false;
  }
  
  int MulticastEndpoint::write(void* vdBuffer, unsigned int unLength) {
    return sendto(this->socket(), vdBuffer, unLength, 0, (struct sockaddr*)&m_saAddrGroup, sizeof(m_saAddrGroup));
  }
  
  int MulticastEndpoint::read(unsigned char* vdBuffer, unsigned int unLength, int nFlags, std::string& strIP) {
    struct sockaddr saAddr;
    
    socklen_t slLength = sizeof(m_saAddrGroup);
    int nRecv = ::recvfrom(this->socket(), vdBuffer, unLength, nFlags | MSG_DONTWAIT, &saAddr, &slLength);
    
    strIP = std::string(inet_ntoa(((struct sockaddr_in*)&saAddr)->sin_addr));
    
    if(nRecv == -1) {
      if(errno == EWOULDBLOCK) {
	return 0;
      }
      
      return -1;
    } else {
      if(nRecv == 0) {
	return -1;
      }
      
      return nRecv;
    }
  }
}
