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


#include <macdetect-utils/PacketEntity.h>


namespace macdetect {
  PacketEntity::PacketEntity(int nSocketFD) : m_nSocketFD(nSocketFD), m_bFailureState(false) {
  }
  
  PacketEntity::~PacketEntity() {
    ::close(m_nSocketFD);
  }
  
  int PacketEntity::socket() {
    return m_nSocketFD;
  }
  
  void PacketEntity::setSocket(int nSocketFD) {
    m_nSocketFD = nSocketFD;
  }
  
  bool PacketEntity::send(std::shared_ptr<Value> valSend) {
    std::lock_guard<std::mutex> lgGuard(m_mtxSocketAccess);
    
    bool bResult = false;
    int nBufferSize = 0;
    int nSerializedLength = -1;
    unsigned char* ucarrBuffer = NULL;
    
    do {
      nBufferSize += 2048;
      
      if(ucarrBuffer) {
	delete[] ucarrBuffer;
      }
      
      ucarrBuffer = new unsigned char[nBufferSize];
      nSerializedLength = valSend->serialize(ucarrBuffer, nBufferSize);
    } while(nSerializedLength == -1);
    
    int nSentBytes = 0;
    while(nSentBytes < nSerializedLength) {
      int nResult = this->write(m_nSocketFD, &ucarrBuffer[nSentBytes], nSerializedLength - nSentBytes);
      
      if(nResult > 0) {
	nSentBytes += nResult;
      } else {
	bResult = false;
	break;
      }
    }
    
    if(ucarrBuffer) {
      delete[] ucarrBuffer;
    }
    
    if(nSentBytes == nSerializedLength) {
      bResult = true;
    }
    
    return bResult;
  }
  
  int PacketEntity::write(int nSocketFD, void* vdBuffer, unsigned int unLength) {
    return ::send(nSocketFD, vdBuffer, unLength, 0);
  }
  
  int PacketEntity::recv(int nSocketFD, void* vdBuffer, int nLength, int nFlags) {
    return ::recv(nSocketFD, vdBuffer, nLength, nFlags);
  }
  
  std::shared_ptr<Value> PacketEntity::receive(bool& bDisconnected) {
    std::lock_guard<std::mutex> lgGuard(m_mtxSocketAccess);
    
    std::shared_ptr<Value> valReceived = NULL;
    
    int nBufferSize = 0;
    unsigned char* ucarrBuffer = NULL;
    int nLength = 0;
    
    do {
      nBufferSize += 2048;
      
      if(ucarrBuffer) {
	delete[] ucarrBuffer;
      }
      
      ucarrBuffer = new unsigned char[nBufferSize];
      nLength = this->recv(m_nSocketFD, ucarrBuffer, nBufferSize, MSG_PEEK);
      
      if(!(nLength > 0)) {
	break;
      }
    } while(nLength == nBufferSize);
    
    if(nLength == -1) {
      if(errno != EAGAIN && errno != EWOULDBLOCK) {
	m_bFailureState = true;
	bDisconnected = true;
      }
    } else if(nLength == 0) {
      m_bFailureState = true;
      bDisconnected = true;
    } else {
      valReceived = std::make_shared<Value>();
      int nBytesUsed = valReceived->deserialize(ucarrBuffer, nBufferSize);
      
      // Only pull so many formerly peeked bytes in as we actually
      // used for the deserialization.
      ::recv(m_nSocketFD, ucarrBuffer, nBytesUsed, 0);
    }
    
    delete[] ucarrBuffer;
    
    return valReceived;
  }
  
  bool PacketEntity::failureState() {
    return m_bFailureState;
  }
}
