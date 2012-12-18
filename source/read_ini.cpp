////////////////////////////////////////////////////////////////////////
// File read_ini.cpp                                                  //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#include "read_ini.h"
#include <cmath>
#include <cstring>

void ReadIni::processVariable(string str) {
  int pos1 = str.find(" ", 0);
  string nodeName = str.substr(0, pos1);
  Node* nodePtr = readInputPtr->findNode(nodeName);
  if(nodePtr == NULL) {
    cout << "No such variable " << nodeName << endl;
    exit(1);
  }
  
  int pos2 = str.find(" ", pos1+1);
  if(pos2 == string::npos)
    pos2 = str.size();
  string nodeType = str.substr(pos1+1, pos2-pos1-1);
  nodePtr->setNumberOfClusters(-1);
  if(nodeType.compare("agglomerative") == 0)
    nodePtr->setType(BOTTOM_UP);
  else if(nodeType.compare("divisive") == 0)
    nodePtr->setType(TOP_DOWN);
  else if(nodeType.compare("flat") == 0) {
    nodePtr->setType(FLAT);
    nodePtr->setNumberOfClusters(int(log(nodePtr->numberOfValues())));
  }
  else {
    cout << "Unknown type of variable " << nodeName << endl;
    exit(1);
  }
  if(pos2 == str.size())
    return;

  int pos3 = str.find(" ", pos2+1);
  if(pos3 == string::npos)
    pos3 = str.size();
  string numberOfClustersStr = str.substr(pos2+1, pos3-pos2-1);
  int numberOfClusters = atoi(numberOfClustersStr.c_str());
  if(numberOfClusters < 2) {
    cout << "Unreasonable number of clusters: " << numberOfClusters << endl;
    exit(1);
  }
  nodePtr->setNumberOfClusters(numberOfClusters);
  if(nodePtr->getType() == BOTTOM_UP)
    nodePtr->setFinalStepTrigger(numberOfClusters * 2);
  if(nodePtr->getType() == TOP_DOWN)
    nodePtr->setFinalStepTrigger(numberOfClusters / 2);
  if(pos3 == str.size())
    return;
  
  int finalStepTrigger = atoi(str.substr(pos3+1, str.size()-pos3-1).c_str());
  if((nodePtr->getType() == BOTTOM_UP && finalStepTrigger > numberOfClusters * 2) ||
     (nodePtr->getType() == TOP_DOWN && finalStepTrigger < numberOfClusters / 2))
    nodePtr->setFinalStepTrigger(finalStepTrigger);
}

void ReadIni::readIniFile(string iniFileName) {
  ifstream in(iniFileName.c_str());  // Open file
  if(!in) {
    cout << "Can't open ini file: " << iniFileName << endl;
    exit(1);
  }

  int scheduleFlag = 0;
  int varFlag = 0;
  int weightsFlag = 0;

  const int SZ = 1000; // Buffer size;
  char buf[SZ];
  while(in.getline(buf, SZ)) { // Removes \n
    string str(buf);

    if(str.find("//", 0) == 0)
      continue;                // Ignore comment lines

    if(str.find("#", 0) == 0) {
      if(varFlag)
        varFlag = 3;
      if(scheduleFlag)
        scheduleFlag = 3;
      if(weightsFlag)
	weightsFlag = 3;
    }

    if(str.compare("# Schedule") == 0){
      scheduleFlag = 1;
      continue;
    }

    if(scheduleFlag > 0 && scheduleFlag < 3) {
      Node* nodePtr = readInputPtr->findNode(str);
      if(nodePtr == NULL) {
        cout << "Problem with schedule in line " << str << endl;
        exit(1);
      }
      schedule.push_back(nodePtr);
      scheduleFlag = 2;
    }

    if(str.compare("# Variables") == 0){
      varFlag = 1;
      continue;
    }

    if(varFlag > 0 && varFlag < 3) {
      processVariable(str);
      varFlag = 2;
    }

    if(str.compare("# Edge weights") == 0){
      weightsFlag = 1;
      continue;
    }

    if(weightsFlag > 0 && weightsFlag < 3) {
      int pos1 = str.find (" ", 0);
      string node1 = str.substr(0, pos1);
      int pos2 = str.find (" ", pos1+1);
      string node2 = str.substr(pos1+1, pos2-pos1-1);
			
      Count weight = (Count)atof(str.substr(pos2+1, str.size()-pos2-1).c_str());
      Node* node1Ptr = readInputPtr->findNode(node1);
      Node* node2Ptr = readInputPtr->findNode(node2);
      bool result = node1Ptr->addWeight(node2Ptr, weight);
      if(result == false) {
	cout << "No edge between " << node1 << " and " << node2 << endl;
        exit(1);
      }
      node2Ptr->addWeight(node1Ptr, weight);
      weightsFlag = 2;
    }
  }

  if(scheduleFlag < 2) {
    cout << "No schedule found - abort" << endl;
    exit(1);
  }
  if(varFlag < 2) {
    cout << "Warning: only divisive variables found" << endl;
  }
}

void ReadIni::readDirectory() {
  DIR* dirPtr = opendir(".");
  if (dirPtr == NULL) {
    cout << "Problem with opening the current directory" << endl;
    exit(1);
  }
  struct dirent *filePtr;
  while ((filePtr = readdir(dirPtr)) != NULL) {
    if (strstr(filePtr->d_name, ".ini") != NULL)
      fileNames.push_back(filePtr->d_name);
  }
  closedir(dirPtr);
}

bool ReadIni::openFile(string name, ifstream* fileInput) {
  FileNames::iterator it;
  for (it = fileNames.begin(); it != fileNames.end(); it++) {
    //cout << "Compare " << *it << " with " << name << endl;
    if(it->compare(name + ".ini") == 0) {
      fileInput->open(it->c_str(), ifstream::in);  // Open file
      if(!(*fileInput)) {
        cout << "Can't open file " << *it << endl;
        exit(1);
      }
      return true;
    }
  }
  return false;
}

bool ReadIni::readLine(ifstream* fileInput, Index* returnClusterIndex,
                       string* returnValue, bool* returnGolden) {
  const int SZ = 1000;
  char buf[SZ];
  if(fileInput->getline(buf, SZ)) {
    string str(buf);
    //cout << "Line " << str << endl;
    if(str.find("Cluster ", 0) != string::npos) {
      if(str.find("Golden ", 0) == 0)
        golden = true;
      else
        golden = false;
      int pos = str.rfind(" ", str.size()-1);
      clusterIndex = (Index)atoi(str.substr(pos+1, str.size()-pos-1).c_str());
      fileInput->getline(buf, SZ);
      string str1(buf);
      str = str1;
    }
    if(str.find("\t", 0) == 0) {
      value = str.substr(1, str.size()-1);
    }
    *returnClusterIndex = clusterIndex;
    *returnValue = value;
    *returnGolden = golden;
    return true;
  }
  return false;
}
