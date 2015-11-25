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


#ifndef __WIRE_H__
#define __WIRE_H__


// System
#include <iostream>
#include <string>
#include <memory.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ether.h>

// MAC Detect
#include <macdetect/SocketReader.h>


namespace macdetect {
  class Wire : public SocketReader {
  private:
    std::string m_strDeviceName;
    unsigned short m_usProtocol;
    
  public:
    Wire(std::string strDeviceName, int nDefaultReadingLength, unsigned short usProtocol = ETH_P_ALL);
    ~Wire();
    
    static int wrapInEthernetFrame(std::string strSourceMAC, std::string strDestinationMAC, unsigned short usEtherType, void* vdPayload, unsigned int unPayloadLength, void* vdBuffer);
    
    int createSocket(std::string strDeviceName, unsigned short usProtocol);
    
    bool write(void* vdBuffer, unsigned int unLength);
    bool write(void* vdBuffer, unsigned int unLength, unsigned short usProtocol);
  };
}


#endif /* __WIRE_H__ */
