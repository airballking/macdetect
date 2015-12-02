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


#include <cstdlib>

#include <macdetectd/Data.h>


int main(int argc, char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  macdetect::Data dtData;
  if(dtData.readVendors()) {
    macdetect::Data::Vendor vdApple = dtData.vendorForMAC("00:CD:FE:00:00:00");
    
    if(vdApple.bValid && vdApple.strVendor == "Apple, Inc.") {
      macdetect::Data::Vendor vdInvalid = dtData.vendorForMAC("FF:FF:FF:00:00:00");
      
      if(!vdInvalid.bValid) {
	nReturnvalue = EXIT_SUCCESS;
      }
    }
  }
  
  return nReturnvalue;
}
