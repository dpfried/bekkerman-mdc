////////////////////////////////////////////////////////////////////////
// File read_input.cpp                                                //
// Version 3.1                                                        //
// Author: Ron Bekkerman (ronb@cs.umass.edu)                          //
// Copyright by the author, 2005                                      //
////////////////////////////////////////////////////////////////////////

#include "read_input.h"
#include <cstring>

#ifdef MS
void ReadInput::readDirectory() {
  WIN32_FIND_DATA FindFileData;
  HANDLE hFind = INVALID_HANDLE_VALUE;
  DWORD dwError;

  hFind = FindFirstFile("*.mdc", &FindFileData);

  if (hFind == INVALID_HANDLE_VALUE) {
    cout << "Invalid file handle. Error is " << GetLastError() << endl;
    exit(1);
  }
  else
    {
      fileNames.push_back(FindFileData.cFileName);
      while (FindNextFile(hFind, &FindFileData) != 0) {
	fileNames.push_back(FindFileData.cFileName);
      }

      dwError = GetLastError();
      FindClose(hFind);
      if (dwError != ERROR_NO_MORE_FILES) {
	cout << "FindNextFile error. Error is " << dwError << endl;
	exit(1);
      }
    }
}

#else

void ReadInput::readDirectory() {
  DIR* dirPtr = opendir(".");
  if (dirPtr == NULL) {
    cout << "Problem with opening the current directory" << endl;
    exit(1);
  }
  struct dirent *filePtr;
  bool fileFound = false;
  while ((filePtr = readdir(dirPtr)) != NULL) {
    if (strstr(filePtr->d_name, ".mdc") != NULL) {
      fileNames.push_back(filePtr->d_name);
      fileFound = true;
    }
  }
  if(fileFound == false) {
    cout << "No input file found" << endl;
    exit(1);
  }
  closedir(dirPtr);
}

#endif

void ReadInput::createGraph() {
  FileNames::iterator it;
  int edgeID = 0;

  for (it = fileNames.begin(); it != fileNames.end(); it++) {
    string name(*it);
    int pos1 = name.find("_", 0);
    if(pos1 == string::npos) {
      cout << "Wrong file name convention: " << name << endl;
      exit(1);
    }
    string name1 = name.substr(0, pos1);
    int pos2 = name.find(".", pos1);
    string name2 = name.substr(pos1+1, pos2-pos1-1);

    addNode(name1, edgeID);
    addNode(name2, edgeID);
    edgeID++;
  }
}

void ReadInput::addNode(string name, int edgeID) {
  Node *nodePtr;
  nodePtr = findNode(name);
  if(nodePtr == NULL) {
    nodePtr = new Node(name);
    nodePtr->addEdgeID(edgeID);
    graph.push_back(nodePtr);
  }
  else {
    nodePtr->addEdgeID(edgeID);
  }
  nodePtr->putOverallCount();
  nodePointers.push_back(nodePtr);
}

Node* ReadInput::findNode(string name) {
  Graph::iterator it;

  for (it = graph.begin(); it != graph.end(); it++) {
    if((*it)->sameName(name))
      return *it;
  }

  return NULL;
}

void ReadInput::readFiles() {
  FileNames::iterator itFN;
  NodePointers::iterator itNP = nodePointers.begin();
  int fileNameIndex = 0;

  for (itFN = fileNames.begin(); itFN != fileNames.end(); itFN++) {
    cout << "Processing file #" << fileNameIndex << " (" << *itFN << ")" << endl;
    // edgeID is the fileName index
    Node *firstNodePtr, *secondNodePtr;
    firstNodePtr = *itNP;
    itNP++;
    secondNodePtr = *itNP;
    itNP++;
    firstNodePtr->addEdge(secondNodePtr);
    secondNodePtr->addEdge(firstNodePtr);

    ifstream in(itFN->c_str());  // Open file
    if(!in) {
      cout << "Can't open file: " << *itFN << endl;
      exit(1);
    }

    readFile(in, firstNodePtr, secondNodePtr, fileNameIndex);
    in.close();
    fileNameIndex++;
  }
}

void ReadInput::readFile(ifstream& in,
			 Node *firstNodePtr, Node *secondNodePtr, int edgeID) {
  const int SZ = 1000; // Buffer size;
  int pairs = 0;
  char buf[SZ];
  while(in.getline(buf, SZ)) { // Removes \n
    pairs++;
    if(pairs % 10000 == 0)
      cout << "Pairs read: " << pairs << endl;
    string str(buf);

    int pos1 = str.find (" ", 0);
    string value1 = str.substr(0, pos1);
    int pos2 = str.find (" ", pos1+1);
    string value2 = str.substr(pos1+1, pos2-pos1-1);

    Count count = (Count)atof(str.substr(pos2+1, str.size()-pos2-1).c_str());
    if(count == 0.0)
      continue;

    Index index1 = firstNodePtr->addValue(value1);
    Index index2 = secondNodePtr->addValue(value2);

    firstNodePtr->putCount(edgeID, index1, index2, count);

    secondNodePtr->putCount(edgeID, index2, index1, count);
  }
}
