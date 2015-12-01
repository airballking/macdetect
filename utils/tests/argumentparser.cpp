#include <cstdlib>

#include <macdetect-utils/ArgumentParser.h>


int main(int argc, char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  char* carrArguments[] = {"executable-name", "--test1", "--test2=a", "-h", "-test3", "value"};
  
  macdetect::ArgumentParser apParser({{"test1", false}, {"test2", true}, {"test3", true}, {"h", false}});
  apParser.parse(6, carrArguments);
  
  return nReturnvalue;
}
