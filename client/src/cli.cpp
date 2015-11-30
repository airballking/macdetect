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


// System
#include <iostream>
#include <unistd.h>

// MAC detect
#include <macdetect-client/MDClient.h>


int main(int argc, char** argv) {
  macdetect_client::MDClient mdcClient;
  
  if(mdcClient.connect("127.0.0.1")) {
    std::list<std::string> lstDeviceNames = mdcClient.deviceNames();
    for(std::string strDeviceName : lstDeviceNames) {
      std::cout << "Enabling stream for '" << strDeviceName << "'" << std::endl;
      if(mdcClient.enableStream(strDeviceName)) {
	std::cout << " - OK" << std::endl;
      } else {
	std::cout << " - Failed" << std::endl;
      }
    }
    
    std::cout << std::endl << "Known MAC addresses:" << std::endl;
    std::list<std::string> lstMACAddresses = mdcClient.knownMACAddresses();
    for(std::string strMACAddress : lstMACAddresses) {
      std::cout << " - " << strMACAddress << std::endl;
    }
    
    while(true) {
      std::shared_ptr<macdetect::Value> valInfo = mdcClient.info();
    }
    
    return EXIT_SUCCESS;
  } else {
    std::cerr << "Fail." << std::endl;
    
    return EXIT_FAILURE;
  }
}
