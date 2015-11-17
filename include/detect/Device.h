#ifndef __DEVICE_H__
#define __DEVICE_H__


#include <string>


namespace detect {
  class Device {
  public:
    typedef enum {
      Loopback = 0,
      Wired = 1,
      Wireless = 2
    } HardwareType;
    
  private:
    int m_nSocketFD;
    std::string m_strDeviceName;
    HardwareType m_hwtType;
    
  protected:
  public:
    Device(std::string strDeviceName, HardwareType hwtType);
    ~Device();
    
    std::string deviceName();
    HardwareType hardwareType();
    
    static bool systemDeviceExists(std::string strDeviceName);
  };
}


#endif /* __DEVICE_H__ */
