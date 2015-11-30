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


#include <macdetectd/Event.h>


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
  
  void DeviceEvent::setEvidence(std::string strField, std::string strValue, std::string strValueFormer) {
    this->setEvidenceField(strField);
    this->setEvidenceValue(strValue);
    this->setEvidenceValueFormer(strValueFormer);
  }
  
  void DeviceEvent::setEvidenceField(std::string strField) {
    m_strEvidenceField = strField;
  }
  
  std::string DeviceEvent::evidenceField() {
    return m_strEvidenceField;
  }
  
  void DeviceEvent::setEvidenceValue(std::string strValue) {
    m_strEvidenceValue = strValue;
  }
  
  std::string DeviceEvent::evidenceValue() {
    return m_strEvidenceValue;
  }
  
  void DeviceEvent::setEvidenceValueFormer(std::string strValue) {
    m_strEvidenceValueFormer = strValue;
  }
  
  std::string DeviceEvent::evidenceValueFormer() {
    return m_strEvidenceValueFormer;
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
