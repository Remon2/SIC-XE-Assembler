#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <iomanip>
#include <bitset>
#include "stdlib.h"

using namespace std;

class Conversions
{
    public:
        Conversions();
        virtual ~Conversions();

        /**Conversions For Pass2*/
         int hexToDec(string hexa);
         string hexToBin(string hexa);
         int binToDec(string rvrs);
         string binToHex(string binary);


        /* String Handling*/
        string capitalize(string small);
        bool eqIgnoreCase(string str1, string str2);
        string expandHexa(string hexa, int length);
        string expandString(string line, int length);
        string convertToString(int number);
        int convertToInteger(string num);
        string asciiToHex(string c);

        /*Hexa Vs. Decimal*/
        string decToHexa(int num);
        int hexaToDec(const char* hexStr);


        /*Locctr Operations*/
        //Get the locctr in Hexadecimal and the operand in decimal and make the given operation overit which will be
        // Addition or subtraction and returns the new locctr
        string operateLocctr(string locctr, char oper, int operand);

        /*Check Literal Form, but has no checking on LITTAB*/
        bool isLiteral(string name);

    protected:
    private:
};
#endif // CONVERSIONS_H
