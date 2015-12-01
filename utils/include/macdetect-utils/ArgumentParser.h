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
