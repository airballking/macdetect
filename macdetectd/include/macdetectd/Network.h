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


#ifndef __NETWORK_H__
#define __NETWORK_H__


// System
#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <regex>

// MAC Detect
#include <macdetectd/Device.h>
#include <macdetectd/Event.h>
#include <macdetectd/Wire.h>
#include <macdetectd/Data.h>
#include <macdetectd/RARP.h>


#define NSEC_PER_SEC 1000000000L


namespace macdetect {
  class Network {
  public:
    typedef enum {
      Whitelist = 0,
      Blacklist = 1
    } WhiteBlackListMode;
    
    typedef struct {
      std::string strMAC;
      std::string strDeviceName;
      std::string strHostName;
      std::string strIP;
      double dFirstSeen;
      double dLastSeen;
      double dLastUpdate;
    } MACEntity;
    
  private:
    int m_nSocketFDControl;
    bool m_bAutoManageDevices;
    std::list< std::shared_ptr<Device> > m_lstDevices;
    std::list<std::string> m_lstSystemDeviceNames;
    
    std::list< std::shared_ptr<Event> > m_lstEvents;
    std::list<MACEntity> m_lstMACSeen;
    
    WhiteBlackListMode m_wbmDevices;
    std::list<std::string> m_lstDeviceWhiteBlackList;
    
    double m_dMaxMACAge;
    double m_dUpdateInterval;
    double m_dPingBroadcastInterval;
    bool m_bShouldRun;
    bool m_bIgnoreDeviceMACs;
    
    Data m_dtData;
    RARP m_rpRARP;
    
  protected:
    void scheduleEvent(std::shared_ptr<Event> evSchedule);
    
  public:
    Network();
    ~Network();
    
    bool privilegesSuffice();
    
    void clearDeviceWhiteBlacklist();
    void setDeviceWhiteBlacklistMode(WhiteBlackListMode wbmSet);
    void addDeviceWhiteBlacklistEntry(std::string strPattern);
    bool deviceAllowed(std::string strDeviceName);
    
    void setIgnoreDeviceMACs(bool bIgnore);
    bool ignoreDeviceMACs();
    
    bool cycle();
    void shutdown();
    
    bool addDevice(std::string strDeviceName);
    bool removeDevice(std::string strDeviceName);
    
    void setAutoManageDevices(bool bAutoManageDevices);
    bool autoManageDevices();
    
    std::shared_ptr<Device> knownDevice(std::string strDeviceName);
    std::list< std::shared_ptr<Device> > knownDevices();
    
    Device::HardwareType deviceHardwareType(std::string strDeviceName);
    
    void maintainDeviceStatus(std::shared_ptr<Device> dvMaintain);
    
    bool systemDeviceNameExists(std::string strDeviceName);
    std::list<std::string> systemDeviceNames();
    
    std::list< std::shared_ptr<Event> > events();
    
    std::string mac(unsigned char* ucMAC);
    
    double time();
    
    void detectNetworkActivity();
    
    bool ipAllowed(std::string strIP);
    bool macAllowed(std::string strMAC);
    bool addMAC(std::string strMACAddress, std::string strDeviceName);
    void removeMAC(std::string strDeviceName, std::string strMAC);
    
    double macLastSeen(std::string strMAC, std::string strDeviceName);
    std::list<Network::MACEntity> knownMACs();
    
    void setMACMaxAge(double dMaxAge);
    double macMaxAge();
    
    std::string readableMACIdentifier(std::string strMAC, bool bDefaultToMAC = true);
  };
}


#endif /* __NETWORK_H__ */
