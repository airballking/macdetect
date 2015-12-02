#include <cstdlib>
#include <memory>
#include <vector>
#include <string.h>

#include <macdetect-utils/ArgumentParser.h>


std::pair<int, char**> makeArguments(std::vector<std::string> vecArguments) {
  vecArguments.insert(vecArguments.begin(), "executable");
  char** carrArguments = new char*[vecArguments.size()];
  
  for(int nI = 0; nI < vecArguments.size(); nI++) {
    carrArguments[nI] = new char[vecArguments[nI].length() + 1];
    memcpy(carrArguments[nI], vecArguments[nI].c_str(), vecArguments[nI].length());
    carrArguments[nI][vecArguments[nI].length()] = '\0';
  }
  
  return std::make_pair(vecArguments.size(), carrArguments);
}


void deleteArguments(std::pair<int, char**> prArguments) {
  for(int nI = 0; nI < prArguments.first; nI++) {
    delete[] prArguments.second[nI];
  }
  
  delete[] prArguments.second;
}


int main(int argc, char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  macdetect::ArgumentParser apParser({{"t1", "test1", macdetect::ArgumentParser::Switch}, {"t2", "test2", macdetect::ArgumentParser::Token}});
  
  std::pair<int, char**> prArguments = makeArguments({"-t1", "--test2", "content", "some-parameter"});
  apParser.parse(prArguments.first, prArguments.second);
  
  if(apParser.switched("test1") && apParser.value("test2") == "content") {
    if(apParser.parameters().front() == "some-parameter") {
      nReturnvalue = EXIT_SUCCESS;
    }
  }
  
  deleteArguments(prArguments);
  
  return nReturnvalue;
}
