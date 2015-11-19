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


#include <macdetect/Event.h>


namespace macdetect {
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
