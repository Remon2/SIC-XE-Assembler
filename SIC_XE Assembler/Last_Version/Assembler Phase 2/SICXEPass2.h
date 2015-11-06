#ifndef SICXEPASS2_H
#define SICXEPASS2_H

#include "Conversions.h"
#include "SICXEPass1.h"
#include "Tables.h"
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
using namespace std;

class SICXEPass2 {
public:
	string niFlag, xFlag, bFlag, pFlag, eFlag;
	int ctrlSecNum;
	string* parseOperand(string def, int *length);

	SICXEPass2(string fileName);
	virtual ~SICXEPass2();
	void getObjectCode(int line);
	void setFlags(string operand,int line);
	bool pass2();
protected:
private:
	int counter;
	Tables * t;
	bool isNumber(string operand);
	void getOperands(string s, string arr[], int* length);
	string setLiterals(string operand);
	int directAddressConstant(string operand, int line);
	string directAddressLabel(string operand, int line);
	bool generateModificationRecords();
	bool generateDefinitions();
	bool generateReferences();
	int numberOfOperands(string s);
	bool destroyFile(string message);
	void newSection();
	void flushRecord();
	bool headerRecord(string programName, string startAdd, string endAdd);
	bool flushTextRecord();
	bool appendTextRecord(string locationCounter, string objectCode, bool executable);
	bool endRecord();
	bool modificationRecord(string address, string length, string details);
	bool addExternalDefinition(string name, string address);
	bool addExternalReference(string name);
};

#endif // SICXEPASS2_H