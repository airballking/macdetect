#ifndef __WIRE_H__
#define __WIRE_H__


#include <iostream>
#include <string>
#include <memory.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

#include <detect/SocketReader.h>


namespace detect {
  class Wire : public SocketReader {
  private:
  public:
    Wire(std::string strDeviceName, int nDefaultReadingLength);
    ~Wire();
    
    bool write(unsigned char* ucBuffer, unsigned int unLength);
  };
}


#endif /* __WIRE_H__ */
