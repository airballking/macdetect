#include <detect/Device.h>


namespace detect {
  Device::Device(std::string strDeviceName, HardwareType hwtType) : m_strDeviceName(strDeviceName), m_hwtType(hwtType), m_bUp(false), m_bRunning(false), m_wrWire(strDeviceName, 1514) {
  }
  
  Device::~Device() {
  }
  
  std::string Device::deviceName() {
    return m_strDeviceName;
  }
  
  Device::HardwareType Device::hardwareType() {
    return m_hwtType;
  }
  
  void Device::setUp(bool bUp) {
    m_bUp = bUp;
  }
  
  void Device::setRunning(bool bRunning) {
    m_bRunning = bRunning;
  }
  
  bool Device::up() {
    return m_bUp;
  }
  
  bool Device::running() {
    return m_bRunning;
  }
  
  unsigned char* Device::read(int& nLengthRead) {
    unsigned char* ucBuffer;
    nLengthRead = 1514;
    nLengthRead = m_wrWire.bufferedRead(&ucBuffer, nLengthRead);
    
    return ucBuffer;
  }
}
