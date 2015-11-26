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


#include <macdetectd/SocketReader.h>


namespace macdetect {
  SocketReader::SocketReader() : m_ucInternalBuffer(NULL) {
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
