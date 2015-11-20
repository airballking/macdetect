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


#include <macdetect/Device.h>


namespace macdetect {
  Device::Device(std::string strDeviceName, HardwareType hwtType) : m_strDeviceName(strDeviceName), m_hwtType(hwtType), m_bUp(false), m_bRunning(false), m_wrWire(strDeviceName, ETH_FRAME_LEN), m_strMAC("") {
  }
  
  Device::~Device() {
  }
  
  std::string Device::deviceName() {
    return m_strDeviceName;
  }
  
  Device::HardwareType Device::hardwareType() {
    return m_hwtType;
  }
  
  std::string Device::mac() {
    if(m_strMAC == "") {
      struct ifreq ifrTemp;
      
      memset(&ifrTemp, 0, sizeof(ifrTemp));
      strcpy(ifrTemp.ifr_name, this->deviceName().c_str());
      ioctl(m_wrWire.socket(), SIOCGIFHWADDR, &ifrTemp);
      
      std::string strDeviceMAC = "";
      char carrBuffer[17];
      int nOffset = 0;
      for(int nI = 0; nI < 6; nI++) {
	sprintf(&(carrBuffer[nI + nOffset]), "%.2x", (unsigned char)ifrTemp.ifr_hwaddr.sa_data[nI]);
	nOffset++;
	
	if(nI < 5) {
	  nOffset++;
	  carrBuffer[nI + nOffset] = ':';
	}
      }
      
      strDeviceMAC = std::string(carrBuffer, 17);
      
      m_strMAC = strDeviceMAC;
    }
    
    return m_strMAC;
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
    nLengthRead = m_wrWire.defaultReadingLength();
    nLengthRead = m_wrWire.bufferedRead(&ucBuffer, nLengthRead);
    
    return ucBuffer;
  }
  
  Wire* Device::wire() {
    return &m_wrWire;
  }
}
