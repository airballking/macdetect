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
    typedef struct {
      std::string strToken;
      std::string strValue;
      bool bHasValue;
      bool bHasToken;
    } Argument;
    
  private:
    std::list< std::pair<std::string, bool> > m_lstValidArguments;
    std::list<Argument> m_lstArguments;
    
  protected:
    void addArgument(Argument argAdd);
    
  public:
    ArgumentParser(std::list< std::pair<std::string, bool> > lstValidArguments);
    ~ArgumentParser();
    
    void parse(int nArgCount, char** carrArgs);
    
    std::list<Argument> arguments();
    std::string value(std::string strToken);
  };
}


#endif /* __ARGUMENTPARSER_H__ */
