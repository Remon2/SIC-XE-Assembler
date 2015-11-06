#ifndef TABLES_H
#define TABLES_H

#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "stdlib.h"

using namespace std;

class Tables
{
    /*Hash Tables*/
    map<string, string >  symTabControlSection;

    map<string, int> registersTable;
    set<string> directivesSet;
    map<string, string> opTab;



public:
    int sectionNum;
    Tables();
    virtual ~Tables();

    map<string, vector<string> >  symTab;
    string LITTAB[1000][4];

    /*Registers HashTable*/
    void createRegTable();
    int searchRegTable(string reg);

    /*Directives HashSet*/
    void createDirectivesSet();
    bool isDirective(string directive);

    /*Sympols Codes HashTable*/
    void insertSymTab(string symbol, string location);
    void increaseSectionNumber();
    string searchSymTab(string symbol);
    int searchSymTabControlSection(string symbol);
    void insertSymTabCSECT(string symbol, string location);

    /*Literals Table*/
    bool insertLITTAB(string name);
    bool isFoundLITTAB(string name);
    bool insertLITTAB(string name, string value);

    /*Operation Codes HashTable*/
    void createOpTab();
    string searchOpTab(string opCode);

    bool isFormat2(string instruction);
    string findLiteral(string name);


protected:
private:
};

#endif // TABLES_H

