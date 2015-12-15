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


// System
#include <iostream>
#include <unistd.h>

// MAC detect
#include <macdetect-client/MDClient.h>


int main(int argc, char** argv) {
  macdetect_client::MDClient mdcClient;
  
  // if(mdcClient.connect("127.0.0.1")) {
  //   std::list<std::string> lstDeviceNames = mdcClient.deviceNames();
  //   for(std::string strDeviceName : lstDeviceNames) {
  //     std::cout << "Enabling stream for '" << strDeviceName << "'" << std::endl;
  //     if(mdcClient.enableStream(strDeviceName)) {
  // 	std::cout << " - OK" << std::endl;
  //     } else {
  // 	std::cout << " - Failed" << std::endl;
  //     }
  //   }
    
  //   std::cout << std::endl << "Known MAC addresses:" << std::endl;
  //   std::list<std::string> lstMACAddresses = mdcClient.knownMACAddresses();
  //   for(std::string strMACAddress : lstMACAddresses) {
  //     std::cout << " - " << strMACAddress << std::endl;
  //   }
    
  //   while(true) {
  //     bool bDisconnected;
      
  //     // TODO(winkler): Honor `bDisconnected` here.
      
  //     std::shared_ptr<macdetect::Value> valInfo = mdcClient.receive(bDisconnected);
  //   }
    
  //   return EXIT_SUCCESS;
  // } else {
  //   std::cerr << "Fail." << std::endl;
    
  //   return EXIT_FAILURE;
  // }
}
