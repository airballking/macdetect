#include <detect/SocketReader.h>


namespace detect {
  SocketReader::SocketReader(std::string strDeviceName) : m_strDeviceName(strDeviceName) {
    m_ucInternalBuffer = NULL;
  }
  
  SocketReader::~SocketReader() {
    if(m_ucInternalBuffer != NULL) {
      delete[] m_ucInternalBuffer;
      m_ucInternalBuffer = NULL;
    }
  }
  
  void SocketReader::setSocket(int nSocket) {
    m_nSocket = nSocket;
  }
  
  int SocketReader::socket() {
    return m_nSocket;
  }
  
  int SocketReader::read(unsigned char* ucBuffer, unsigned int unLength) {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    
    strncpy(ifr.ifr_name, m_strDeviceName.c_str(), sizeof(ifr.ifr_name) - 1);
    ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = '\0';
    
    ioctl(m_nSocket, SIOCGIFINDEX, &ifr);
    
    struct sockaddr_ll socket_address;
    memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sll_family = AF_PACKET;
    socket_address.sll_ifindex = ifr.ifr_ifindex;
    socket_address.sll_protocol = htons(0x0003); // ETH_P_ALL
    
    //int nRecv = ::recv(m_nSocket, ucBuffer, unLength, MSG_DONTWAIT);
    socklen_t slLength = sizeof(socket_address);
    int nRecv = ::recvfrom(m_nSocket, ucBuffer, unLength, MSG_DONTWAIT, (struct sockaddr*)&socket_address, &slLength);
    
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
  
  int SocketReader::forward(unsigned int unBytes) {
    unsigned char ucBuffer[unBytes];
    
    return this->read(ucBuffer, unBytes);
  }
  
  int SocketReader::bufferedRead(unsigned char **ucBuffer) {
    return this->bufferedRead(ucBuffer, m_nDefaultReadingLength);
  }
  
  int SocketReader::bufferedRead(unsigned char **ucBuffer, int nReadingLength) {
    if(m_ucInternalBuffer != NULL) {
      delete[] m_ucInternalBuffer;
      m_ucInternalBuffer = NULL;
    }
    
    m_ucInternalBuffer = new unsigned char[nReadingLength]();
    int nReadBytes = this->read(m_ucInternalBuffer, nReadingLength);
    
    // if(nReadBytes != nReadingLength) {
    //   delete[] m_ucInternalBuffer;
    //   m_ucInternalBuffer = NULL;
    // } else {
    //   *ucBuffer = m_ucInternalBuffer;
    // }
    *ucBuffer = m_ucInternalBuffer;
    
    return nReadBytes;
  }
  
  void SocketReader::setDefaultReadingLength(int nDefaultReadingLength) {
    m_nDefaultReadingLength = nDefaultReadingLength;
  }
  
  int SocketReader::defaultReadingLength() {
    return m_nDefaultReadingLength;
  }
}
