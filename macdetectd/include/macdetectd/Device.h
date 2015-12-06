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


#ifndef __DEVICE_H__
#define __DEVICE_H__


// System
#include <string>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

// Detect
#include <macdetectd/Wire.h>


namespace macdetect {
  class Device {
  public:
    typedef enum {
      Loopback = 0,
      Wired = 1,
      Wireless = 2
    } HardwareType;
    
  private:
    int m_nSocketFD;
    bool m_bUp;
    bool m_bRunning;
    std::string m_strDeviceName;
    HardwareType m_hwtType;
    Wire m_wrWire;
    std::string m_strMAC;
    std::string m_strIP;
    std::string m_strBroadcastIP;
    double m_dPingBroadcastLastSent;
    
  protected:
  public:
    Device(std::string strDeviceName, HardwareType hwtType);
    ~Device();
    
    std::string deviceName();
    HardwareType hardwareType();
    
    void setUp(bool bUp);
    void setRunning(bool bRunning);
    
    std::string mac();
    void setIP(std::string strIP);
    std::string ip();
    void setBroadcastIP(std::string strIP);
    std::string broadcastIP();
    
    bool up();
    bool running();
    
    unsigned char* read(int& nLengthRead);
    
    Wire* wire();
    
    bool sendPingBroadcast(double dTime, double dInterval);
    bool sendPing(std::string strDestinationIP, std::string strDestinationMAC);
    
    unsigned short icmpHeaderChecksum(unsigned short* usData, int nLength);
    uint16_t ipHeaderChecksum(void* vdData, int nLength);
    
    static bool systemDeviceExists(std::string strDeviceName);
  };
}


#endif /* __DEVICE_H__ */
