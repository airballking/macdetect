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


#ifndef __ARGUMENTPARSER_H__
#define __ARGUMENTPARSER_H__


// System
#include <string>
#include <vector>
#include <list>
#include <iostream>


namespace macdetect {
  class ArgumentParser {
  public:
    typedef enum {
      Switch = 0,
      Token = 1,
      Parameter = 2
    } ArgumentType;
    
    typedef struct {
      std::string strShort;
      std::string strLong;
      ArgumentType atType;
    } ArgumentPrototype;
    
    typedef struct {
      std::string strKey;
      std::string strValue;
      ArgumentType atType;
    } Argument;
    
  private:
    std::list<ArgumentPrototype> m_lstValidArguments;
    std::list<Argument> m_lstArguments;
    
  protected:
    void addArgument(Argument argAdd);
    
  public:
    ArgumentParser(std::list<ArgumentPrototype> lstValidArguments);
    ~ArgumentParser();
    
    void parse(int nArgCount, char** carrArgs);
    
    std::list<Argument> arguments();
    bool switched(std::string strKey);
    std::string value(std::string strKey);
    
    bool keyFitsArgument(std::string strKey, Argument argArgument);
    std::list<std::string> parameters();
  };
}


#endif /* __ARGUMENTPARSER_H__ */
