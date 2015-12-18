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


#include <macdetect-utils/Config.h>


namespace macdetect {
  Config::Config() {
  }
  
  Config::~Config() {
  }
  
  std::list<std::string> Config::splitString(std::string strContent) {
    std::stringstream sts(strContent);
    std::string strBuffer;
    std::list<std::string> lstLines;
    
    while(std::getline(sts, strBuffer, '\n')) {
      strBuffer.erase(strBuffer.find_last_not_of(" \n\r\t") + 1);
      lstLines.push_back(strBuffer);
    }
    
    return lstLines;
  }
  
  std::list<std::string> Config::filterLines(std::list<std::string> lstLines) {
    std::list<std::string> lstFiltered;
    
    for(std::string strLine : lstLines) {
      if(strLine.length() > 0) {
	std::string strLineFiltered;
	char cStringDelimiter = '\0';
	
	for(int nI = 0; nI < strLine.length(); nI++) {
	  char cAt = strLine.at(nI);
	  
	  if(cAt == '"' || cAt == '\'') {
	    if(cStringDelimiter == '\0') {
	      cStringDelimiter = cAt;
	    } else {
	      if(cStringDelimiter == cAt) {
		cStringDelimiter = '\0';
	      }
	    }
	  } else if(cAt == '#') {
	    if(cStringDelimiter == '\0') {
	      break;
	    }
	  }
	  
	  strLineFiltered += cAt;
	}
	
	strLineFiltered.erase(strLineFiltered.find_last_not_of(" \n\r\t") + 1);
	
	if(strLineFiltered.length() > 0) {
	  lstFiltered.push_back(strLineFiltered);
	}
      }
    }
    
    return lstFiltered;
  }
  
  std::map<std::string, std::map<std::string, std::string>> Config::parseLines(std::list<std::string> lstLines) {
    std::map<std::string, std::map<std::string, std::string>> mapResult;
    std::string strSection = "global";
    
    for(std::string strLine : lstLines) {
      if(strLine.at(0) == '[' && strLine.at(strLine.length() - 1) == ']') {
	strSection = strLine.substr(1, strLine.length() - 2);
      } else {
	size_t idxAssign = strLine.find_first_of("=");
	std::string strTerm = strLine.substr(0, idxAssign);
	std::string strValue = strLine.substr(idxAssign + 1);
	
	mapResult[strSection][strTerm] = strValue;
      }
    }
    
    return mapResult;
  }
  
  bool Config::loadFromString(std::string strConfig) {
    std::list<std::string> lstLines = this->filterLines(this->splitString(strConfig));
    std::map<std::string, std::map<std::string, std::string>> mapSections = this->parseLines(lstLines);
    
    m_mapSections = mapSections;
    
    return true;
  }
  
  bool Config::loadFromFile(std::string strFilepath) {
    std::ifstream ifsFile(strFilepath);
    
    if(ifsFile.is_open()) {
      std::string strContent((std::istreambuf_iterator<char>(ifsFile)),
			     std::istreambuf_iterator<char>());
      
      return this->loadFromString(strContent);
    }
    
    return false;
  }
  
  std::string Config::value(std::string strSection, std::string strTerm) {
    return m_mapSections[strSection][strTerm];
  }
  
  std::list<std::string> Config::list(std::string strSection, std::string strTerm) {
    std::list<std::string> lstList;
    
    std::string strValue = this->value(strSection, strTerm);
    char cStringDelimiter = '\0';
    size_t idxLast = 0;
    
    for(int nI = 0; nI < strValue.length(); nI++) {
      char cAt = strValue.at(nI);
      
      if(cAt == '"' || cAt == '\'') {
	if(cStringDelimiter == '\0') {
	  cStringDelimiter = cAt;
	} else {
	  if(cStringDelimiter == cAt) {
	    cStringDelimiter = '\0';
	  }
	}
      } else if(cAt == ',') {
	if(cStringDelimiter == '\0') {
	  std::string strPortion = strValue.substr(idxLast, nI - idxLast);
	  strPortion.erase(strPortion.find_last_not_of(" \n\r\t") + 1);
	  strPortion = strPortion.substr(strPortion.find_first_not_of(" \n\r\t"));
	  
	  if((strPortion.at(0) == '"' && strPortion.at(strPortion.length() - 1) == '"') ||
	     (strPortion.at(0) == '\'' && strPortion.at(strPortion.length() - 1) == '\'')) {
	    strPortion = strPortion.substr(1, strPortion.length() - 2);
	  }
	  
	  lstList.push_back(strPortion);
	  
	  idxLast = nI + 1;
	}
      }
    }
    
    std::string strPortion = strValue.substr(idxLast);
    strPortion.erase(strPortion.find_last_not_of(" \n\r\t") + 1);
    strPortion = strPortion.substr(strPortion.find_first_not_of(" \n\r\t"));
    
    if((strPortion.at(0) == '"' && strPortion.at(strPortion.length() - 1) == '"') ||
       (strPortion.at(0) == '\'' && strPortion.at(strPortion.length() - 1) == '\'')) {
      strPortion = strPortion.substr(1, strPortion.length() - 2);
    }
    
    lstList.push_back(strPortion);
    
    return lstList;
  }
}
