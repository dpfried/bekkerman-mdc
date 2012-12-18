////////////////////////////////////////////////////////////////////////
// File clustering.cpp                                                //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#include "node.h"
#include "clustering.h"
#include "jensen_shannon.h"
#include "read_ini.h"
#include <cmath>

int Clustering::initializeFromFile(ReadIni* readIniPtr, int size, 
				   ifstream *fileInputPtr, string name) {
  for(int i = 0; i < size; i++) {
    values2clusters.push_back((Index)-1);
    goldenValues.push_back(false);
  }
  int clusterNum, oldClusterNum = -1;
  string value;
  bool golden;
  Cluster *clusterPtr;
  while(readIniPtr->readLine(fileInputPtr, &clusterNum, &value, &golden)) {
    Index index = thisNodePtr->findIndex(value);
    if(index == -1) {
      cout << "Unseen value " << value << " in file " << name << endl;
      exit(1);
    }
    if(oldClusterNum != clusterNum) {
      goldenClusters.push_back(golden);
      clusterPtr = new Cluster();
      clusters.push_back(clusterPtr);
      oldClusterNum = clusterNum;
    }
    clusterPtr->insert(index);
    values2clusters[index] = clusterNum;
    goldenValues[index] = golden;
  }
  return clusterNum;
}

int Clustering::initializeFlatClustering(int size) {
  int numberOfClusters = thisNodePtr->getNumberOfClusters();
  Cluster *clusterPtr;
  for(int i = 0; i < numberOfClusters; i++) {
    clusterPtr = new Cluster();
    clusters.push_back(clusterPtr);
    goldenClusters.push_back(false);
  }
  for(int i = 0; i < size; i++) {
    Index index = (Index)i;
    int randomNumber = generateRandomNumber(numberOfClusters);
    clusters[randomNumber]->insert(index);
    values2clusters.push_back(randomNumber);
    goldenValues.push_back(false);
  }
  return numberOfClusters;
}

void Clustering::initialize(ReadIni* readIniPtr, int size) {
  ifstream fileInput;

  readIniPtr->readDirectory();
  string name = thisNodePtr->getName();
  if(readIniPtr->openFile(name, &fileInput)){
    int clusterNum = initializeFromFile(readIniPtr, size, &fileInput, name);
    fileInput.close();
    for(int i = 0; i < size; i++) {
      if(values2clusters[i] == -1) {
	cout << "Problem with " << name << ".ini" << endl;
	exit(1);
      }
    }
    cout << "Node " << name << " initialized (from file) with "
	 << clusterNum+1 << " clusters" << endl;
    return;
  }

  if(clusteringType == FLAT) {
    int numberOfClusters = initializeFlatClustering(size);
    cout << "Flat node " << name << " initialized with "
         << numberOfClusters << " clusters" << endl;
    return;
  }

  if(clusteringType == BOTTOM_UP) {
    for(int i = 0; i < size; i++) {
      Index index = (Index)i;
      Cluster *clusterPtr = new Cluster();
      clusterPtr->insert(index);
      clusters.push_back(clusterPtr);
      goldenClusters.push_back(false);
      values2clusters.push_back(index);
      goldenValues.push_back(false);
    }
    cout << "Agglomerative node " << name << " initialized with "
         << size << " clusters" << endl;
  }
  else {
    Cluster *clusterPtr = new Cluster();
    for(int i = 0; i < size; i++) {
      Index index = (Index)i;
      clusterPtr->insert(index);
      values2clusters.push_back((Index)0);
      goldenValues.push_back(false);
    }
    clusters.push_back(clusterPtr);
    goldenClusters.push_back(false);
    cout << "Divisive node " << name << " initialized with 1 cluster" << endl;
  }
}

void Clustering::splitOrMerge(bool last) {
  if(last) {
    if(clusteringType == BOTTOM_UP)
      merge(1);
    else
      split(1);
  }
  else {
    if(clusteringType == BOTTOM_UP)
      merge(clusters.size() / 2);
    else
      split(clusters.size());
  }
}

Clustering::ClustersIterator Clustering::findLargestCluster() {
  ClustersIterator itClusters = clusters.begin(), savedItClusters;
  int maxClusterSize = 0, clusterIndex = -1;
	
  for(int iClusters = 0; itClusters < clusters.end(); iClusters++, itClusters++) {
    int size = (*itClusters)->size();
    if(size > maxClusterSize) {
      maxClusterSize = size;
      savedItClusters = itClusters;
      clusterIndex = iClusters;
    }
  }
  //cout << "Cluster " << clusterIndex << " of size " << maxClusterSize
  //     << " is split" << endl;
  return savedItClusters;
}

