#ifndef __NETWORK_H__
#define __NETWORK_H__


#include <iostream>
#include <string>
#include <list>
#include <algorithm>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/wireless.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <detect/Device.h>
#include <detect/Event.h>


namespace detect {
  class Network {
  private:
    int m_nSocketFDControl;
    bool m_bAutoManageDevices;
    std::list<Device*> m_lstDevices;
    std::list<std::string> m_lstSystemDeviceNames;
    
    std::list<Event*> m_lstEvents;
    
  protected:
    void scheduleEvent(Event* evSchedule);
    
  public:
    Network();
    ~Network();
    
    bool cycle();
    
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
  };
}


#endif /* __NETWORK_H__ */
