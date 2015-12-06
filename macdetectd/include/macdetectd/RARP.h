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


#ifndef __RARP_H__
#define __RARP_H__


// System
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <memory>

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
    
    void requestIP(std::string strMAC, std::shared_ptr<Device> dvDevice);
    std::string ipFromResponse(RARPPacket rpResponse);
  };
}


#endif /* __RARP_H__ */
