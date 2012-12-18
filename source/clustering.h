////////////////////////////////////////////////////////////////////////
// File clustering.h                                                  //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)
#ifndef CLUSTERING_H
#define CLUSTERING_H
#include "main.h"
#include "neighborhood.h"

class JensenShannon;
class ReadIni;

class Clustering {
 public:
  typedef vector<Index> Mapping;
  typedef vector<bool> Flags;
  typedef vector<Cluster*> Clusters;
  typedef Clusters::iterator ClustersIterator;

  Clustering(void): clusteringType(TOP_DOWN) {};

  ~Clustering(void) {
    for(ClustersIterator it = clusters.begin(); it != clusters.end(); it++)
      delete *it;
    clusters.clear();
    values2clusters.clear();
    goldenClusters.clear();
    valuesOverClusters.clear();
    clustersOverClusters.clear();
  };

  void setType(char _clusteringType) {
    clusteringType = _clusteringType;
  };

  char getType() {
    return clusteringType;
  };

  void setThisNodePtr(Node *_thisNodePtr) {
    thisNodePtr = _thisNodePtr;
  };

  void initialize(ReadIni* readIniPtr, int size);
  void splitOrMerge(bool last);

  void buildValuesOverClusters(Index position, Node *nodePtr);
  void buildClustersOverClusters();
  void buildContingencyTable(Index position,
			     Neighborhood* neighborhoodPtr, Index otherNodePosition);
  double correctionLoop();
  double calculateInformationGain();
  void copy(Clustering *from);

  void print(ofstream& out);
  void cprint();

  Clusters* getClusters() {
    return &clusters;
  };

  Mapping* getValues2clusters() {
    return &values2clusters;
  };

  Flags* getGoldenClusters() {
    return &goldenClusters;
  };

  Neighborhood* getClustersOverClusters() {
    return &clustersOverClusters;
  };

  Node* getThisNodePtr() {
    return thisNodePtr;
  };

  Neighborhood* getValuesOverClusters() {
    return &valuesOverClusters;
  };

  int size() {
    return clusters.size();
  };

  bool isGolden(Index index) {
    return goldenClusters[index];
  };

 private:
  int initializeFromFile(ReadIni* readIniPtr, int size, 
			 ifstream *fileInputPtr, string name);
  int initializeFlatClustering(int size);

  Cluster* mergeClusters(Index first, Index second, Index index);
  Clustering::ClustersIterator findLargestCluster();
  double calculateGainOnAdd(Index index, Index position);
  double calculateGainOnSubtract(Index index, Index position);
  void erase(Index index);
  bool findAllPeers(Mapping* clusterIndexesPtr, JensenShannon* jsPtr,
		    Index* index1Ptr, Index* index2Ptr);
  bool findTwoPeers(Mapping* clusterIndexesPtr, JensenShannon* jsPtr,
		    Index* index1Ptr, Index* index2Ptr);

  void merge(int numberOfMerges);
  void split(int numberOfSplits);

  char clusteringType;

  Node *thisNodePtr;

  Clusters clusters;
  Mapping values2clusters;
  Flags goldenValues;
  Flags goldenClusters;

  Neighborhood valuesOverClusters;
  Neighborhood clustersOverClusters;
};

#endif // CLUSTERING_H
