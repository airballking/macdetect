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


#ifndef __RARP_H__
#define __RARP_H__


// System
#include <net/ethernet.h>
#include <netinet/ether.h>

// MAC Detect
#include <macdetectd/Device.h>


namespace macdetect {
  typedef struct {
    uint16_t un16HardwareType;
    uint16_t un16ProtocolType;
    uint8_t un8HardwareAddressLength;
    uint8_t un8ProtocolAddressLength;
    uint16_t un16Opcode;
    unsigned char uc6SourceHWAddr[6];
    unsigned char uc4SourceProtocolAddr[4];
    unsigned char uc6DestinationHWAddr[6];
    unsigned char uc4DestinationProtocolAddr[4];
  } RARPPacket;
  
  
  class RARP {
  private:
  protected:
  public:
    RARP();
    ~RARP();
    
    void requestIP(std::string strMAC, Device* dvDevice);
    std::string ipFromResponse(RARPPacket rpResponse);
  };
}


#endif /* __RARP_H__ */
