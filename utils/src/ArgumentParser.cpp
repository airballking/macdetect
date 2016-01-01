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


#include <macdetect-utils/ArgumentParser.h>


namespace macdetect {
  ArgumentParser::ArgumentParser(std::list<ArgumentPrototype> lstValidArguments) : m_lstValidArguments(lstValidArguments) {
  }
  
  ArgumentParser::~ArgumentParser() {
  }
  
  void ArgumentParser::addArgument(Argument argAdd) {
    m_lstArguments.push_back(argAdd);
  }
  
  void ArgumentParser::parse(int nArgCount, char** carrArgs) {
    std::vector<std::string> vecArtifacts;
    
    for(unsigned int unI = 1; unI < nArgCount; ++unI) {
      vecArtifacts.push_back(std::string(carrArgs[unI]));
    }
    
    for(unsigned int unI = 0; unI < vecArtifacts.size(); ++unI) {
      std::string strArtifact = vecArtifacts[unI];
      
      ArgumentType atType;
      std::string strKey = "";
      bool bLong = false;
      bool bParameter = false;
      
      if(strArtifact.substr(0, 2) == "--") {
	strKey = strArtifact.substr(2);
	bLong = true;
      } else if(strArtifact.substr(0, 1) == "-") {
	strKey = strArtifact.substr(1);
      } else {
	strKey = strArtifact;
	bParameter = true;
      }
      
      if(!bParameter) {
	for(ArgumentPrototype apArgument : m_lstValidArguments) {
	  if(((bLong && apArgument.strLong == strKey) ||
	      (!bLong && apArgument.strShort == strKey))) {
	    if(apArgument.atType == Switch) {
	      this->addArgument({strKey, "", Switch});
	    } else if(apArgument.atType == Token) {
	      if(unI < vecArtifacts.size() - 1) {
		this->addArgument({strKey, vecArtifacts[unI + 1], Token});
		unI++;
	      }
	    }
	  }
	}
      } else {
	this->addArgument({"", strKey, Parameter});
      }
    }
  }
  
  std::list<ArgumentParser::Argument> ArgumentParser::arguments() {
    return m_lstArguments;
  }
  
  bool ArgumentParser::switched(std::string strKey) {
    bool bReturn = false;
    
    for(Argument argArgument : this->arguments()) {
      if(argArgument.atType == Switch && this->keyFitsArgument(strKey, argArgument)) {
	bReturn = true;
	break;
      }
    }
    
    return bReturn;
  }
  
  std::string ArgumentParser::value(std::string strKey) {
    std::string strReturn = "";
    
    for(Argument argArgument : this->arguments()) {
      if(argArgument.atType == Token && this->keyFitsArgument(strKey, argArgument)) {
	strReturn = argArgument.strValue;
	break;
      }
    }
    
    return strReturn;
  }
  
  bool ArgumentParser::keyFitsArgument(std::string strKey, Argument argArgument) {
    bool bReturn = false;
    
    for(ArgumentPrototype apArgument : m_lstValidArguments) {
      if(apArgument.strShort == strKey || apArgument.strLong == strKey) {
	bReturn = (argArgument.strKey == apArgument.strShort || argArgument.strKey == apArgument.strLong);
	break;
      }
    }
    
    return bReturn;
  }
  
  std::list<std::string> ArgumentParser::parameters() {
    std::list<std::string> lstParameters;
    
    for(Argument argArgument : this->arguments()) {
      if(argArgument.atType == Parameter) {
	lstParameters.push_back(argArgument.strValue);
      }
    }
    
    return lstParameters;
  }
}