void Clustering::split(int numberOfSplits) {
  typedef vector<Cluster::iterator> IndexPointers;
  ClustersIterator itClusters = clusters.begin();
  Clusters newClusters;

  // split _first_ "numberOfSplits" clusters, unless numberOfSplits = 1
  for(int iClusters = 0; iClusters < numberOfSplits; itClusters++, iClusters++) {

    if(numberOfSplits == 1)
      itClusters = findLargestCluster();
		
    if((*itClusters)->size() == 1)
      continue;

    Cluster *newClusterPtr = new Cluster();
    Cluster::iterator it;
    IndexPointers indexPointers;
    for(it = (*itClusters)->begin(); it != (*itClusters)->end(); it++) {
      indexPointers.push_back(it);
    }
    int size = indexPointers.size();
    Index clusterIndex = clusters.size() + newClusters.size();
    for(int i = 0; i < size; i++) {
      int randomNumber = generateRandomNumber(indexPointers.size());
      Cluster::iterator indexPtr = indexPointers[randomNumber];
      indexPointers.erase(indexPointers.begin() + randomNumber);
      if(i & 1) {
        // add it to a new cluster
        newClusterPtr->insert(*indexPtr);
        // change its values2clusters
        values2clusters[*indexPtr] = clusterIndex;
        // remove index from itClusters
        (*itClusters)->erase(indexPtr);
      }
    }
    newClusters.push_back(newClusterPtr);
  }
  clusters.insert(clusters.end(), newClusters.begin(), newClusters.end());
}

void Clustering::merge(int numberOfMerges) {
  if(clusters.size() == 1) {
    cout << "Warning: only one cluster" << endl;
    return;
  }
  JensenShannon js(this);
  thisNodePtr->buildValuesOverClusters();
  buildClustersOverClusters();

  Mapping clusterIndexes;
  for(int i = 0; i < clusters.size(); i++) {
    clusterIndexes.push_back((Index)i);
  }

  Clusters* clustersPtr = new Clusters();
  Flags* goldenClustersPtr = new Flags();
  for(int ii = 0; ii < numberOfMerges; ii++) {
    Index index1, index2;
    bool returnFlag;
    if(numberOfMerges != 1)
      returnFlag = findAllPeers(&clusterIndexes, &js, &index1, &index2);
    else
      returnFlag = findTwoPeers(&clusterIndexes, &js, &index1, &index2);
    if(returnFlag == false)
      break;
		
    Cluster* clusterPtr = mergeClusters(index1, index2, clustersPtr->size());
    clustersPtr->push_back(clusterPtr);
    if(goldenClusters[index1] && goldenClusters[index2]) {
      cout << "Problem: golden clusters merged" << endl;
      exit(1);
    }
    if(goldenClusters[index1] || goldenClusters[index2])
      goldenClustersPtr->push_back(true);
    else
      goldenClustersPtr->push_back(false);
  }
  // add remaining clusters
  for(int iii = 0; iii < clusterIndexes.size(); iii++) {
    Index index = clusterIndexes[iii];
    Cluster* clusterPtr = new Cluster(*clusters[index]);
    for(Cluster::iterator it = clusterPtr->begin();
        it != clusterPtr->end(); it++) {
      values2clusters[*it] = clustersPtr->size();
    }
    clustersPtr->push_back(clusterPtr);
    goldenClustersPtr->push_back(goldenClusters[index]);
  }
  // clean and copy
  for(ClustersIterator it = clusters.begin(); it != clusters.end(); it++)
    delete *it;
  clusters.clear();
  goldenClusters.clear();
  clusters.assign(clustersPtr->begin(), clustersPtr->end());
  goldenClusters.assign(goldenClustersPtr->begin(), goldenClustersPtr->end());
}

Cluster* Clustering::mergeClusters(Index first, Index second, Index index) {
  Cluster* clusterPtr = new Cluster();
  Cluster::iterator it;
  for(it = clusters[first]->begin(); it != clusters[first]->end(); it++) {
    clusterPtr->insert(*it);
    values2clusters[*it] = index;
  }
  for(it = clusters[second]->begin(); it != clusters[second]->end(); it++) {
    clusterPtr->insert(*it);
    values2clusters[*it] = index;
  }
  return clusterPtr;
}

