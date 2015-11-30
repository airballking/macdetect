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
  
  std::shared_ptr<Value> PacketEntity::receive() {
    std::lock_guard<std::mutex> lgGuard(m_mtxSocketAccess);
    
    std::shared_ptr<Value> valReceived = NULL;
    
    unsigned char ucarrBuffer[2048];
    int nLength = ::read(m_nSocketFD, ucarrBuffer, 2048);
    
    if(nLength == -1) {
      if(errno != EAGAIN && errno != EWOULDBLOCK) {
	m_bFailureState = true;
      }
    } else if(nLength == 0) {
      m_bFailureState = true;
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
