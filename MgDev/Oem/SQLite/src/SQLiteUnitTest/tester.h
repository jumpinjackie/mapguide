#ifndef TESTER_H
#define TESTER_H

#include <fstream>
#include <stdio.h>
#include <string>

#define TESTNAME L"SQLiteUnitTest"

#ifdef WIN32
#include "tester-windows.h"
#else
#include "tester-linux.h"
#endif

using namespace std;

wstring BuildFileName(wstring rootFileName);
int WriteResultsToFile(ofstream &out);
char* ConvertToCharArray(wstring &theStr);
wstring ConvertToWstring(char* theStr);
wstring GetName();

#endif
