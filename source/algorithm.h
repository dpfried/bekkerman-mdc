////////////////////////////////////////////////////////////////////////
// File algorithm.h                                                   //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)
#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "main.h"
#include "node.h"

class Algorithm {
 public:

  Algorithm(Graph *_graphPtr, Schedule *_schedulePtr):
    graphPtr(_graphPtr),
    schedulePtr(_schedulePtr) {};

  void run();
  void initialize(ReadIni* readIniPtr);

  Graph* getGraphPtr() {
    return graphPtr;
  };

 private:
  Graph *graphPtr;
  Schedule *schedulePtr;

  void buildValuesOverClusters();
  void buildClustersOverClusters();
};

#endif // ALGORITHM_H
