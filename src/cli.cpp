// System
#include <iostream>
#include <unistd.h>

// MAC detect
#include <macdetect/Client.h>


int main(int argc, char** argv) {
  macdetect::Client cliClient;
  
  if(cliClient.connect("127.0.0.1")) {
    //macdetect::Packet* pktPacket = new macdetect::Packet("request", "devices-list");
    macdetect::Packet* pktPacket = new macdetect::Packet("request", "known-mac-addresses");
    cliClient.sendPacket(pktPacket);
    
    bool bGoon = true;
    while(bGoon) {
      macdetect::Packet* pktReceived = cliClient.receivePacket();
      
      if(pktReceived) {
	pktReceived->print();
	
	delete pktReceived;
      }
    }
    
    return EXIT_SUCCESS;
  } else {
    std::cerr << "Fail." << std::endl;
    
    return EXIT_FAILURE;
  }
}
