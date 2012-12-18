////////////////////////////////////////////////////////////////////////
// File cooccurrence.h                                                //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)
#ifndef COOCCURRENCE_H
#define COOCCURRENCE_H
#include "main.h"

class Cooccurrence {
 public:
  typedef vector<Row*> Rows;
  typedef vector<Count> ValueCounts;

  Cooccurrence(void) {};
  Cooccurrence(int size) {
    for(int i = 0; i < size; i++) {
      Row *row = new Row;
      rows.push_back(row);
      valueCounts.push_back(0.0);
    }
  };

  Cooccurrence(Cooccurrence* from) {
    Rows::iterator it;
    for(it = from->getRows()->begin(); it != from->getRows()->end(); it++) {
      Row* row = new Row(**it);
      rows.push_back(row);
    }
    valueCounts.assign(from->getValueCounts()->begin(), 
		       from->getValueCounts()->end());
  };

  void clear();

  void putCount(Index pos, Index index, Count count);
  void addCount(Index pos, Index index, Count count);
  void addRow(Index indexTo, Row* rowFromPtr);
  void addCooccurrence(Cooccurrence* cooccurrencePtr);
  void subtractCount(Index pos, Index index, Count count);
  void subtractRow(Index indexTo, Row* rowFromPtr);
  void subtractCooccurrence(Cooccurrence* cooccurrencePtr);

  Row* getRowPtr(Index index);
  Count getValueCount(Index index);

  ValueCounts* getValueCounts() {
    return &valueCounts;
  };

  Rows* getRows() {
    return &rows;
  };

 private:
  Rows rows;
  ValueCounts valueCounts;
};

#endif // COOCCURRENCE_H
