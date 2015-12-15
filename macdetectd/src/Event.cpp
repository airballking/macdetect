// macdetect, a daemon and clients for detecting MAC addresses
// Copyright (C) 2015 Jan Winkler <jan.winkler.84@gmail.com>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

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
  DeviceEvent::DeviceEvent(EventType tpType, std::string strDeviceName) : m_strDeviceName(strDeviceName), m_bStateChangeUp(false), m_bStateChangeRunning(false), Event(tpType), m_strEvidenceField(""), m_strEvidenceValue(""), m_strEvidenceValueFormer("") {
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
  
  void DeviceEvent::setUp(bool bUp) {
    m_bUp = bUp;
  }
  
  bool DeviceEvent::up() {
    return m_bUp;
  }
  
  void DeviceEvent::setRunning(bool bRunning) {
    m_bRunning = bRunning;
  }
  
  bool DeviceEvent::running() {
    return m_bRunning;
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
