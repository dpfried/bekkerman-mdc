////////////////////////////////////////////////////////////////////////
// File node.cpp                                                      //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#include "node.h"
#include <stdio.h>
#define NUMBER_OF_RANDOM_RESTARTS 10
#define NUMBER_OF_CONVERGENT_STEPS 2

Index Node::addValue(string valueName) {
  Index pos = findIndex(valueName);
  if(pos >= 0)
    return pos;

  pos = valueNames.size();
  valueNames.push_back(valueName);
  valueNames2Indexes[valueName] = pos;

  values.addValue(edgeIDs.size());
  return pos;
}

bool Node::addWeight(Node *nodePtr, Count weight) {
  Index pos = findEdgePosition(nodePtr);
  if(pos == -1)
    return false;
  weights[pos] = weight;
  return true;
}

Index Node::findIndex(string valueName) {
  if(valueNames2Indexes.find(valueName) == valueNames2Indexes.end())
    return -1;
  else
    return valueNames2Indexes[valueName];
}

void Node::putCount(int edgeID, Index value1, Index value2, Count count) {
  Index position = findEdgeIDPosition(edgeID);

  values.putCount(position, value1, value2, count);
}

void Node::buildValuesOverClusters() {
  Edges::iterator it;
  int neighborhoodIndex = 0;

  clustering.getValuesOverClusters()->create(edges.size(), values.size());
  for(it = edges.begin(); it < edges.end(); it++) {
    clustering.buildValuesOverClusters(neighborhoodIndex, *it);
    neighborhoodIndex++;
  }
}

Index Node::findEdgeIDPosition(int edgeID) {
  EdgeIDs::iterator it;
  int position = 0;
  bool found = false;

  for(it = edgeIDs.begin(); it < edgeIDs.end(); it++) {
    if(*it == edgeID) {
      found = true;
      break;
    }
    position++;
  }

  if(!found) {
    cout << "Problem with egdeID " << edgeID << endl;
    exit(1);
  }

  return position;
}

Index Node::findEdgePosition(Node *nodePtr) {
  Edges::iterator it;
  int position = 0;

  for(it = edges.begin(); it < edges.end(); it++) {
    if(*it == nodePtr) {
      return position;
    }
    position++;
  }
  return -1;
}

Cooccurrence* Node::findValuesCooccurrence(int edgeID) {
  Index position = findEdgeIDPosition(edgeID);
  return values.getCooccurrencePtr(position);
}

Index Node::findSymmetricPosition(Index i) {
  int edgeID = edgeIDs[i];
  Node* nodePtr = edges[i];
  return nodePtr->findEdgeIDPosition(edgeID);
}

void Node::iteration(bool last) {
  double bestGain = 0.0;
  Clustering bestClustering;
  bestClustering.copy(&clustering);
  Clustering backupClustering;
  backupClustering.copy(&clustering);
  buildValuesOverClusters();
  for(int i = 0; i < NUMBER_OF_RANDOM_RESTARTS; i++) {
    cout << "Random restart #" << i << endl;

    if(clusteringType() != FLAT)
      clustering.splitOrMerge(last);
		
    if(clustering.size() == 1) {
      cout << "Warning: there is only one cluster in " << name << endl;
      continue;
    }
    buildClustersOverClusters();

    double gain = calculateInformationGain();
    cout << "Initial value of objective function: " << gain << endl;
    for(int j = 0; j < NUMBER_OF_CONVERGENT_STEPS; j++) {
      cout << "Convergent step: " << j << endl;
      gain += clustering.correctionLoop();
    }
    if(gain > bestGain) {
      bestGain = gain;
      cout << "Best gain: " << gain << endl;
      bestClustering.copy(&clustering);
    }
    clustering.copy(&backupClustering);
  }
  clustering.copy(&bestClustering);
}

void Node::runIteration(int iterationNumber) {
  cout << "Iteration " << iterationNumber << " (" << name << ")" << endl;
  iteration(false);
  cout << "Clustering size: " << clusteringSize() << endl;
  print(iterationNumber, false);
}

void Node::runLastIteration() {
  cout << "Last iteration: starting with " <<
    clusteringSize() << " clusters" << endl;
  iteration(true);
  print(clusteringSize(), true);
}

bool Node::checkAndRunLastIteration() {
  if(clusteringType() == FLAT ||
     numberOfClusters == -1)
    return false;

  if(clusteringType() == BOTTOM_UP &&
     numberOfClusters != -1 && clusteringSize() < finalStepTrigger) {
    for(int i = clusteringSize(); i > numberOfClusters; i--)
      runLastIteration();
    return true;
  }
  if(clusteringType() == TOP_DOWN &&
     numberOfClusters != -1 && clusteringSize() > finalStepTrigger) {
    for(int i = clusteringSize(); i < numberOfClusters; i++)
      runLastIteration();
    return true;
  }
  return false;
}

void Node::print(int iterationNumber, bool lastIteration) {
  string fileName(name);
  if(lastIteration)
    fileName.append("_final");
  char numberChar[10];
  sprintf(numberChar, "%d", iterationNumber);
  string numberString(numberChar);
  fileName.append(".");
  fileName.append(numberString);

  ofstream out(fileName.c_str());  // Open file
  if(!out) {
    cout << "Can't open output file: " << fileName << endl;
    exit(1);
  }

  clustering.print(out);
  out.close();
}
