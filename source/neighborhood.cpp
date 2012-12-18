////////////////////////////////////////////////////////////////////////
// File neighborhood.cpp                                              //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#include "neighborhood.h"

void Neighborhood::addValue(int size) {
  Cooccurrence *cooccurrence = new Cooccurrence(size);
  cooccurrences.push_back(cooccurrence);
}

void Neighborhood::putCount(int position, Index index1,
                            Index index2, Count count) {
  cooccurrences[index1]->putCount(position, index2, count);
  overallCounts[position] += count;
}

void Neighborhood::addCount(Index index, Index position,
                            Index value, Count count) {
  cooccurrences[index]->addCount(position, value, count);
  overallCounts[position] += count;
}

void Neighborhood::create(int cooccurrenceSize, int numberOfCooccurrences) {
  clear();
  for(int i = 0; i < numberOfCooccurrences; i++) {
    Cooccurrence* cooccurrencePtr = new Cooccurrence(cooccurrenceSize);
    cooccurrences.push_back(cooccurrencePtr);
  }
  for(int ii = 0; ii < cooccurrenceSize; ii++) {
    overallCounts.push_back(0.0);
  }
}

void Neighborhood::erase(Index index) {
  // overallCounts should also be updated but this seems to be unnecessary
  cooccurrences[index]->clear();
  cooccurrences.erase(cooccurrences.begin() + index);
};

void Neighborhood::clear() {
  Cooccurrences::iterator it;

  for(it = cooccurrences.begin(); it != cooccurrences.end(); it++)
    (*it)->clear();

  cooccurrences.clear();
  overallCounts.clear();
}

void Neighborhood::assign(Neighborhood *from) {
  Cooccurrences::iterator it;

  for(it = from->getCooccurrences()->begin();
      it != from->getCooccurrences()->end(); it++) {
    Cooccurrence *cooccurrence = new Cooccurrence(*it);
    cooccurrences.push_back(cooccurrence);
  }

  overallCounts.assign(from->getOverallCounts()->begin(),
                       from->getOverallCounts()->end());
}
