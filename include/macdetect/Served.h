#ifndef __SERVED_H__
#define __SERVED_H__


// System
#include <unistd.h>
#include <iostream>

// MAC Detect
#include <macdetect/PacketEntity.h>


namespace macdetect {
  class Served : public PacketEntity {
  private:
  protected:
  public:
    Served(int nSocketFD);
    ~Served();
  };
}


#endif /* __SERVED_H__ */
