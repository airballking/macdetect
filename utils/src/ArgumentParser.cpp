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


#include <macdetect-utils/ArgumentParser.h>


namespace macdetect {
  ArgumentParser::ArgumentParser(std::list< std::pair<std::string, bool> > lstValidArguments) : m_lstValidArguments(lstValidArguments) {
  }
  
  ArgumentParser::~ArgumentParser() {
  }
  
  void ArgumentParser::addArgument(Argument argAdd) {
    m_lstArguments.push_back(argAdd);
  }
  
  void ArgumentParser::parse(int nArgCount, char** carrArgs) {
    std::vector<std::string> vecArtifacts(carrArgs, carrArgs + nArgCount);
    
    for(unsigned int unI = 1; unI < vecArtifacts.size(); unI++) {
      std::string strArtifact = vecArtifacts[unI];
      
      // TODO(winkler): Implement the artifact parsing here.
    }
  }
  
  std::list<ArgumentParser::Argument> ArgumentParser::arguments() {
    return m_lstArguments;
  }
  
  std::string ArgumentParser::value(std::string strToken) {
    std::string strReturn = "";
    
    for(Argument argArgument : m_lstArguments) {
      if(argArgument.bHasToken && argArgument.strToken == strToken) {
	strReturn = argArgument.strValue;
	break;
      }
    }
    
    return strReturn;
  }
}