bool Clustering::findAllPeers(Mapping* clusterIndexesPtr, JensenShannon* jsPtr,
                              Index* index1Ptr, Index* index2Ptr) {
  int randomNumber = generateRandomNumber(clusterIndexesPtr->size());
  *index1Ptr = (*clusterIndexesPtr)[randomNumber];

  *index2Ptr = jsPtr->closestPeer(*index1Ptr);
  if(*index2Ptr < 0)
    return false;
  clusterIndexesPtr->erase(clusterIndexesPtr->begin() + randomNumber);
  //cout << "Cluster " << (*index2Ptr) <<
  //  " is the closest peer of cluster " << (*index1Ptr) << endl;
  for(Mapping::iterator itI = clusterIndexesPtr->begin();
      itI != clusterIndexesPtr->end(); itI++)
    if(*itI == *index2Ptr) {
      clusterIndexesPtr->erase(itI);
      break;
    }
  return true;
}

bool Clustering::findTwoPeers(Mapping* clusterIndexesPtr, JensenShannon* jsPtr,
                              Index* index1Ptr, Index* index2Ptr) {
  int minClusterSize = 0;
  Index clusterIndex = 0;
  for(ClustersIterator it = clusters.begin(); it != clusters.end(); it++) {
    int clusterSize = (*it)->size();
    if(clusterSize < minClusterSize || minClusterSize == 0) {
      minClusterSize = clusterSize;
      *index1Ptr = clusterIndex;
    }
    clusterIndex++;
  }

  *index2Ptr = jsPtr->closestPeer(*index1Ptr);
  if(*index2Ptr < 0)
    return false;
  cout << "Cluster " << (*index2Ptr) <<
    " is the closest peer of cluster " << (*index1Ptr) << endl;
  clusterIndexesPtr->erase(clusterIndexesPtr->begin() + *index1Ptr);
  for(Mapping::iterator itI = clusterIndexesPtr->begin();
      itI != clusterIndexesPtr->end(); itI++)
    if(*itI == *index2Ptr) {
      clusterIndexesPtr->erase(itI);
      break;
    }
  return true;
}

void Clustering::buildValuesOverClusters(Index position, Node *nodePtr) {
  Clustering *anotherClusteringPtr;
  anotherClusteringPtr = nodePtr->getClusteringPtr();
  // upside down
  Index otherNodePosition = thisNodePtr->findSymmetricPosition(position);
  anotherClusteringPtr->buildContingencyTable(otherNodePosition,
                                              &valuesOverClusters, position);
}

void Clustering::buildContingencyTable(Index position,
                                       Neighborhood* neighborhoodPtr,
                                       Index otherNodePosition) {
  ClustersIterator itClusters;
  Index clusterIndex = 0;

  for(itClusters = clusters.begin(); itClusters != clusters.end();
      itClusters++, clusterIndex++) {
    Cluster::iterator it;

    for(it = (*itClusters)->begin(); it != (*itClusters)->end(); it++) {
      Row *rowPtr = thisNodePtr->getRowPtr(*it, position);

      Row::iterator itRow;
      for(itRow = rowPtr->begin(); itRow != rowPtr->end(); itRow++) {
        neighborhoodPtr->addCount(itRow->first, otherNodePosition,
                                  clusterIndex, itRow->second);
      }
    }
  }
}

void Clustering::buildClustersOverClusters() {
  ClustersIterator itClusters;
  Index index = 0;

  clustersOverClusters.create(thisNodePtr->numberOfNeighbors(), clusters.size());

  for(itClusters = clusters.begin(); itClusters != clusters.end(); itClusters++) {
    Cluster::iterator it;

    for(it = (*itClusters)->begin(); it != (*itClusters)->end(); it++) {
      clustersOverClusters.addCooccurrence(index,
					   valuesOverClusters.getCooccurrencePtr(*it));
    }
    index++;
  }
}

