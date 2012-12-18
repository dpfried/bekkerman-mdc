////////////////////////////////////////////////////////////////////////
// File node.h                                                        //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)
#ifndef NODE_H
#define NODE_H
#include "main.h"
#include "neighborhood.h"
#include "clustering.h"

class Node {
 public:
  typedef vector<string> ValueNames;
  typedef map<string, Index> ValueNames2Indexes;
  typedef vector<int> EdgeIDs;
  typedef vector<Node *> Edges;
  typedef vector<Count> Weights;

  Node(string _name): name(_name), numberOfClusters(-1) {
    setType(TOP_DOWN);
  };

  bool sameName(string str) {
    return (name.compare(str) == 0);
  };

  Index addValue(string valueName);
  Index findIndex(string valueName);
  void putCount(int edgeID, Index value1, Index value2, Count count);
  void buildValuesOverClusters();
  void iteration(bool last);

  Index findEdgeIDPosition(int edgeID);
  Cooccurrence* findValuesCooccurrence(int edgeID);
  Index findSymmetricPosition(Index i);

  void runIteration(int iterationNumber);
  bool checkAndRunLastIteration();
  void print(int iterationNumber, bool lastIteration);

  void addEdgeID(int edgeID) {
    edgeIDs.push_back(edgeID);
  };

  void addEdge(Node *nodePtr) {
    edges.push_back(nodePtr);
    weights.push_back(1.0);
  };

  Node* getEdge(Index i) {
    return edges[i];
  };

  Count getWeight(Index i) {
    return weights[i];
  };

  Index findEdgePosition(Node *nodePtr);
  bool addWeight(Node *nodePtr, Count weight);

  void buildClustersOverClusters() {
    clustering.buildClustersOverClusters();
  };

  void clusteringInitialize(ReadIni* readIniPtr) {
    clustering.setThisNodePtr(this);
    clustering.initialize(readIniPtr, numberOfValues());
  };

  void setNumberOfClusters(int _numberOfClusters) {
    numberOfClusters = _numberOfClusters;
  }

  int getNumberOfClusters() {
    return numberOfClusters;
  }

  void setFinalStepTrigger(int _finalStepTrigger) {
    finalStepTrigger = _finalStepTrigger;
  }

  void setType(char nodeType) {
    clustering.setType(nodeType);
  };

  char getType() {
    return clustering.getType();
  };

  double calculateInformationGain() {
    return clustering.calculateInformationGain();
  };

  Clustering* getClusteringPtr() {
    return &clustering;
  };

  Row* getRowPtr(Index index, int position) {
    return values.getRowPtr(index, position);
  }

  Neighborhood* getClustersOverClusters() {
    return clustering.getClustersOverClusters();
  }

  int numberOfNeighbors() {
    return edges.size();
  };

  int numberOfValues() {
    return values.size();
  }

  Count getOverallCount(Index position) {
    return values.getOverallCount(position);
  };

  Count getValueCount(Index index, Index position) {
    return values.getValueCount(index, position);
  };

  string getValueName(Index index) {
    return valueNames[index];
  };

  void putOverallCount() {
    values.putOverallCount();
  };

  string getName() {
    return name;
  };

  int clusteringSize() {
    return clustering.size();
  };

  int clusteringType() {
    return clustering.getType();
  };

 private:
  void runLastIteration();

  string name;
  int numberOfClusters;
  int finalStepTrigger;

  Neighborhood values;
  ValueNames valueNames;
  ValueNames2Indexes valueNames2Indexes;
  EdgeIDs edgeIDs;
  Edges edges;
  Weights weights;
  Clustering clustering;
};

#endif // NODE_H
