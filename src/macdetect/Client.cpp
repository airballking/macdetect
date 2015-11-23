#include <macdetect/Client.h>


namespace macdetect {
  Client::Client() : m_nSocketFD(-1) {
  }
  
  Client::~Client() {
    this->disconnect();
  }
  
  bool Client::connect(std::string strIP, unsigned short usPort) {
    bool bConnected = false;
    
    if(this->connected()) {
      this->disconnect();
    }
    
    m_nSocketFD = ::socket(PF_INET, SOCK_STREAM, 0);
    
    if(m_nSocketFD != -1) {
      struct sockaddr_in sinAddress;
      memset(&sinAddress, 0, sizeof(struct sockaddr_in));
      
      sinAddress.sin_family = AF_INET;
      sinAddress.sin_port = htons(usPort);
      inet_pton(AF_INET, strIP.c_str(), &(sinAddress.sin_addr));
      
      if(::connect(m_nSocketFD, (struct sockaddr*)&sinAddress, sizeof(struct sockaddr)) == 0) {
	bConnected = true;
      }
    }
    
    return bConnected;
  }
  
  bool Client::disconnect() {
    bool bDisconnected = false;
    
    if(this->connected()) {
      ::close(m_nSocketFD);
      m_nSocketFD = -1;
      
      bDisconnected = true;
    }
    
    return bDisconnected;
  }
  
  bool Client::connected() {
    return (m_nSocketFD != -1);
  }
}
