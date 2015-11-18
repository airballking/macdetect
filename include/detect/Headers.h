#ifndef __HEADERS_H__
#define __HEADERS_H__


namespace detect {
  struct EthernetFrameHeader {
    unsigned char ucReceiverHW[6];
    unsigned char ucSenderHW[6];
    unsigned short usFrameType;
  };
  
  struct IPHeader {
    __u8 ihl:4;
    __u8 version:4;
    __u8	tos;
    __be16 tot_len;
    __be16 id;
    __be16 frag_off;
    __u8 ttl;
    __u8 protocol;
    __sum16 check;
    __be32 saddr;
    __be32 daddr;
    /* The options start here. */
  };
}


#endif /* __HEADERS_H__ */
