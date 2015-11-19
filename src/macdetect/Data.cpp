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


#include <macdetect/Data.h>


namespace macdetect {
  Data::Data() {
  }
  
  Data::~Data() {
  }
  
  Data::Vendor Data::vendorForMAC(std::string strMAC) {
    strMAC = strMAC.substr(0, 2) + strMAC.substr(3, 2) + strMAC.substr(6, 2);
    std::transform(strMAC.begin(), strMAC.end(),strMAC.begin(), ::toupper);
    
    if(m_mapVendorsCache.find(strMAC) != m_mapVendorsCache.end()) {
      return m_mapVendorsCache[strMAC];
    } else {
      return {false, ""};
    }
  }
  
  bool Data::readVendors() {
    std::ifstream ifOUI(Data::dataPath() + "/oui.txt");
    std::string strLine = "";
    
    while(std::getline(ifOUI, strLine)) {
      std::istringstream iss(strLine);
      
      std::string strMAC;
      std::string strVendor;
      getline(iss, strMAC, ',');
      getline(iss, strVendor, ',');
      
      strMAC.erase(strMAC.find_last_not_of(" \n\r\t\"")+1);
      
      strVendor.erase(strVendor.find_last_not_of(" \n\r\t\"")+1);
      strVendor = strVendor.substr(1);
      
      m_mapVendorsCache[strMAC] = {true, strVendor};
    }
  }
  
  std::string Data::executablePath() {
    char carrBuffer[1024];
    int nBytes = readlink("/proc/self/exe", carrBuffer, 1024);
    std::string strPath = "";
    
    if(nBytes > -1) {
      strPath = std::string(carrBuffer, nBytes);
      strPath = strPath.substr(0, strPath.find_last_of("/"));
    }
    
    return strPath;
  }
  
  std::string Data::dataPath() {
    return Data::executablePath() + "/../data";
  }
}
