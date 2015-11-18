#include <cstdlib>
#include <signal.h>

#include <detect/Network.h>


detect::Network g_nwNetwork;


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
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = catchHandler;
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGINT, &action, NULL);
  
  g_nwNetwork.setAutoManageDevices(true);
  
  while(g_nwNetwork.cycle()) {
    std::list<detect::Event*> lstEvents = g_nwNetwork.events();
    
    for(detect::Event* evEvent : lstEvents) {
      switch(evEvent->type()) {
      case detect::Event::DeviceAdded: {
	detect::DeviceEvent* devEvent = (detect::DeviceEvent*)evEvent;
	std::cout << "Device added: '" << devEvent->deviceName() << "'" << std::endl;
      } break;
	
      case detect::Event::DeviceRemoved: {
	detect::DeviceEvent* devEvent = (detect::DeviceEvent*)evEvent;
	std::cout << "Device removed: '" << devEvent->deviceName() << "'" << std::endl;
      } break;
	
      case detect::Event::DeviceStateChanged: {
	detect::DeviceEvent* devEvent = (detect::DeviceEvent*)evEvent;
	detect::Device* dvDevice = g_nwNetwork.knownDevice(devEvent->deviceName());
	
	if(devEvent->stateChangeUp()) {
	  std::cout << "Device '" << devEvent->deviceName() << "' is now " << (dvDevice->up() ? "up" : "down") << std::endl;
	}
	
	if(devEvent->stateChangeRunning()) {
	  std::cout << "Device '" << devEvent->deviceName() << "' is now " << (dvDevice->running() ? "running" : "not running") << std::endl;
	}
      } break;
	
      case detect::Event::MACAddressDiscovered: {
	detect::MACEvent* mvEvent = (detect::MACEvent*)evEvent;
	std::cout << "MAC address discovered on device '" << mvEvent->deviceName() << "': '" << mvEvent->macAddress() << "'" << std::endl;
      } break;
	
      case detect::Event::MACAddressDisappeared: {
	detect::MACEvent* mvEvent = (detect::MACEvent*)evEvent;
	std::cout << "MAC address disappeared from device '" << mvEvent->deviceName() << "': '" << mvEvent->macAddress() << "'" << std::endl;
      } break;
      }
    }
  }
  
  std::cout << "\r";
  std::cout << "Exiting gracefully." << std::endl;
  
  return EXIT_SUCCESS;
}
