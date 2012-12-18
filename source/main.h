////////////////////////////////////////////////////////////////////////
// File main.h                                                        //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)
#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <cstdlib>

//#define MS 1
#define TOP_DOWN 0
#define BOTTOM_UP 1
#define FLAT 2

using namespace std;

typedef int Index;
typedef float Count;

class Node;
typedef vector<Node*> Graph;
typedef vector<Node*> Schedule;
typedef map<Index, Count> Row;
typedef set<Index> Cluster;

inline int generateRandomNumber(int max) {
  double denom = ((double)RAND_MAX + 1.0) / (double)max;
  return (int)((double)rand() / denom);
}

#endif // MAIN_H
