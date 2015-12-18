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


// System
#include <cstdlib>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

// MAC Detect
#include <macdetectd/Globals.h>
#include <macdetectd/Network.h>
#include <macdetectd/Server.h>
#include <macdetectd/Daemon.h>
#include <macdetect-utils/ArgumentParser.h>


macdetect::Daemon g_dmDaemon;

namespace macdetect {
  bool g_bDaemon = false;
}


void catchHandler(int nSignum) {
  switch(nSignum) {
  case SIGTERM:
  case SIGINT: {
    g_dmDaemon.shutdown();
  } break;
    
  default:
    break;
  }
}


int main(int argc, char** argv) {
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = catchHandler;
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGINT, &action, NULL);
  
  int nReturnvalue = EXIT_FAILURE;
  
  macdetect::ArgumentParser apArguments({{"d", "daemon", macdetect::ArgumentParser::Switch},
                                         {"c", "config-file", macdetect::ArgumentParser::Token}});
  apArguments.parse(argc, argv);
  
  bool bPrivilegesSuffice = g_dmDaemon.privilegesSuffice();
  bool bRunnable = false;
  
  if(bPrivilegesSuffice) {
    if(apArguments.switched("daemon")) {
      macdetect::g_bDaemon = true;
      
      pid_t pPid = fork();
      
      if(pPid == 0) {
	// We are the child.
	bRunnable = true;
	umask(0);
	
	openlog(argv[0], LOG_NOWAIT | LOG_PID, LOG_USER);
	log(macdetect::Normal, "Started macdetectd daemon.");
	
	pid_t pSid = setsid();
	
	if(pSid < 0) {
	  log(macdetect::Error, "Could not create process group (failed during `setsid()`).");
	} else {
	  // NOTE(winkler): One could change the working directory here;
	  // we rely on the local data directory, so we're not doing
	  // that at the moment.
	  
	  close(STDIN_FILENO);
	  close(STDOUT_FILENO);
	  close(STDERR_FILENO);
	  
	  nReturnvalue = EXIT_SUCCESS;
	}
      
	if(nReturnvalue == EXIT_SUCCESS) {
	  bRunnable = true;
	}
      } else if(pPid < 0) {
	// Something went wrong.
      } else if(pPid > 0) {
	// We are the parent.
	nReturnvalue = EXIT_SUCCESS;
      }
    } else {
      bRunnable = true;
    }
  
    if(bRunnable) {
      std::string strConfigFile = apArguments.value("config-file");
      
      bool bConfigLoaded = false;
      if(g_dmDaemon.parseConfigFile(strConfigFile) == false) {
	bConfigLoaded = g_dmDaemon.parseConfigFile("~/.macdetectd/macdetectd.config");
      } else {
	bConfigLoaded = true;
      }
      
      if(!bConfigLoaded) {
	log(macdetect::Error, "Didn't find any valid configuration files. The daemon will run unconfigured.");
      }
      
      /*g_dmDaemon.serve("lo", 7090);
	g_dmDaemon.serve("wlan0", 7090);*/
      
      log(macdetect::Normal, "Entering main loop.");
      
      while(g_dmDaemon.cycle()) {
	usleep(10);
      }
      
      log(macdetect::Normal, "Exiting gracefully.");
    }
  } else {
    log(macdetect::Error, "User privileges don't suffice. Exiting.");
  }
  
  closelog();
  
  return nReturnvalue;
}
