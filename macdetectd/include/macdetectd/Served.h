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


#ifndef __SERVED_H__
#define __SERVED_H__


// System
#include <unistd.h>
#include <iostream>

// MAC Detect
#include <macdetect-utils/PacketEntity.h>


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