double Clustering::correctionLoop() {
  Mapping valueIndexes;
  int size = values2clusters.size();
  double overallGain = 0.0;

  for(int ii = 0; ii < size; ii++)
    valueIndexes.push_back((Index)ii);

  int numberOfRelocatedValues = 0;
  for(int i = 0; i < size; i++) {
    if((i+1) % 1000 == 0)
      cout << "Values processed: " << (i+1) << endl;
    int randomNumber = generateRandomNumber(valueIndexes.size());
    Index index = valueIndexes[randomNumber];
    valueIndexes.erase(valueIndexes.begin() + randomNumber);
    if(goldenValues[index])
      continue;

    double bestGain = 0.0;
    Index bestCluster = -1;
    Index clusterOfValue = values2clusters[index];
    double removalGain = calculateGainOnSubtract(index, clusterOfValue);
    for(Index clusterIndex = 0; clusterIndex < clusters.size(); clusterIndex++) {
      if(clusterIndex == clusterOfValue)
        continue;
      double gain = calculateGainOnAdd(index, clusterIndex) + removalGain;
      if(bestGain < gain) {
        bestGain = gain;
        bestCluster = clusterIndex;
      }
    }
    if(bestCluster != -1) {
      numberOfRelocatedValues++;
      // remove index from its cluster
      Index oldClusterIndex = values2clusters[index];
      Cluster::iterator forRemoval = clusters[oldClusterIndex]->find(index);
      clusters[oldClusterIndex]->erase(forRemoval);
      // add index to bestCluster
      clusters[bestCluster]->insert(index);
      // update values2clusters
      values2clusters[index] = bestCluster;
      Cooccurrence* cooccurrencePtr =
        valuesOverClusters.getCooccurrencePtr(index);
      clustersOverClusters.addCooccurrence(bestCluster, cooccurrencePtr);
      if(clusters[oldClusterIndex]->empty()) {
        //cout << "Cluster " << oldClusterIndex << " is empty" << endl;
        erase(oldClusterIndex);
      }
      else
        clustersOverClusters.subtractCooccurrence(oldClusterIndex,
                                                  cooccurrencePtr);
      overallGain += bestGain;
    }
  }

  cout << "Number of relocated values: " << numberOfRelocatedValues << endl;
  return overallGain;
}

void Clustering::erase(Index index) {
  if(index >= clusters.size()) {
    cout << "Problem with erasing cluster " << index << endl;
    exit(1);
  }
  clusters.erase(clusters.begin() + index);
  goldenClusters.erase(goldenClusters.begin() + index);
  for(int i = 0; i < values2clusters.size(); i++) {
    if(values2clusters[i] > index)
      values2clusters[i]--;
  }
  clustersOverClusters.erase(index);
}

double Clustering::calculateGainOnAdd(Index index, Index clusterIndex) {
  typedef set<Index> Visited;
  double valueGain = 0.0;
  for(int i = 0; i < thisNodePtr->numberOfNeighbors(); i++) {
    Neighborhood* neighborhoodPtr =
      thisNodePtr->getEdge(i)->getClustersOverClusters();
    Index symmetricPosition = thisNodePtr->findSymmetricPosition(i);
    double cooccurrenceGain = 0.0;

    double N = thisNodePtr->getOverallCount(i);
    double Nv = thisNodePtr->getValueCount(index, i);
    double Ncx = clustersOverClusters.getValueCount(clusterIndex, i);

    double factor1 = N / (Nv + Ncx);
    double factor2 = N / Ncx;

    Row* clusterRowPtr = clustersOverClusters.getRowPtr(clusterIndex, i);
    Row* valueRowPtr = valuesOverClusters.getRowPtr(index, i);

    Row::iterator it;
    Visited visited;
    for(it = clusterRowPtr->begin(); it != clusterRowPtr->end(); it++) {
      Row::iterator valueIt = valueRowPtr->find(it->first);
      double Ncxcy = it->second;
      if(valueIt == valueRowPtr->end())
        cooccurrenceGain += (Ncxcy * log(factor1/factor2));
      else {
        double Ncy = neighborhoodPtr->getValueCount(it->first,
                                                    symmetricPosition);
        //if(Ncxcy <= 0 || Ncy <= 0 || Ncx <= 0) {
	//	cout << "Problem with contingency table; Ncxcy = " << Ncxcy <<
	//		"; Ncy = " << Ncy << "; Ncx = " << Ncx << endl;
	//	exit(1);
        //}
        double sum = Ncxcy + valueIt->second;
        cooccurrenceGain +=
          (sum * log(sum * factor1 / Ncy) -
           Ncxcy * log(Ncxcy * factor2 / Ncy));
      }
      visited.insert(it->first);
    }

    for(it = valueRowPtr->begin(); it != valueRowPtr->end(); it++) {
      if(visited.find(it->first) != visited.end())
        continue;
      double Ncy = neighborhoodPtr->getValueCount(it->first, symmetricPosition);
      double Nvcy = it->second;
      cooccurrenceGain += (Nvcy * log(Nvcy * factor1 / Ncy));
    }
    valueGain += (thisNodePtr->getWeight(i) * cooccurrenceGain / N);
  }
  return valueGain;
}

