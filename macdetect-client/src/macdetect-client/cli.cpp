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
    
    std::list<std::string> lstMACAddresses = mdcClient.knownMACAddresses();
    for(std::string strMACAddress : lstMACAddresses) {
      std::cout << " - " << strMACAddress << std::endl;
    }
    
    while(true) {
      macdetect::Packet* pktInfo = mdcClient.info();
      
      if(pktInfo) {
	delete pktInfo;
      }
    }
    
    return EXIT_SUCCESS;
  } else {
    std::cerr << "Fail." << std::endl;
    
    return EXIT_FAILURE;
  }
  
  //  macdetect::Client cliClient;
  
  //if(cliClient.connect("127.0.0.1")) {
    // macdetect::Packet* pktPacket = new macdetect::Packet("request", "devices-list");
    // //macdetect::Packet* pktPacket = new macdetect::Packet("request", "known-mac-addresses");
    // //macdetect::Packet* pktPacket = new macdetect::Packet("request", "enable-stream");
    // //pktPacket->add(new macdetect::Packet("device-name", "lo"));
    
    // cliClient.sendPacket(pktPacket);
    
    // bool bGoon = true;
    // while(bGoon) {
    //   macdetect::Packet* pktReceived = cliClient.receivePacket();
      
    //   if(pktReceived) {
    // 	if(pktReceived->key() == "response") {
    // 	  if(pktReceived->value() == "devices-list") {
    // 	    for(macdetect::Packet* pktDevice : pktReceived->subPackets()) {
    // 	      std::string strDeviceName = pktDevice->value();
	      
    // 	      macdetect::Packet* pktEnableStream = new macdetect::Packet("request", "enable-stream");
    // 	      pktEnableStream->add(new macdetect::Packet("device-name", strDeviceName));
    // 	      cliClient.sendPacket(pktEnableStream);
	      
    // 	      delete pktEnableStream;
    // 	    }
    // 	  } else if(pktReceived->value() == "enable-stream") {
    // 	    std::cout << "Stream enabled: " << pktReceived->sub("device-name")->value() << std::endl;
    // 	  } else {
    // 	    pktReceived->print();
    // 	  }
    // 	} else {
    // 	  pktReceived->print();
    // 	}
	
    // 	delete pktReceived;
    //   }
    // }
}
