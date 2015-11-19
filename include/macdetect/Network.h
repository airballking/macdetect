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
#include <arpa/inet.h>
#include <linux/wireless.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <regex>

// Detect
#include <macdetect/Device.h>
#include <macdetect/Event.h>
#include <macdetect/Wire.h>
#include <macdetect/Headers.h>
#include <macdetect/Data.h>


#define NSEC_PER_SEC 1000000000L


namespace macdetect {
  class Network {
  public:
    typedef enum {
      Whitelist = 0,
      Blacklist = 1
    } WhiteBlackListMode;
    
  private:
    typedef struct {
      std::string strMAC;
      std::string strDeviceName;
      double dLastSeen;
    } MACEntity;
    
    int m_nSocketFDControl;
    bool m_bAutoManageDevices;
    std::list<Device*> m_lstDevices;
    std::list<std::string> m_lstSystemDeviceNames;
    
    std::list<Event*> m_lstEvents;
    std::list<MACEntity> m_lstMACSeen;
    
    WhiteBlackListMode m_wbmDevices;
    std::list<std::string> m_lstDeviceWhiteBlackList;
    
    double m_dMaxMACAge;
    bool m_bShouldRun;
    
    Data m_dtData;
    
  protected:
    void scheduleEvent(Event* evSchedule);
    
  public:
    Network();
    ~Network();
    
    bool privilegesSuffice();
    
    void clearDeviceWhiteBlacklist();
    void setDeviceWhiteBlacklistMode(WhiteBlackListMode wbmSet);
    void addDeviceWhiteBlacklistEntry(std::string strPattern);
    bool deviceAllowed(std::string strDeviceName);
    
    bool cycle();
    void shutdown();
    
    bool addDevice(std::string strDeviceName);
    bool removeDevice(std::string strDeviceName);
    
    void setAutoManageDevices(bool bAutoManageDevices);
    bool autoManageDevices();
    
    Device* knownDevice(std::string strDeviceName);
    std::list<Device*> knownDevices();
    
    Device::HardwareType deviceHardwareType(std::string strDeviceName);
    
    void maintainDeviceStatus(Device* dvMaintain);
    
    bool systemDeviceNameExists(std::string strDeviceName);
    std::list<std::string> systemDeviceNames();
    
    std::list<Event*> events();
    
    std::string mac(unsigned char* ucMAC);
    
    double time();
    
    void detectNetworkActivity();
    void addMAC(std::string strMACAddress, std::string strDeviceName);
    void removeMAC(std::string strDeviceName, std::string strMAC);
    
    double macLastSeen(std::string strMAC, std::string strDeviceName);
    
    void setMACMaxAge(double dMaxAge);
    double macMaxAge();
    
    std::string readableMACIdentifier(std::string strMAC);
  };
}


#endif /* __NETWORK_H__ */
