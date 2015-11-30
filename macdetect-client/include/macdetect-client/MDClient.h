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


#ifndef __MDCLIENT_H__
#define __MDCLIENT_H__


// System
#include <list>
#include <string>

// MAC detect
#include <macdetect-client/Client.h>


namespace macdetect_client {
  class MDClient {
  private:
    std::list<macdetect::Packet*> m_lstReceivedPackets;
    macdetect::Client m_cliClient;
    
  protected:
    macdetect::Packet* getPacket(std::string strKey, std::string strValue, bool bBlock = true);
    
  public:
    MDClient();
    ~MDClient();
    
    bool connect(std::string strIP);
    bool disconnect();
    
    macdetect::Packet* requestResponse(std::string strRequest);
    macdetect::Packet* requestResponse(macdetect::Packet* pktRequest, std::string strKey = "response");
    
    std::list<std::string> knownMACAddresses();
    std::list<std::string> deviceNames();
    
    std::list<macdetect::Packet*> devicesList();
    
    bool enableStream(std::string strDeviceName);
    bool disableStream(std::string strDeviceName);
    
    macdetect::Packet* info();
  };
}


#endif /* __MDCLIENT_H__ */
