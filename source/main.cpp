////////////////////////////////////////////////////////////////////////
// File main.cpp                                                      //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#include "read_input.h"
#include "read_ini.h"
#include "algorithm.h"

int main(int argc, char *argv[]) {

  ReadInput readInput;
  readInput.readDirectory();
  readInput.createGraph();
  readInput.readFiles();

  ReadIni readIni(&readInput, "mdc.ini");
  
  Algorithm algorithm(readInput.getGraphPtr(), readIni.getSchedulePtr());
  algorithm.initialize(&readIni);
  algorithm.run();

  return 0;
}
