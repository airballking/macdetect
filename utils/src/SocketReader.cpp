// macdetect, a daemon and clients for detecting MAC addresses
// Copyright (C) 2015 Jan Winkler <jan.winkler.84@gmail.com>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

/** \author Jan Winkler */


#include <macdetect-utils/SocketReader.h>


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
    return this->read(ucBuffer, unLength, 0);
  }
  
  int SocketReader::read(unsigned char* ucBuffer, unsigned int unLength, int nFlags) {
    std::string strIP = "";
    
    return this->read(ucBuffer, unLength, nFlags, strIP);
  }
  
  int SocketReader::read(unsigned char* ucBuffer, unsigned int unLength, int nFlags, std::string& strIP) {
    struct sockaddr_in siAddr;
    socklen_t slSize = sizeof(siAddr);
    
    int nRecv = ::recvfrom(m_nSocket, ucBuffer, unLength, nFlags | MSG_DONTWAIT, (struct sockaddr*)&siAddr, &slSize);
    
    strIP = std::string(inet_ntoa(siAddr.sin_addr));
    
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
  
  int SocketReader::bufferedRead(unsigned char** ucBuffer) {
    return this->bufferedRead(ucBuffer, m_nDefaultReadingLength);
  }
  
  int SocketReader::bufferedRead(unsigned char** ucBuffer, int nReadingLength) {
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
