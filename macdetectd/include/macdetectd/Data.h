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


#ifndef __DATA_H__
#define __DATA_H__


// System
#include <string>
#include <unistd.h>
#include <map>
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>


namespace macdetect {
  class Data {
  public:
    typedef struct {
      bool bValid;
      std::string strVendor;
    } Vendor;
    
  private:
    std::map<std::string, Vendor> m_mapVendorsCache;
    
  protected:
    bool tryReadVendors(std::string strPath);
    
  public:
    Data();
    ~Data();
    
    Vendor vendorForMAC(std::string strMAC);
    bool readVendors(std::string strFilepath = "");
    
    static std::string executablePath();
    static std::string dataPath();
  };
}


#endif /* __DATA_H__ */
