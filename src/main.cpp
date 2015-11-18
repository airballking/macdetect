#include <cstdlib>

#include <detect/Network.h>


int main(int argc, char** argv) {
  detect::Network nwNetwork;
  
  nwNetwork.setAutoManageDevices(true);
  
  while(nwNetwork.cycle()) {
    std::list<detect::Event*> lstEvents = nwNetwork.events();
    
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
	std::cout << "Device state changed: '" << devEvent->deviceName() << "'" << std::endl;
      } break;
      }
    }
  }
  
  return EXIT_SUCCESS;
}
