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


#ifndef __CLIENT_H__
#define __CLIENT_H__


// System
#include <string>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

// MAC detect
#include <macdetect-utils/PacketEntity.h>


namespace macdetect {
  class Client : public PacketEntity {
  private:
  protected:
  public:
    Client();
    ~Client();
    
    bool connect(std::string strIP, unsigned short usPort = 7090);
    bool disconnect();
    
    bool connected();
  };
}


#endif /* __CLIENT_H__ */