double Clustering::calculateGainOnSubtract(Index index, Index clusterIndex) {
  double valueGain = 0.0;
  for(int i = 0; i < thisNodePtr->numberOfNeighbors(); i++) {
    Neighborhood* neighborhoodPtr =
      thisNodePtr->getEdge(i)->getClustersOverClusters();
    Index symmetricPosition = thisNodePtr->findSymmetricPosition(i);
    double cooccurrenceGain = 0.0;

    double N = thisNodePtr->getOverallCount(i);
    double Nv = thisNodePtr->getValueCount(index, i);
    double Ncx = clustersOverClusters.getValueCount(clusterIndex, i);

    double factor1 = (Ncx - Nv)? N / (Ncx - Nv) : 0.0;
    double factor2 = N / Ncx;

    Row* clusterRowPtr = clustersOverClusters.getRowPtr(clusterIndex, i);
    Row* valueRowPtr = valuesOverClusters.getRowPtr(index, i);

    Row::iterator it;
    for(it = clusterRowPtr->begin(); it != clusterRowPtr->end(); it++) {
      Row::iterator valueIt = valueRowPtr->find(it->first);
      double Ncxcy = it->second;

      if(valueIt == valueRowPtr->end() && factor1 != 0) {
        cooccurrenceGain += (Ncxcy * log(factor1/factor2));
      }
      else {
        double Ncy = neighborhoodPtr->getValueCount(it->first,
                                                    symmetricPosition);
        cooccurrenceGain -= (Ncxcy * log(Ncxcy * factor2 / Ncy));
        double res = Ncxcy - valueIt->second;
        if(res > 0 && factor1 > 0)
          cooccurrenceGain += (res * log(res * factor1 / Ncy));
	//if(res < 0 || factor1 < 0) {
	//	cout << "Problem with subtracting element "
	//			 << thisNodePtr->getValueName(index) << endl;
	//	exit(1);
        //}
      }
    }
    valueGain += (thisNodePtr->getWeight(i) * cooccurrenceGain / N);
  }
  return valueGain;
}

double Clustering::calculateInformationGain() {
  double informationGain = 0.0;
  for(int i = 0; i < thisNodePtr->numberOfNeighbors(); i++) {
    Neighborhood* neighborhoodPtr =
      thisNodePtr->getEdge(i)->getClustersOverClusters();
    Index symmetricPosition = thisNodePtr->findSymmetricPosition(i);
    double localGain = 0.0;
    double N = thisNodePtr->getOverallCount(i);

    for(Index clusterIndex = 0; clusterIndex < clustersOverClusters.size();
        clusterIndex++) {
      double Ncx = clustersOverClusters.getValueCount(clusterIndex, i);

      Row* clusterRowPtr = clustersOverClusters.getRowPtr(clusterIndex, i);
      for(Row::iterator it = clusterRowPtr->begin(); it != clusterRowPtr->end();
          it++) {
        double Ncxcy = it->second;
        double Ncy = neighborhoodPtr->getValueCount(it->first,
                                                    symmetricPosition);
	localGain += (Ncxcy * log( (Ncxcy / Ncx) * (N / Ncy) ));
      }
    }
    Count weight = thisNodePtr->getWeight(i);
    cout << "Term " << i << ": " << (localGain / N) << "; weight: " << weight << endl;
    informationGain += (weight * localGain / N);
  }
  return informationGain;
}

void Clustering::copy(Clustering *from) {
  ClustersIterator it;
  for(it = clusters.begin(); it != clusters.end(); it++)
    delete *it;
  clusters.clear();

  for(it = from->getClusters()->begin(); it != from->getClusters()->end(); it++) {
    Cluster* cluster = new Cluster(**it);
    clusters.push_back(cluster);
  }

  values2clusters.clear();
  values2clusters.assign(from->getValues2clusters()->begin(),
                         from->getValues2clusters()->end());
  goldenClusters.clear();
  goldenClusters.assign(from->getGoldenClusters()->begin(),
                        from->getGoldenClusters()->end());

  clustersOverClusters.clear();
  clustersOverClusters.assign(from->getClustersOverClusters());
}

void Clustering::print(ofstream &out) {
  ClustersIterator itClusters;
  int position = 0;

  for(itClusters = clusters.begin(); itClusters != clusters.end();
      itClusters++, position++) {
    out << "Cluster " << position << endl;

    Cluster::iterator it;
    for(it = (*itClusters)->begin(); it!= (*itClusters)->end(); it++) {
      string valueName = thisNodePtr->getValueName(*it);
      out << "\t" << valueName << endl;
    }
  }
}

void Clustering::cprint() {
  ClustersIterator itClusters;
  int position = 0;

  for(itClusters = clusters.begin(); itClusters != clusters.end();
      itClusters++, position++) {
    cout << "Cluster " << position << endl;

    Cluster::iterator it;
    for(it = (*itClusters)->begin(); it!= (*itClusters)->end(); it++) {
      string valueName = thisNodePtr->getValueName(*it);
      cout << "\t" << valueName << endl;
    }
  }
}
