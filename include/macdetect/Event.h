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


#ifndef __EVENT_H__
#define __EVENT_H__


// System
#include <string>


namespace macdetect {
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
