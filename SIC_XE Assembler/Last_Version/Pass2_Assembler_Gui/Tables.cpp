#include "Tables.h"
#include "Conversions.h"

using namespace std;

Conversions *conv;

Tables::Tables()
{
    sectionNum=0;
    conv = new Conversions();
    createDirectivesSet();
    createOpTab();
    createRegTable();
}
Tables::~Tables() {}

void Tables::createRegTable()
{
    registersTable["A"] =0;
    registersTable["X"] =1;
    registersTable["L"] =2;
    registersTable["B"] =3;
    registersTable["S"] =4;
    registersTable["T"] =5;
    registersTable["F"] =6;
    registersTable["PC"] =8;
    registersTable["SW"] =9;
}

int Tables::searchRegTable(string reg)
{
    if(reg=="0"||reg=="1"||reg=="2"||reg=="3"||reg=="4"||reg=="5"||reg=="6"||reg=="7"||reg=="8"||reg=="9")
        return (conv->convertToInteger(reg));
    int result = registersTable[conv->capitalize(reg)];
    if(result == 0&&!conv->eqIgnoreCase(reg,"A"))
        return -1;
    else
        return result;
}

void Tables::createDirectivesSet()
{
    directivesSet.insert("START");
    directivesSet.insert("END");
    directivesSet.insert("BYTE");
    directivesSet.insert("WORD");
    directivesSet.insert("RESB");
    directivesSet.insert("RESW");
    directivesSet.insert("EQU");
    directivesSet.insert("ORG");
    directivesSet.insert("LTORG");
    directivesSet.insert("BASE");
    directivesSet.insert("NOBASE");
    directivesSet.insert("EXTDEF");
    directivesSet.insert("EXTREF");
    directivesSet.insert("CSECT");
}

bool Tables::isDirective(string directive)
{
    set<string>::const_iterator got = directivesSet.find (conv->capitalize(directive));
    if (got == directivesSet.end())
        return false;
    else
        return true;
}

void Tables::insertSymTab(string symbol, string location)
{
    vector<string> ar;
    ar.push_back(location);
    ar.push_back(conv->convertToString(sectionNum));
    symTab[conv->capitalize(symbol)] = ar;
}

void Tables::insertSymTabCSECT(string symbol, string location)
{
    vector<string> ar;
    ar.push_back(location);
    ar.push_back(conv->convertToString(sectionNum+1));
    symTab[conv->capitalize(symbol)] = ar;
}

void Tables::increaseSectionNumber()
{
    sectionNum++;
}

string Tables::searchSymTab(string symbol)
{
    vector<string> arr = symTab[conv->capitalize(symbol)];
    if(arr.size()>0)
        return arr.at(0);
    else
        return "";
}

int Tables::searchSymTabControlSection(string symbol)
{
    vector<string> arr = symTab[conv->capitalize(symbol)];
    if(arr.size()>1)
        return conv->convertToInteger(arr.at(1));
    else
        return -1;
}


string Tables::searchOpTab(string opCode)
{
    opCode = conv->capitalize(opCode);
    if(opCode.at(0)!='+')
        return opTab[opCode];
    else
        return opTab[opCode.substr(1, opCode.size()-1)];
}


