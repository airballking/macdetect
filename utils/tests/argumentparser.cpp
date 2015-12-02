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
