#ifndef __GLOBALS_H__
#define __GLOBALS_H__


// System
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <string>
#include <iostream>


namespace macdetect {
  typedef enum {
    Normal = 0,
    Error = 1
  } MessagePriority;
  
  void log(MessagePriority mpPriority, const char* carrFormat, ...);
}


#endif /* __GLOBALS_H__ */
