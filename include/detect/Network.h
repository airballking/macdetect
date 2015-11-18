#ifndef __NETWORK_H__
#define __NETWORK_H__


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

#include <detect/Device.h>
#include <detect/Event.h>
#include <detect/Wire.h>
#include <detect/Headers.h>


#define NSEC_PER_SEC 1000000000L


namespace detect {
  class Network {
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
    
    double m_dMaxMACAge;
    
    bool m_bShouldRun;
    
  protected:
    void scheduleEvent(Event* evSchedule);
    
  public:
    Network();
    ~Network();
    
    bool privilegesSuffice();
    
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
  };
}


#endif /* __NETWORK_H__ */
