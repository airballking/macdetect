#include <detect/Wire.h>


namespace detect {
  Wire::Wire(std::string strDeviceName, int nDefaultReadingLength) {
    this->setSocket(::socket(AF_PACKET, SOCK_RAW, htons(0x0800)));
    this->setDefaultReadingLength(nDefaultReadingLength);
    
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    
    strncpy(ifr.ifr_name, strDeviceName.c_str(), sizeof(ifr.ifr_name) - 1);
    ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = '\0';
    
    ioctl(m_nSocket, SIOCGIFINDEX, &ifr);
    
    struct sockaddr_ll socket_address;
    memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sll_family = AF_PACKET;
    socket_address.sll_ifindex = ifr.ifr_ifindex;
    socket_address.sll_protocol = htons(0x0800);
    
    bind(this->socket(), (sockaddr*)&socket_address, sizeof(socket_address));
  }
  
  Wire::~Wire() {
  }
  
  bool Wire::write(unsigned char* ucBuffer, unsigned int unLength) {
    struct sockaddr_ll socket_address;
    memset(&socket_address, 0, this->defaultReadingLength());
    socket_address.sll_family = AF_PACKET;
    socket_address.sll_protocol = htons(0x0800);
    
    if(sendto(m_nSocket,
	      ucBuffer,
	      unLength,
	      0,
	      (struct sockaddr*)&socket_address,
	      sizeof(socket_address)) == -1) {
      std::cout << "Fail (length " << unLength << "): " << strerror(errno) << std::endl;
      
      m_nSocket = -1;
      return false;
    }
    
    return true;
  }
}
