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


#ifndef __CONFIG_H__
#define __CONFIG_H__


// System
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <string>


namespace macdetect {
  class Config {
  private:
    std::map<std::string, std::map<std::string, std::string>> m_mapSections;
    
  protected:
  public:
    Config();
    ~Config();
    
    std::list<std::string> splitString(std::string strContent);
    std::list<std::string> filterLines(std::list<std::string> lstLines);
    std::map<std::string, std::map<std::string, std::string>> parseLines(std::list<std::string> lstLines);
    
    bool loadFromString(std::string strConfig);
    bool loadFromFile(std::string strFilepath);
    
    std::string value(std::string strSection, std::string strTerm);
    std::list<std::string> list(std::string strSection, std::string strTerm);
  };
}


#endif /* __CONFIG_H__ */
