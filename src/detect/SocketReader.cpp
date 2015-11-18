#include <detect/SocketReader.h>


namespace detect {
  SocketReader::SocketReader() {
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
    int nRecv = ::recv(m_nSocket, ucBuffer, unLength, MSG_DONTWAIT);
    
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
