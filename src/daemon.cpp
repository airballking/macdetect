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
#include <cstdlib>
#include <signal.h>

// Detect
#include <macdetect/Network.h>
#include <macdetect/Server.h>


//macdetect::Network g_nwNetwork;
macdetect::Server g_srvServer;


void catchHandler(int nSignum) {
  switch(nSignum) {
  case SIGTERM:
  case SIGINT: {
    //g_nwNetwork.shutdown();
    g_srvServer.shutdown();
  } break;
    
  default:
    break;
  }
}


int main(int argc, char** argv) {
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = catchHandler;
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGINT, &action, NULL);
  
  unsigned short usPort = 7090;
  std::list<std::string> lstSystemDeviceNames = g_srvServer.systemDeviceNames();
  
  for(std::string strSystemDeviceName : lstSystemDeviceNames) {
    std::cout << "Serving on " << strSystemDeviceName << ":" << usPort << std::endl;
    
    g_srvServer.serve(strSystemDeviceName, usPort);
  }
  
  while(g_srvServer.cycle()) {
    usleep(10);
    // ...
  }
  
  std::cout << "\rExiting gracefully." << std::endl;
  
  // if(g_nwNetwork.privilegesSuffice()) {
  //   struct sigaction action;
  //   memset(&action, 0, sizeof(struct sigaction));
  //   action.sa_handler = catchHandler;
  //   sigaction(SIGTERM, &action, NULL);
  //   sigaction(SIGINT, &action, NULL);
  
  //   g_nwNetwork.setAutoManageDevices(true);
  //   g_nwNetwork.setDeviceWhiteBlacklistMode(macdetect::Network::Whitelist);
  //   g_nwNetwork.addDeviceWhiteBlacklistEntry("(wlan)(.*)");
  //   g_nwNetwork.addDeviceWhiteBlacklistEntry("(eth)(.*)");
  
  //   while(g_nwNetwork.cycle()) {
  //     std::list<macdetect::Event*> lstEvents = g_nwNetwork.events();
    
  //     for(macdetect::Event* evEvent : lstEvents) {
  // 	switch(evEvent->type()) {
  // 	case macdetect::Event::DeviceAdded: {
  // 	  macdetect::DeviceEvent* devEvent = (macdetect::DeviceEvent*)evEvent;
  // 	  std::cout << "Device added: '" << devEvent->deviceName() << "' (";
	  
  // 	  switch(g_nwNetwork.knownDevice(devEvent->deviceName())->hardwareType()) {
  // 	  case macdetect::Device::Loopback: {
  // 	    std::cout << "loopback";
  // 	  } break;
	    
  // 	  case macdetect::Device::Wired: {
  // 	    std::cout << "wired";
  // 	  } break;
	    
  // 	  case macdetect::Device::Wireless: {
  // 	    std::cout << "wireless";
  // 	  } break;
  // 	  }
	  
  // 	  std::cout << ")" << std::endl;
  // 	} break;
	
  // 	case macdetect::Event::DeviceRemoved: {
  // 	  macdetect::DeviceEvent* devEvent = (macdetect::DeviceEvent*)evEvent;
  // 	  std::cout << "Device removed: '" << devEvent->deviceName() << "'" << std::endl;
  // 	} break;
	
  // 	case macdetect::Event::DeviceStateChanged: {
  // 	  macdetect::DeviceEvent* devEvent = (macdetect::DeviceEvent*)evEvent;
  // 	  macdetect::Device* dvDevice = g_nwNetwork.knownDevice(devEvent->deviceName());
	
  // 	  if(devEvent->stateChangeUp()) {
  // 	    std::cout << "Device '" << devEvent->deviceName() << "' is now " << (dvDevice->up() ? "up" : "down") << std::endl;
  // 	  }
	
  // 	  if(devEvent->stateChangeRunning()) {
  // 	    std::cout << "Device '" << devEvent->deviceName() << "' is now " << (dvDevice->running() ? "running" : "not running") << std::endl;
  // 	  }
  // 	} break;
	  
  // 	case macdetect::Event::DeviceEvidenceChanged: {
  // 	  macdetect::DeviceEvent* devEvent = (macdetect::DeviceEvent*)evEvent;
	  
  // 	  std::cout << "Device evidence changed: '" << devEvent->evidenceField() << "': '" << devEvent->evidenceValueFormer() << "' -> '" << devEvent->evidenceValue() << "'" << std::endl;
  // 	} break;
	  
  // 	case macdetect::Event::MACAddressDiscovered: {
  // 	  macdetect::MACEvent* mvEvent = (macdetect::MACEvent*)evEvent;
  // 	  std::cout << "MAC address discovered on device '" << mvEvent->deviceName() << "': " << g_nwNetwork.readableMACIdentifier(mvEvent->macAddress()) << std::endl;
  // 	} break;
	
  // 	case macdetect::Event::MACAddressDisappeared: {
  // 	  macdetect::MACEvent* mvEvent = (macdetect::MACEvent*)evEvent;
  // 	  std::cout << "MAC address disappeared from device '" << mvEvent->deviceName() << "': " << g_nwNetwork.readableMACIdentifier(mvEvent->macAddress()) << std::endl;
  // 	} break;
  // 	}
  //     }
  //   }
  
  //   std::cout << "\r";
  //   std::cout << "Exiting gracefully." << std::endl;
  
  //   return EXIT_SUCCESS;
  // } else {
  //   std::cerr << "Your priviliges don't suffice for running this program." << std::endl;
  //   std::cerr << "Did you forget to run it as root?" << std::endl;
    
  //   return EXIT_FAILURE;
  // }
}
