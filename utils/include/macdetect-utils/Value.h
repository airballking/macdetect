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


#ifndef __VALUE_H__
#define __VALUE_H__


// System
#include <list>
#include <string>
#include <string.h>
#include <iostream>
#include <memory>


namespace macdetect {
  class Value {
  public:
  private:
    std::list< std::shared_ptr<Value> > m_lstSubValues;
    
    std::string m_strKey;
    std::string m_strContent;
    
  protected:
  public:
    Value(std::string strKey = "", std::string strContent = "", std::list< std::pair<std::string, std::string> > lstSubValues = {});
    ~Value();
    
    void add(std::shared_ptr<Value> valAdd);
    void add(std::string strKey, std::string strContent);
    
    unsigned int serialize(void* vdBuffer, unsigned int unLength);
    unsigned int deserialize(void* vdBuffer, unsigned int unLength);
    
    void set(std::string strKey, std::string strContent);
    
    std::list< std::shared_ptr<Value> > subValues();
    std::string key();
    std::string content();
    
    void print(unsigned int unIndent = 0);
    
    std::shared_ptr<Value> copy();
    std::shared_ptr<Value> sub(std::string strSubKey);
  };
}


#endif /* __VALUE_H__ */
