#include <cstdlib>

#include <detect/Network.h>


int main(int argc, char** argv) {
  detect::Network nwNetwork;
  
  nwNetwork.setAutoManageDevices(true);
  
  while(nwNetwork.cycle()) {
    // ...
  }
  
  return EXIT_SUCCESS;
}
