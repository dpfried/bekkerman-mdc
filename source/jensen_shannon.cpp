////////////////////////////////////////////////////////////////////////
// File jensen_shannon.cpp                                            //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#include "jensen_shannon.h"
#include "clustering.h"
#include "node.h"
#include "neighborhood.h"
#include <cmath>

Index JensenShannon::closestPeer(Index clusterIndex) {
  double bestReductionOfInformation = -1.0;
  Index bestCluster = -1;

  Neighborhood* clustersOverClustersPtr = clusteringPtr->getClustersOverClusters();
  for(Index index = 0; index < clustersOverClustersPtr->size(); index++) {
    if(visitedClusters.find(index) != visitedClusters.end() ||
       clusterIndex == index)
      continue;
    if(clusteringPtr->isGolden(clusterIndex) && clusteringPtr->isGolden(index))
      continue;

    double accumulativeReduction = 0.0;
    for(int i = 0; i < clusteringPtr->getThisNodePtr()->numberOfNeighbors(); i++) {
      double reductionOfInformation = 0.0;

      double N = clusteringPtr->getThisNodePtr()->getOverallCount(i);
      double Nc1 = clustersOverClustersPtr->getValueCount(clusterIndex, i);
      double Nc2 = clustersOverClustersPtr->getValueCount(index, i);
      double sum = Nc1 + Nc2;
      Row* c1RowPtr = clustersOverClustersPtr->getRowPtr(clusterIndex, i);
      Row* c2RowPtr = clustersOverClustersPtr->getRowPtr(index, i);

      Row::iterator it;
      Visited visited;
      for(it = c1RowPtr->begin(); it != c1RowPtr->end(); it++) {
        double Nc1cy = it->second;

        Row::iterator c2It = c2RowPtr->find(it->first);
        if(c2It != c2RowPtr->end()) {
          double Nc2cy = c2It->second;
          double factor = sum / (Nc1cy + Nc2cy);
          double term = Nc1cy * log((Nc1cy/Nc1) * factor) +
	    Nc2cy * log((Nc2cy/Nc2) * factor);
          if(term < -1e-10) {
            cout << "Problem with Jensen-Shannon\n";
            exit(1);
          }
          reductionOfInformation += term;
        }
        else {
          reductionOfInformation += (Nc1cy * log(sum/Nc1));
        }
        visited.insert(it->first);
      }

      for(it = c2RowPtr->begin(); it != c2RowPtr->end(); it++) {
        if(visited.find(it->first) != visited.end())
          continue;
        reductionOfInformation += (it->second * log(sum/Nc2));
      }

      accumulativeReduction += (reductionOfInformation / N);
    }

    if(bestReductionOfInformation == -1 ||
       accumulativeReduction < bestReductionOfInformation) {
      bestReductionOfInformation = accumulativeReduction;
      bestCluster = index;
    }
  }

  visitedClusters.insert(clusterIndex);
  if(bestCluster >= 0)
    visitedClusters.insert(bestCluster);

  return bestCluster;
}
