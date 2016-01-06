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
    std::shared_ptr<Value> add(std::string strKey, std::string strContent);
    
    int serialize(void* vdBuffer, unsigned int unLength);
    unsigned int deserialize(void* vdBuffer, unsigned int unLength);
    
    void setKey(std::string strKey);
    void setContent(std::string strContent);
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
