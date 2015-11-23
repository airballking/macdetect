#ifndef __IP_H__
#define __IP_H__


// System
#include <string>
#include <linux/ip.h>


namespace macdetect {
  namespace protocols {
    class IP {
    private:
    protected:
    public:
      IP();
      ~IP();
      
      static void wrap(void* vdBuffer, void* vdPayload, unsigned int unPayloadLength, uint8_t un8Protocol, std::string strSourceIP, std::string strDestinationIP);
      static void parse(void* vdData, void*& vdPayload, unsigned int& unPayloadLength, uint8_t& un8Protocol, std::string& strSourceIP, std::string& strDestinationIP);
    };
  }
}


#endif /* __IP_H__ */
