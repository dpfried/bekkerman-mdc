////////////////////////////////////////////////////////////////////////
// File neighborhood.h                                                //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)
#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H
#include "main.h"
#include "cooccurrence.h"

class Neighborhood {
 public:
  typedef vector<Cooccurrence*> Cooccurrences;
  typedef vector<Count> OverallCounts;

  Neighborhood(void) {};

  void create(int cooccurrenceSize, int numberOfCooccurrences);

  void addValue(int size);

  void putCount(int position, Index index1, Index index2, Count count);
  void addCount(Index index, Index position, Index value, Count count);

  void putOverallCount() {
    overallCounts.push_back(0.0);
  };

  int size() {
    return cooccurrences.size();
  };

  void addCooccurrence(Index index, Cooccurrence *cooccurrencePtr) {
    cooccurrences[index]->addCooccurrence(cooccurrencePtr);
  };

  void subtractCooccurrence(Index index, Cooccurrence *cooccurrencePtr) {
    cooccurrences[index]->subtractCooccurrence(cooccurrencePtr);
  };

  void erase(Index index);

  Cooccurrence* getCooccurrencePtr(Index index) {
    return cooccurrences[index];
  };

  Row* getRowPtr(Index index, Index position) {
    return cooccurrences[index]->getRowPtr(position);
  };

  Count getOverallCount(Index position) {
    return overallCounts[position];
  };

  OverallCounts* getOverallCounts() {
    return &overallCounts;
  };

  Cooccurrences* getCooccurrences() {
    return &cooccurrences;
  };

  Count getValueCount(Index index, Index position) {
    return cooccurrences[index]->getValueCount(position);
  };

  void clear();
  void assign(Neighborhood *from);

 private:
  Cooccurrences cooccurrences;
  OverallCounts overallCounts;
};

#endif // NEIGHBORHOOD_H
