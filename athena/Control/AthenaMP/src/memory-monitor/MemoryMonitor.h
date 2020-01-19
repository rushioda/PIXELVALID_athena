/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <vector>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include <condition_variable> 
#include <thread> 
#include <sstream>

int ReadProcs(pid_t mother_pid, unsigned long values[4], unsigned long long valuesIO[4], bool verbose=false);
int MemoryMonitor(pid_t mpid, char* filename=NULL, char* jsonSummary=NULL, unsigned int interval=600);


