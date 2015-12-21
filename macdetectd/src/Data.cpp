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


#include <macdetectd/Data.h>


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
  
  bool Data::tryReadVendors(std::string strPath) {
    std::string strLine = "";
    std::ifstream ifOUI(strPath);
    
    if(ifOUI) {
      while(std::getline(ifOUI, strLine)) {
	std::string strMAC = strLine.substr(0, 6);
	std::string strVendor = strLine.substr(7);
	
	strMAC.erase(strMAC.find_last_not_of(" \n\r\t\"")+1);
	
	strVendor.erase(strVendor.find_last_not_of(" \n\r\t\"")+1);
	strVendor = strVendor.substr(1);
	
	m_mapVendorsCache[strMAC] = {true, strVendor};
      }
      
      return true;
    }
    
    return false;
  }
  
  bool Data::readVendors() {
    // Locations to look in for `oui.txt`.
    std::list<std::string> lstLocations = {Data::dataPath(),
					   "/usr/local/share/macdetect/data",
					   "/usr/share/macdetect/data"};
    
    for(std::string strLocation : lstLocations) {
      if(this->tryReadVendors(strLocation + "/oui.txt")) {
	return true;
      }
    }
    
    return false;
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
