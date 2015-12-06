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
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

// MAC Detect
#include <macdetectd/Network.h>
#include <macdetectd/Server.h>
#include <macdetectd/Daemon.h>
#include <macdetect-utils/ArgumentParser.h>


macdetect::Daemon g_dmDaemon;


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
  
  if(g_dmDaemon.privilegesSuffice()) {
    macdetect::ArgumentParser apArguments({{"d", "daemon", macdetect::ArgumentParser::Switch}});
    apArguments.parse(argc, argv);
    bool bRegularStartup = false;
  
    if(apArguments.switched("daemon")) {
      pid_t pPid = fork();
    
      if(pPid == 0) {
	// We are the child.
	umask(0);
      
	openlog(argv[0], LOG_NOWAIT | LOG_PID, LOG_USER);
	syslog(LOG_NOTICE, "Started macdetectd daemon.");
      
	pid_t pSid = setsid();
	if(pSid < 0) {
	  syslog(LOG_ERR, "Could not create process group (failed during `setsid()`).");
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
	  bRegularStartup = true;
	} else {
	  closelog();
	}
      } else if(pPid < 0) {
	// Something went wrong.
      } else if(pPid > 0) {
	// We are the parent.
	nReturnvalue = EXIT_SUCCESS;
      }
    } else {
      bRegularStartup = true;
    }
  
    if(bRegularStartup) {
      syslog(LOG_NOTICE, "Entering main loop.");
    
      while(g_dmDaemon.cycle()) {
	usleep(10);
      }
    
      syslog(LOG_NOTICE, "Exiting gracefully.");
      closelog();
    }
  } else {
    syslog(LOG_NOTICE, "User privileges don't suffice. Exiting.");
  }
  
  return nReturnvalue;
}
