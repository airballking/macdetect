#ifndef __EVENT_H__
#define __EVENT_H__


#include <detect/Device.h>


namespace detect {
  /*
    Base Events
  */
  class Event {
  public:
    typedef enum {
      DeviceAdded = 0,
      DeviceRemoved = 1,
      DeviceStateChanged = 2,
      MACAddressDiscovered = 3,
      MACAddressDisappeared = 4
    } EventType;
    
  private:
    EventType m_tpType;
    
  protected:
  public:
    Event(EventType tpType);
    ~Event();
    
    EventType type();
    void setEventType(EventType tpSet);
  };
  
  /*
    Device Events
  */
  class DeviceEvent : public Event {
  private:
    std::string m_strDeviceName;
    bool m_bStateChangeUp;
    bool m_bStateChangeRunning;
    
  protected:
  public:
    DeviceEvent(EventType tpType, std::string strDeviceName);
    ~DeviceEvent();
    
    std::string deviceName();
    
    void setStateChangeUp(bool bStateChangeUp);
    void setStateChangeRunning(bool bStateChangeUp);
    
    bool stateChangeUp();
    bool stateChangeRunning();
  };
  
  /*
    MAC Events
  */
  class MACEvent : public DeviceEvent {
  private:
    std::string m_strMAC;
    
  protected:
  public:
    MACEvent(EventType tpType, std::string strDeviceName, std::string strMAC);
    ~MACEvent();
    
    std::string macAddress();
  };
}


#endif /* __EVENT_H__ */
