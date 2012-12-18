////////////////////////////////////////////////////////////////////////
// File cooccurrence.cpp                                              //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#include "cooccurrence.h"

Row* Cooccurrence::getRowPtr(Index index) {
  //if(index >= rows.size()) {
  //cout << "Cannot get row #" << index << endl;
  //exit(1);
  //}
  return rows[index];
};

Count Cooccurrence::getValueCount(Index index) {
  //int size = valueCounts.size();
  //if(index >= size) {
  //cout << "Cannot get count #" << index << endl;
  //exit(1);
  //}
  return valueCounts[index];
};

void Cooccurrence::putCount(Index pos, Index index, Count count) {
  (*rows[pos])[index] = count;
  valueCounts[pos] += count;
}

void Cooccurrence::addCount(Index pos, Index index, Count count) {
  Row* rowPtr = rows[pos];

  if(rowPtr->find(index) == rowPtr->end())
    (*rowPtr)[index] = count;
  else
    (*rowPtr)[index] += count;
  valueCounts[pos] += count;
}

void Cooccurrence::addRow(Index indexTo, Row* rowFromPtr) {
  Row::iterator it;

  for(it = rowFromPtr->begin(); it != rowFromPtr->end(); it++) {
    addCount(indexTo, it->first, it->second);
  }
}

void Cooccurrence::addCooccurrence(Cooccurrence *cooccurrencePtr) {
  for(int position = 0; position < rows.size(); position++)
    addRow(position, cooccurrencePtr->getRowPtr(position));
}		

void Cooccurrence::subtractCount(Index pos, Index index, Count count) {
  Row* rowPtr = rows[pos];

  if(rowPtr->find(index) != rowPtr->end()) {
    (*rowPtr)[index] -= count;
    if((*rowPtr)[index] == 0.0) {
      Row::iterator it = rowPtr->find(index);
      rowPtr->erase(it);
    }
    valueCounts[pos] -= count;
    //if(valueCounts[pos] == 0.0) {
    //cout << "Problem with valueCounts at position " << pos << endl;
    //exit(1);
    //}
  }
}

void Cooccurrence::subtractRow(Index indexTo, Row* rowFromPtr) {
  Row::iterator it;

  for(it = rowFromPtr->begin(); it != rowFromPtr->end(); it++) {
    subtractCount(indexTo, it->first, it->second);
  }
}

void Cooccurrence::subtractCooccurrence(Cooccurrence *cooccurrencePtr) {
  for(int position = 0; position < rows.size(); position++)
    subtractRow(position, cooccurrencePtr->getRowPtr(position));
}

void Cooccurrence::clear() {
  Rows::iterator it;

  for(it = rows.begin(); it != rows.end(); it++)
    delete *it;

  rows.clear();
  valueCounts.clear();
}
