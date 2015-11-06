#ifndef SICXEPASS1_H
#define SICXEPASS1_H

#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <fstream>
#include <vector>
#include <algorithm>
#include "Tables.h"


using namespace std;

class SICXEPass1
{

    string line , label, operation, operand, temp, inputFile;
    string listingFile [1000][9];
    bool isStarted, isEnded, isBased;


    stringstream output;
    istringstream instruction;

public:
    string locctr;
    int lineNum;

    vector<string> ends;
    vector<string> starts;
    vector<vector<string> > externalReferences;
    vector<vector<string> > externals;
    SICXEPass1();
    SICXEPass1(string fileInput);
    virtual ~SICXEPass1();
    bool pass1();
    void generateIntermediateFile();
    string evaluateExpression(string expression);

    /**Extra Methods*/
bool isExternalReference(string lb);

    string getListingFile(int line, int column);
    void setListingFile(int line,int column, string objectCode);
    Tables * getTable();

protected:
private:
    bool checkOperandCharacters(string operand);
    void goThroughDelimeters(string line);
    char nextToken(string line);
    bool isComment(string line);
    string goThroughWords(string line, bool delimeters);
    string tokenExtraction(string line, bool delimeters,bool plus);
    string deleteReturn(string str);
    int verifyLine(string line);
    vector<string> split(string s);
    bool checkOperands(string operation, string operands);
    vector<string> splitTerms(string expressions);
    bool isLiteralFoundListingFile(int lineNum, string literal);
    vector<string> splitOperators(string expression);
};

#endif // SICXEPASS1_H
