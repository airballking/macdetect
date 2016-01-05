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


#ifndef __PACKETENTITY_H__
#define __PACKETENTITY_H__


// System
#include <unistd.h>
#include <sys/socket.h>
#include <mutex>

// MAC detect
#include <macdetect-utils/Value.h>


namespace macdetect {
  class PacketEntity {
  private:
  protected:
    int m_nSocketFD;
    bool m_bFailureState;
    std::mutex m_mtxSocketAccess;
    
  public:
    PacketEntity(int nSocketFD);
    ~PacketEntity();
    
    bool send(std::shared_ptr<Value> valSend);

    virtual int recv(int nSocketFD, void* vdBuffer, int nLength, int nFlags);
    std::shared_ptr<Value> receive(bool& bDisconnected);
    
    int socket();
    void setSocket(int nSocketFD);
    
    bool failureState();
    
    virtual int write(int nSocketFD, void* vdBuffer, unsigned int unLength);
  };
}


#endif /* __PACKETENTITY_H__ */
