////////////////////////////////////////////////////////////////////////
// File read_ini.h                                                    //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)
#ifndef READINI_H
#define READINI_H
#include "main.h"
#include "node.h"
#include "read_input.h"

class ReadIni {
 public:
  typedef vector<string> FileNames;
  ReadIni(ReadInput *_readInputPtr, string iniFileName):
    readInputPtr(_readInputPtr) {
    readIniFile(iniFileName);
  };

  Schedule* getSchedulePtr() {
    return &schedule;
  }

  void readDirectory();
  bool openFile(string name, ifstream* fileInput);
  bool readLine(ifstream* fileInput, Index* returnClusterIndex, string* returnValue, bool* returnGolden);

 private:
  void readIniFile(string iniFileName);
  void processVariable(string str);

  Schedule schedule;
  ReadInput *readInputPtr;
  FileNames fileNames;
  int clusterIndex;
  string value;
  bool golden;
};

#endif // READINI_H
