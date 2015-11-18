#include <detect/Event.h>


namespace detect {
  /*
    Base event class
  */
  Event::Event(EventType tpType) : m_tpType(tpType) {
  }
  
  Event::~Event() {
  }
  
  Event::EventType Event::type() {
    return m_tpType;
  }
  
  /*
    Device related event class
  */
  DeviceEvent::DeviceEvent(EventType tpType, std::string strDeviceName) : m_strDeviceName(strDeviceName), m_bStateChangeUp(false), m_bStateChangeRunning(false), Event(tpType) {
  }
  
  DeviceEvent::~DeviceEvent() {
  }
  
  void DeviceEvent::setStateChangeUp(bool bStateChangeUp) {
    m_bStateChangeUp = bStateChangeUp;
  }
  
  void DeviceEvent::setStateChangeRunning(bool bStateChangeRunning) {
    m_bStateChangeRunning = bStateChangeRunning;
  }
  
  bool DeviceEvent::stateChangeUp() {
    return m_bStateChangeUp;
  }
  
  bool DeviceEvent::stateChangeRunning() {
    return m_bStateChangeRunning;
  }
  
  std::string DeviceEvent::deviceName() {
    return m_strDeviceName;
  }
  
  /*
    MAC related event class
  */
  MACEvent::MACEvent(EventType tpType, std::string strDeviceName, std::string strMAC) : m_strMAC(strMAC), DeviceEvent(tpType, strDeviceName) {
  }
  
  MACEvent::~MACEvent() {
  }
  
  std::string MACEvent::macAddress() {
    return m_strMAC;
  }
}
