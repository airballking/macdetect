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
  
  bool PacketEntity::send(std::shared_ptr<Value> valSend) {
    std::lock_guard<std::mutex> lgGuard(m_mtxSocketAccess);
    
    unsigned char ucarrBuffer[2048];
    unsigned int unLength = valSend->serialize(ucarrBuffer, 2048);
    
    int nResult = ::write(m_nSocketFD, ucarrBuffer, unLength);
    
    return nResult == unLength;
  }
  
  std::shared_ptr<Value> PacketEntity::receive(bool& bDisconnected) {
    std::lock_guard<std::mutex> lgGuard(m_mtxSocketAccess);
    
    std::shared_ptr<Value> valReceived = NULL;
    
    unsigned char ucarrBuffer[2048];
    int nLength = ::read(m_nSocketFD, ucarrBuffer, 2048);
    
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
      valReceived->deserialize(ucarrBuffer, nLength);
    }
    
    return valReceived;
  }
  
  bool PacketEntity::failureState() {
    return m_bFailureState;
  }
}
