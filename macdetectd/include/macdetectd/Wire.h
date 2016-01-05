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
#include <syslog.h>
#include <sstream>

// MAC Detect
#include <macdetect-utils/SocketReader.h>
#include <macdetectd/Globals.h>


namespace macdetect {
  class Wire : public SocketReader {
  private:
    std::string m_strDeviceName;
    unsigned short m_usProtocol;
    
  public:
    Wire(std::string strDeviceName, int nDefaultReadingLength, unsigned short usProtocol = ETH_P_ALL, bool bSilent = false);
    ~Wire();
    
    static int wrapInEthernetFrame(std::string strSourceMAC, std::string strDestinationMAC, unsigned short usEtherType, void* vdPayload, unsigned int unPayloadLength, void* vdBuffer);
    
    int createSocket(std::string strDeviceName, unsigned short usProtocol, bool bSilent = false);
    
    bool write(void* vdBuffer, unsigned int unLength);
    bool write(void* vdBuffer, unsigned int unLength, unsigned short usProtocol);
  };
}


#endif /* __WIRE_H__ */
