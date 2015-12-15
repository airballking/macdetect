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
      MACAddressDisappeared = 4,
      DeviceEvidenceChanged = 5
    } EventType;
    
  private:
    EventType m_tpType;
    
  protected:
  public:
    Event(EventType tpType);
    virtual ~Event();
    
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
    std::string m_strEvidenceField;
    std::string m_strEvidenceValue;
    std::string m_strEvidenceValueFormer;
    bool m_bUp;
    bool m_bRunning;
    
  protected:
  public:
    DeviceEvent(EventType tpType, std::string strDeviceName);
    ~DeviceEvent();
    
    std::string deviceName();
    
    void setStateChangeUp(bool bStateChangeUp);
    void setStateChangeRunning(bool bStateChangeUp);
    
    bool stateChangeUp();
    bool stateChangeRunning();
    
    void setEvidence(std::string strField, std::string strValue, std::string strValueFormer);
    void setEvidenceField(std::string strField);
    std::string evidenceField();
    void setEvidenceValue(std::string strValue);
    std::string evidenceValue();
    void setEvidenceValueFormer(std::string strValue);
    std::string evidenceValueFormer();
    
    void setUp(bool bUp);
    bool up();
    
    void setRunning(bool bRunning);
    bool running();
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
