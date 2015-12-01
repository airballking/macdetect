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
