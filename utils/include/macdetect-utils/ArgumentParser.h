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
