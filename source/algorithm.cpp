////////////////////////////////////////////////////////////////////////
// File algorithm.cpp                                                 //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#include "algorithm.h"
#include <unistd.h>
#include <sys/time.h>

void Algorithm::run() {
  timeval tv;
  gettimeofday(&tv, 0);
  srand(tv.tv_usec * getpid());

  buildValuesOverClusters();
  buildClustersOverClusters();

  Schedule::iterator it;
  int iterationNumber = 0;
  for(it = schedulePtr->begin(); it != schedulePtr->end(); it++) {
    bool lastIterationRun = (*it)->checkAndRunLastIteration();
    if(lastIterationRun) break;
    		
    (*it)->runIteration(iterationNumber);
    iterationNumber++;
  }

}

void Algorithm::initialize(ReadIni* readIniPtr) {
  Graph::iterator it;

  for(it = graphPtr->begin(); it != graphPtr->end(); it++)
    (*it)->clusteringInitialize(readIniPtr);
}

void Algorithm::buildValuesOverClusters() {
  Graph::iterator it;

  for(it = graphPtr->begin(); it != graphPtr->end(); it++) {
    (*it)->buildValuesOverClusters();
  }
}

void Algorithm::buildClustersOverClusters() {
  Graph::iterator it;

  for(it = graphPtr->begin(); it != graphPtr->end(); it++) {
    (*it)->buildClustersOverClusters();
  }
}
