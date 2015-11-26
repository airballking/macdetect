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


#ifndef __DATA_H__
#define __DATA_H__


// System
#include <string>
#include <unistd.h>
#include <map>
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
  public:
    Data();
    ~Data();
    
    Vendor vendorForMAC(std::string strMAC);
    bool readVendors();
    
    static std::string executablePath();
    static std::string dataPath();
  };
}


#endif /* __DATA_H__ */
