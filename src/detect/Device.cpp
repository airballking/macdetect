#include <detect/Device.h>


namespace detect {
  Device::Device(std::string strDeviceName, HardwareType hwtType) {
    m_strDeviceName = strDeviceName;
  }
  
  Device::~Device() {
  }
  
  std::string Device::deviceName() {
    return m_strDeviceName;
  }
  
  Device::HardwareType Device::hardwareType() {
    return m_hwtType;
  }
}
