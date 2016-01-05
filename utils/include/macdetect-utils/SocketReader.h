// macdetect, a daemon and clients for detecting MAC addresses
// Copyright (C) 2015 Jan Winkler <jan.winkler.84@gmail.com>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

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


namespace macdetect {
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
    virtual int read(unsigned char* ucBuffer, unsigned int unLength, int nFlags);
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