void Tables::createOpTab()
{
    opTab["ADD"]="18";      opTab["MULF"]="60";
    opTab["ADDF"]="58";     opTab["MULR"]="98";
    opTab["ADDR"]="90";     opTab["NORM"]="C8";
    opTab["AND"]="40";      opTab["OR"]="44";
    opTab["CLEAR"]="B4";    opTab["RD"]="D8";
    opTab["COMP"]="28";     opTab["RMO"]="AC";
    opTab["COMPF"]="88";    opTab["RSUB"]="4C";
    opTab["COMPR"]="A0";    opTab["SHIFTL"]="A4";
    opTab["DIV"]="24";      opTab["SHIFTR"]="A8";
    opTab["DIVF"]="64";     opTab["SIO"]="F0";
    opTab["DIVR"]="9C";     opTab["SSK"]="EC";
    opTab["FIX"]="C4";      opTab["STA"]="0C";
    opTab["FLOAT"]="C0";    opTab["STB"]="78";
    opTab["HIO"]="F4";      opTab["STCH"]="54";
    opTab["J"]="3C";        opTab["STF"]="80";
    opTab["JEQ"]="30";      opTab["STI"]="D4";
    opTab["JGT"]="34";      opTab["STL"]="14";
    opTab["JLT"]="38";      opTab["STS"]="7C";
    opTab["JSUB"]="48";     opTab["STSW"]="E8";
    opTab["LDA"]="00";      opTab["STT"]="84";
    opTab["LDB"]="68";      opTab["STX"]="10";
    opTab["LDCH"]="50";     opTab["SUB"]="1C";
    opTab["LDF"]="70";      opTab["SUBF"]="5C";
    opTab["LDL"]="08";      opTab["SUBR"]="94";
    opTab["LDS"]="6C";      opTab["SVC"]="94";
    opTab["LDT"]="74";      opTab["TD"]="E0";
    opTab["LDX"]="04";      opTab["TIO"]="F8";
    opTab["LPS"]="D0";      opTab["TIX"]="2C";
    opTab["MUL"]="20";      opTab["TIXR"]="B8";
                opTab["WD"]="DC";
}


bool Tables::insertLITTAB(string name)
{
    if(conv->isLiteral(name))
    {
        int i =0;
        string value , length;
        if(name.at(1)=='C'||name.at(1)=='c')
            value = conv->asciiToHex(name.substr(3));
        else
            value = name.substr(3,name.size()-4);
        length = conv->convertToString(name.size()-4);
        for(i=0;i<1000&&LITTAB[i][0]!="\0";i++)
            if(LITTAB[i][1]==value)
                break;
        LITTAB[i][0] = name;
        LITTAB[i][1] = value;
        LITTAB[i][2] = length;
        LITTAB[i][3] = "";
        return true;
    }
    else
        return false;
}

bool Tables::insertLITTAB(string name, string value)
{
    int i;
    string length="";
    if(name!="=*")
        length = conv->convertToString(name.size()-4);
    else
        length= conv->convertToString(name.size()-1);
    for(i=0;i<1000&&LITTAB[i][0]!="\0";i++)
            if(LITTAB[i][1]==value)
                return false;
    LITTAB[i][0] = name;
    LITTAB[i][1] = value;
    LITTAB[i][2] = length;
    LITTAB[i][3] = "";
    return true;
}

bool Tables::isFoundLITTAB(string name)
{
    if(conv->isLiteral(name))
    {
        string value="";
        if(name.at(1)=='C'||name.at(1)=='c')
            value = conv->asciiToHex(name.substr(3));
        else
            value = name.substr(3,name.size()-4);
        for(int i=0;i<1000&&LITTAB[i][0]!="\0";i++)
            if(LITTAB[i][1]==value)
                return true;
    }
    else
        return false;
}

string Tables::findLiteral(string name)
{
                  int s = name.size()-1;


    if(conv->isLiteral(name))
    {
        string value="";
        if(name.at(1)=='C'||name.at(1)=='c')
        {
            value = conv->asciiToHex(name.substr(3));

        }
        else
        {
            value = name.substr(3,name.size()-4);

        }
        for(int i=0;i<1000&&LITTAB[i][0]!="\0";i++){

            if(LITTAB[i][0]==name)
            {

                return LITTAB[i][3];
            }
        }
        return "";
    }
    else
        return "";
}


bool Tables::isFormat2(string instruction)
{
    if(instruction.compare("TIXR") == 0 || instruction.compare("CLEAR") == 0 ||
       instruction.compare("RMO") == 0 || instruction.compare("ADDR") == 0 ||
       instruction.compare("SUBR") == 0 || instruction.compare("MULR") == 0 ||
       instruction.compare("DIVR") == 0 || instruction.compare("COMPR") == 0)
    {
               return true;
    }

    return false;

}
