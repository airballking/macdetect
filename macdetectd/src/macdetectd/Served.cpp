#include <macdetectd/Served.h>


namespace macdetect {
  Served::Served(int nSocketFD) : PacketEntity(nSocketFD) {
  }
  
  Served::~Served() {
  }
}