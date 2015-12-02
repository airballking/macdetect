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


#include <macdetect-client/Client.h>


namespace macdetect {
  Client::Client() : PacketEntity(-1) {
  }
  
  Client::~Client() {
    this->disconnect();
  }
  
  bool Client::connect(std::string strIP, unsigned short usPort) {
    bool bConnected = false;
    
    if(this->connected()) {
      this->disconnect();
    }
    
    m_nSocketFD = ::socket(AF_INET, SOCK_STREAM, 0);
    
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
