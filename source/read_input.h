////////////////////////////////////////////////////////////////////////
// File read_input.h                                                  //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)
#ifndef READINPUT_H
#define READINPUT_H
#include "main.h"
#include "node.h"
#ifdef MS
#include <windows.h>
#else
#include <dirent.h>
#endif

class ReadInput {
 public:
  typedef vector<string> FileNames;
  typedef vector<Node*> NodePointers;

  ReadInput(void) {};

  void createGraph();
  void readFiles();
  void readDirectory();
  Node* findNode(string name);

  Graph* getGraphPtr() {
    return &graph;
  }

 private:
  Graph graph;
  FileNames fileNames;
  NodePointers nodePointers;

  void readFile(ifstream& in, Node *firstNodePtr, 
		Node *secondNodePtr, int edgeID);
  void addNode(string name, int edgeID);
};

#endif // READINPUT_H
