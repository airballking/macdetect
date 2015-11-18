#include <detect/Wire.h>


namespace detect {
  Wire::Wire(std::string strDeviceName, int nDefaultReadingLength) : SocketReader(strDeviceName) {
    this->setSocket(::socket(AF_PACKET, SOCK_RAW, htons(0x0800)));
    this->setDefaultReadingLength(nDefaultReadingLength);
    
    setsockopt(this->socket(), SOL_SOCKET, SO_BINDTODEVICE, strDeviceName.c_str(), strDeviceName.length());
  }
  
  Wire::~Wire() {
  }
  
  bool Wire::write(unsigned char* ucBuffer, unsigned int unLength) {
    struct sockaddr_ll socket_address;
    memset(&socket_address, 0, 1514);//sizeof(socket_address));
    socket_address.sll_family = AF_PACKET;
    socket_address.sll_protocol = htons(0x0003); // ETH_P_ALL
    
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
