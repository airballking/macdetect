#include <macdetectd/Globals.h>


namespace macdetect {
  extern bool g_bDaemon;
  
  void log(MessagePriority mpPriority, const char* carrFormat, ...) {
    va_list valArgs;
    va_start(valArgs, carrFormat);
    std::cout << g_bDaemon << std::endl;
    if(g_bDaemon) {
      int nPriority;
      
      switch(mpPriority) {
      default:
      case Normal: {
	nPriority = LOG_NOTICE;
      } break;
	
      case Error: {
	nPriority = LOG_ERR;
      } break;
      }
      
      vsyslog(nPriority, carrFormat, valArgs);
    } else {
      switch(mpPriority) {
      default:
      case Normal: {
	std::cout << "\033[0m";
      } break;
	
      case Error: {
	std::cout << "\033[0;31m";
      } break;
      }
      
      std::string strNewFormat = "[macdetectd] " + std::string(carrFormat);
      vprintf(strNewFormat.c_str(), valArgs);
      std::cout << std::endl;
      
      std::cout << "\033[0m";
    }
    
    va_end(valArgs);
  }
}
