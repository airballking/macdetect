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
