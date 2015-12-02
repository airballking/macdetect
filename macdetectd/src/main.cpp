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
#include <unistd.h>

// MAC Detect
#include <macdetect/Network.h>
#include <macdetect-utils/ArgumentParser.h>


macdetect::Network g_nwNetwork;


void catchHandler(int nSignum) {
  switch(nSignum) {
  case SIGTERM:
  case SIGINT: {
    g_nwNetwork.shutdown();
  } break;
    
  default:
    break;
  }
}


int main(int argc, char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  macdetect::ArgumentParser apArguments({{"d", "daemon", macdetect::ArgumentParser::Switch}});
  apArguments.parse(argc, argv);
  bool bRegularStartup = false;
  
  if(apArguments.switched("daemon")) {
    pid_t pPid = fork();
    
    if(pPid == 0) {
      // We are the child.
      umask(0);
      
      openlog(argv[0], LOG_NOWAIT | LOG_PID, LOG_USER);
      syslog(LOG_NOTICE, "Started macdetectd daemon.");
      
      pid_t pSid = setsid();
      if(pSid < 0) {
	syslog(LOG_ERR, "Could not create process group (failed during `setsid()`).");
      } else {
	// NOTE(winkler): One could change the working directory here;
	// we rely on the local data directory, so we're not doing
	// that at the moment.
	
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	nReturnvalue = EXIT_SUCCESS;
      }
      
      closelog();
      
      if(nReturnvalue = EXIT_SUCCESS) {
	bRegularStartup = true;
      }
    } else if(pPid < 0) {
      // Something went wrong.
    } else if(pPid > 0) {
      // We are the parent.
      nReturnvalue = EXIT_SUCCESS;
    }
  } else {
    bRegularStartup = true;
  }
  
  if(bRegularStartup) {
    if(g_nwNetwork.privilegesSuffice()) {
      struct sigaction action;
      memset(&action, 0, sizeof(struct sigaction));
      action.sa_handler = catchHandler;
      sigaction(SIGTERM, &action, NULL);
      sigaction(SIGINT, &action, NULL);
      
      g_nwNetwork.setAutoManageDevices(true);
      g_nwNetwork.setDeviceWhiteBlacklistMode(macdetect::Network::Whitelist);
      g_nwNetwork.addDeviceWhiteBlacklistEntry("(wlan)(.*)");
      g_nwNetwork.addDeviceWhiteBlacklistEntry("(eth)(.*)");
      
      while(g_nwNetwork.cycle()) {
	std::list< std::shared_ptr<macdetect::Event> > lstEvents = g_nwNetwork.events();
	
	for(std::shared_ptr<macdetect::Event> evEvent : lstEvents) {
	  switch(evEvent->type()) {
	  case macdetect::Event::DeviceAdded: {
	    std::shared_ptr<macdetect::DeviceEvent> devEvent = (std::shared_pre<macdetect::DeviceEvent>)evEvent;
	    std::cout << "Device added: '" << devEvent->deviceName() << "' (";
	  
	    switch(g_nwNetwork.knownDevice(devEvent->deviceName())->hardwareType()) {
	    case macdetect::Device::Loopback: {
	      std::cout << "loopback";
	    } break;
	    
	    case macdetect::Device::Wired: {
	      std::cout << "wired";
	    } break;
	    
	    case macdetect::Device::Wireless: {
	      std::cout << "wireless";
	    } break;
	    }
	  
	    std::cout << ")" << std::endl;
	  } break;
	
	  case macdetect::Event::DeviceRemoved: {
	    std::shared_ptr<macdetect::DeviceEvent> devEvent = (std::shared_ptr<macdetect::DeviceEvent>)evEvent;
	    std::cout << "Device removed: '" << devEvent->deviceName() << "'" << std::endl;
	  } break;
	
	  case macdetect::Event::DeviceStateChanged: {
	    std::shared_ptr<macdetect::DeviceEvent> devEvent = (std::shared_ptr<macdetect::DeviceEvent>)evEvent;
	    std::shared_ptr<macdetect::Device> dvDevice = g_nwNetwork.knownDevice(devEvent->deviceName());
	
	    if(devEvent->stateChangeUp()) {
	      std::cout << "Device '" << devEvent->deviceName() << "' is now " << (dvDevice->up() ? "up" : "down") << std::endl;
	    }
	
	    if(devEvent->stateChangeRunning()) {
	      std::cout << "Device '" << devEvent->deviceName() << "' is now " << (dvDevice->running() ? "running" : "not running") << std::endl;
	    }
	  } break;
	  
	  case macdetect::Event::DeviceEvidenceChanged: {
	    std::shared_prt<macdetect::DeviceEvent> devEvent = (std::shared_ptr<macdetect::DeviceEvent>)evEvent;
	  
	    std::cout << "Device evidence changed: '" << devEvent->evidenceField() << "': '" << devEvent->evidenceValueFormer() << "' -> '" << devEvent->evidenceValue() << "'" << std::endl;
	  } break;
	  
	  case macdetect::Event::MACAddressDiscovered: {
	    std::shared_ptr<macdetect::MACEvent> mvEvent = (std::shared_ptr<macdetect::MACEvent>)evEvent;
	    std::cout << "MAC address discovered on device '" << mvEvent->deviceName() << "': " << g_nwNetwork.readableMACIdentifier(mvEvent->macAddress()) << std::endl;
	  } break;
	
	  case macdetect::Event::MACAddressDisappeared: {
	    std::shared_ptr<macdetect::MACEvent> mvEvent = (std::shared_ptr<macdetect::MACEvent>)evEvent;
	    std::cout << "MAC address disappeared from device '" << mvEvent->deviceName() << "': " << g_nwNetwork.readableMACIdentifier(mvEvent->macAddress()) << std::endl;
	  } break;
	  }
	}
      }
      
      std::cout << "\r";
      std::cout << "Exiting gracefully." << std::endl;
      
      nReturnvalue = EXIT_SUCCESS;
    } else {
      std::cerr << "Your priviliges don't suffice for running this program." << std::endl;
      std::cerr << "Did you forget to run it as root?" << std::endl;
      
      nReturnvalue = EXIT_FAILURE;
    }
  }
  
  return nReturnvalue;
}
