////////////////////////////////////////////////////////////////////////
// File jensen_shannon.h                                              //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)
#ifndef JENSEN_SHANNON_H
#define JENSEN_SHANNON_H
#include "main.h"

class Clustering;

class JensenShannon {
 public:
  typedef set<Index> Visited;

  JensenShannon(Clustering* _clusteringPtr) {
    clusteringPtr = _clusteringPtr;
  };

  Index closestPeer(Index index);

 private:
  Clustering* clusteringPtr;
  Visited visitedClusters;

};

#endif // JENSEN_SHANNON_H
