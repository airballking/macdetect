// Copyright 2016 Jan Winkler <jan.winkler.84@gmail.com>
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** \author Jan Winkler */


#ifndef __SOCKET_READER_H__
#define __SOCKET_READER_H__


// System
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netpacket/packet.h>
#include <string.h>


namespace detect {
  class SocketReader {
  private:
    int m_nDefaultReadingLength;
    unsigned char* m_ucInternalBuffer;
    
  protected:
    int m_nSocket;
    
  public:
    SocketReader();
    ~SocketReader();
    
    int read(unsigned char* ucBuffer, unsigned int unLength);
    int bufferedRead(unsigned char** ucBuffer);
    int bufferedRead(unsigned char** ucBuffer, int nReadingLength);
    void setDefaultReadingLength(int nDefaultReadingLength);
    int defaultReadingLength();
    
    int forward(unsigned int unBytes);
    
    void setSocket(int nSocket);
    int socket();
  };
}


#endif /* __SOCKET_READER_H__ */
