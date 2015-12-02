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
