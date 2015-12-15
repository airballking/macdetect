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
	fcntl(m_nSocketFD, F_SETFL, O_NONBLOCK);
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
