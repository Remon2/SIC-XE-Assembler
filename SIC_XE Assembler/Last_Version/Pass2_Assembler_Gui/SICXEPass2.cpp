#include "SICXEPass2.h"
#include "SICXEPass1.h"
#include "Conversions.h"
#include "Tables.h"
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
using namespace std;

Conversions *conve;
Tables *table;
vector<string> modificationRecords, externalDefinitions, externalReferences;
vector<string>::iterator it;
bool createFile = false;
bool header = false;
string currentRecord;
string currentStartingAdd;
string firstExecutable;
string currentLocationCounter;
string error = "";
string base;
bool isBaseUsed;
SICXEPass1 * pass1;
ofstream objectFile;
bool is_error;
string startAddress = "0";
string endAddress = "0";

SICXEPass2::SICXEPass2(string fileName){
	pass1 = new SICXEPass1("input.txt"); //Input file Name, boolean isFixedFormat
	conve = new Conversions();
	table = pass1->getTable();
	objectFile.open("OBJFILE.o");
	counter = 0;
	ctrlSecNum = 1;
	currentRecord = "";
	currentStartingAdd = "";
	currentLocationCounter = "";
}

SICXEPass2::~SICXEPass2() {

}

bool isDefined(string s) {
	vector<vector<string> > extr = pass1->externalReferences;
	int i = 0, j = 0;
	for (i = 0; i < extr.size(); i++) {
		for (j = 0; j < extr.at(i).size(); j++) {
			if (extr.at(i).at(j) == s)
				return true;
		}
	}
	cout << "Undefined Reference" << endl;
	return false;
}

bool belongToExtRef(string s, int cs) {
	vector<vector<string> > extr = pass1->externals;
	vector<string> myExt = extr.at(cs);
	int i = 0;
	for (i = 0; i < myExt.size(); i++) {
		if (myExt.at(i) == s)
			return true;
	}
	cout << "External not declared." << endl;
	return false;
}

bool isNumber2(string s) {
	int i = 0;
	for (i = 0; i < s.length(); i++)
		if (s.at(i) != '0' && s.at(i) != '1' && s.at(i) != '2' && s.at(i) != '3'
				&& s.at(i) != '4' && s.at(i) != '5' && s.at(i) != '6'
				&& s.at(i) != '7' && s.at(i) != '8' && s.at(i) != '9'
				&& s.at(i) != '*')
			return false;
	return true;
}

bool verifyOperand(string s) {

	if (s.at(0) == '@' || s.at(0) == '#')
		s = s.substr(1);
	if (isNumber2(s))
		return true;
	if (s.at(0) == 'C' || s.at(0) == 'c' || s.at(0) == 'X' || s.at(0) == 'x') {
		if (s.length() > 1)
			if (s.at(1) == '\'')
				return true;

	}
	if (table->searchSymTab(s) != "")
		return true;
	else {

		if (table->searchRegTable(s) == -1) {
			if (table->findLiteral(s) != "")
				return true;
			else {
				return false;
			}

		} else
			return true;
	}

}

bool SICXEPass2::pass2() {
    if(	pass1->pass1())
    {

	int lineNum = 0;
	bool started = false;
	string progName = "";
	int numberOfControlSections = 1;
	firstExecutable = "";

	while (conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3), "-1")
			|| conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3), "-2"))
		lineNum++;
	if (conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3), "START")) {
		startAddress = pass1->getListingFile(lineNum, 1);
		progName = pass1->getListingFile(lineNum, 2);
		lineNum++;
	}
	if (!started) {
		started = true;
		firstExecutable = startAddress;
		headerRecord(progName, startAddress,
				pass1->ends.at(numberOfControlSections - 1));
	}
	while (lineNum<(pass1->lineNum-1)) {

		if (!(conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3), "-1")
				|| conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3), "-2")
                || conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3), "-3"))) {

			string arr[100];
			int l = 0;
			getOperands(pass1->getListingFile(lineNum, 5), arr, &l);
			int k = 0;
			for (k = 0; k < l; k++) {
				if (arr[k].length() > 1)
					if (!verifyOperand(arr[k].substr(1)))
						return false;
			}

			getObjectCode(lineNum);
			if (conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3), "RESW")
					|| conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3),
							"RESB"))
				flushTextRecord();
			if (conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3),
					"EXTREF")) {
				int length = 0;
				string* refs = parseOperand(pass1->getListingFile(lineNum, 5),
						&length);
				int index = 0;
				for (index = 0; index < length; index++) {
					addExternalReference(refs[index]);
				}
			} else if (conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3),
					"EXTDEF")) {
				int length = 0;
				string* defs = parseOperand(pass1->getListingFile(lineNum, 5),
						&length);
				int index = 0;
				for (index = 0; index < length; index++) {
					//cout<<defs[index]<<endl;
					//  cout<<table->searchSymTab(defs[index])<<endl;
					addExternalDefinition(defs[index],
							table->searchSymTab(defs[index]));
				}
			} else if (conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3),
					"CSECT")) {
				endRecord();
				newSection();
				started = false;
				startAddress = pass1->getListingFile(lineNum, 1);
				numberOfControlSections++;
				progName = pass1->getListingFile(lineNum, 2);
			}

			if (!started) {

				started = true;
				int n = numberOfControlSections - 1;
				headerRecord(progName, startAddress,
						pass1->ends.at(numberOfControlSections - 1));
			}

			if (eFlag == "1") {

				string arr[100];
				string s = pass1->getListingFile(lineNum, 5);

				//cout<<s<<endl;
				int l = 0;
				if (s.at(0) == '#' || s.at(0) == '@')
					s = s.substr(1);
				getOperands(s, arr, &l);
				//cout<<arr[0].substr(1)<<endl;
				//cout<<table->searchSymTabControlSection(arr[0].substr(1))<<endl;
				//cout<<numberOfControlSections<<endl;
				if (table->searchSymTabControlSection(arr[0].substr(1))
						!= (numberOfControlSections - 1) && !isNumber2(s)) {
					if (belongToExtRef(arr[0].substr(1),
							numberOfControlSections - 1)) {
						string length = "05";
						int addressDec = conve->hexToDec(
								pass1->getListingFile(lineNum, 1)) + 1;
						string address = conve->decToHexa(addressDec);
						//cout<<"address "<<address<<endl;
						//cout<<"length "<<length<<endl;

						string details = arr[0];
						// cout<<"details "<<details<<endl;

						modificationRecord(address, length, details);
					} else {
						cout << "Line: " << lineNum << endl;
						return false;
					}
				} else {
					string length = "05";
					int addressDec = conve->hexToDec(
							pass1->getListingFile(lineNum, 1)) + 1;
					string address = conve->decToHexa(addressDec);
					modificationRecord(address, length, "");
				}
			} else if (conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3),
					"WORD")
					|| conve->eqIgnoreCase(pass1->getListingFile(lineNum, 3),
							"EQU")) {
                if(!isNumber2(pass1->getListingFile(lineNum, 5))){

				string arr[100];
				string s = pass1->getListingFile(lineNum, 5);

				//cout<<s<<endl;
				int l = 0;
				getOperands(s, arr, &l);

				int known = 0;

				for (int i = 0; i < l; i++) {
					//  cout<<arr[i].substr(1)<<endl;
					//  cout<<table->searchSymTabControlSection(arr[i].substr(1))<<endl;

					if (table->searchSymTabControlSection(arr[i].substr(1))
							== (numberOfControlSections - 1)
							|| arr[i].substr(1) == "*") {
						if (arr[i].substr(1) != "*" && l != 1)
							known++;
						else
							continue;
					} else {
						vector<vector<string> > extr = pass1->externals;
						vector<string>::iterator it;
						for (it = extr.at(numberOfControlSections - 1).begin();
								it != extr.at(numberOfControlSections - 1).end();
								it++)
							if (arr[i].substr(1).compare(*it) == 0) {
								string length = "06";
								string address = pass1->getListingFile(lineNum,
										1);
								string details = arr[i];
								//cout<<"address "<<address<<endl;
								// cout<<"length "<<length<<endl;
								modificationRecord(address, length, details);
								//  cout<<"details "<<details<<endl;

							}
					}

				}
				if (known % 2 != 0) {
					string length = "06";
					string address = pass1->getListingFile(lineNum, 1);
					string details = "+"
							+ pass1->starts.at(numberOfControlSections - 1);
					modificationRecord(address, length, details);
				}
				}

			}

		}

		lineNum++;
	}
	endRecord();
	if (objectFile.is_open())
		objectFile.close();
	pass1->generateIntermediateFile();
	cout << "Assembled" << endl;
	return true;
    }else{ cout<<"Error from Pass 1"<<endl; return false;}
}

void SICXEPass2::getObjectCode(int line) {

	string locctr = pass1->getListingFile(line + 1, 1);
	string targetAdress;
	string programCounter;
	string displacement;
	int decimal_displacement;
	string flag;
	int decimal_flag;
	string opcode;
	int decimal_opcode;
	string objectCode = "--";


	if (pass1->getListingFile(line, 3).compare("-1") != 0
			&& pass1->getListingFile(line, 3).compare("-2") != 0
			&& pass1->getListingFile(line, 3).compare("-3") != 0) {

		// Setting eFlag
		if (pass1->getListingFile(line, 3).at(0) == '+')
			eFlag = "1";
		else
			eFlag = "0";

		//Setting Flags (except eFlag)
		setFlags(pass1->getListingFile(line, 5), line);

		pass1->setListingFile(line, 4, niFlag + xFlag + bFlag + pFlag + eFlag);

	} else {
		objectCode = setLiterals(pass1->getListingFile(line, 4));
		if (niFlag == "00") {
			pass1->setListingFile(line, 4, "");

		}

		pass1->setListingFile(line, 7, objectCode);

		if (objectCode == "" || objectCode.at(0) == '-')
			appendTextRecord(pass1->getListingFile(line, 1), objectCode, false);

		else
			appendTextRecord(pass1->getListingFile(line, 1), objectCode, true);

	}


	//Check that this line is not a comment line
	if (pass1->getListingFile(line, 3).compare("-2") != 0
			&& pass1->getListingFile(line, 3).compare("-1") != 0
			&& pass1->getListingFile(line, 3).compare("-3") != 0) {

		string operand = pass1->getListingFile(line, 5);
		int operand_size = operand.size();

		if (table->isDirective(pass1->getListingFile(line, 3))) //If Mnemonic is a directive
				{
			if (conve->eqIgnoreCase(pass1->getListingFile(line, 3), "BASE")) {
				isBaseUsed = true;
				if (isNumber(pass1->getListingFile(line, 5)))
					base = pass1->getListingFile(line, 5);

				else if (pass1->getListingFile(line, 5).at(0) == '@'
						|| pass1->getListingFile(line, 5).at(0) == '#') {
					base = table->searchSymTab(
							pass1->getListingFile(line, 5).substr(1));
				} else {
					base = table->searchSymTab(pass1->getListingFile(line, 5));
				}
				return;
			}

			else if (conve->eqIgnoreCase(pass1->getListingFile(line, 3),
					"NOBASE")) {
				isBaseUsed = false;
				base = "";
				return;
			}

			objectCode = setLiterals(operand);
		}

		else {

			//Check if TIXR, Literals, format 2, format 3 or format 4
			if(pass1->getListingFile(line, 5).compare("*") == 0)
            {
                opcode = table->searchOpTab(pass1->getListingFile(line, 3))
						+ "0000";
				decimal_opcode = conve->hexToDec(opcode);
                flag = conve->binToHex(niFlag + xFlag + bFlag + pFlag + eFlag) + "000";
				decimal_flag = conve->hexToDec(flag);

				programCounter = pass1->getListingFile(line , 1);


				int decimal_objectCode = decimal_flag + decimal_opcode + conve->hexToDec(programCounter);
				objectCode = conve->decToHexa(decimal_objectCode);


            }

			else if (pass1->getListingFile(line, 5).compare("") == 0) {


				opcode = table->searchOpTab(pass1->getListingFile(line, 3))
						+ "0000";
				decimal_opcode = conve->hexToDec(opcode);

				flag = conve->binToHex(niFlag + xFlag + bFlag + pFlag + eFlag)
						+ "000";
				decimal_flag = conve->hexToDec(flag);

				int decimal_objectCode = decimal_opcode + decimal_flag;
				objectCode = conve->decToHexa(decimal_objectCode);
			}

			else if (pass1->getListingFile(line, 5).at(0) == '=') {

				targetAdress = table->findLiteral(
						pass1->getListingFile(line, 5));

				if (!conve->eqIgnoreCase(pass1->getListingFile(line + 1, 3),
						"BASE")
						&& !conve->eqIgnoreCase(
								pass1->getListingFile(line + 1, 3), "LTORG"))
					programCounter = pass1->getListingFile(line + 1, 1);
				else
					programCounter = pass1->getListingFile(line + 2, 1);

				decimal_displacement = conve->hexToDec(targetAdress)
						- conve->hexToDec(programCounter);

				//If displacement is larger than its range, use base register;
				if (decimal_displacement > 2047
						|| decimal_displacement < -2048) {
					if (isBaseUsed) {
						bFlag = "1";
						pFlag = "0";
						decimal_displacement = conve->hexToDec(targetAdress)
								- conve->hexToDec(base);
						if (decimal_displacement > 4095
								|| decimal_displacement < 0) {
							is_error = true;
							pass1->setListingFile(line, 7,
									"Not Enough Space for adress");
							return;
						}
					} else {
						is_error = true;
						cerr << "Error. Not Enough Space for adress" << endl;
						pass1->setListingFile(line, 7,
								"Not Enough Space for adress");

						return;
					}
				} else if (decimal_displacement < 0) {
					char buffer[33];
					sprintf(buffer, "%x", decimal_displacement);
					programCounter = "";
					for (int i = 5; i < 8; i++)
						programCounter = programCounter + buffer[i];
					decimal_displacement = conve->hexToDec(programCounter);
				}

				opcode = table->searchOpTab(pass1->getListingFile(line, 3))
						+ "0000";

				flag = conve->binToHex(niFlag + xFlag + bFlag + pFlag + eFlag)
						+ "000";
				decimal_flag = conve->hexToDec(flag);
				decimal_opcode = conve->hexToDec(opcode);
				int decimal_objectCode = decimal_opcode + decimal_flag
						+ decimal_displacement;
				objectCode = conve->decToHexa(decimal_objectCode);
				while (objectCode.length() < 6)
					objectCode = "0" + objectCode;

			}

			else if (pass1->getListingFile(line, 3).compare("TIXR") == 0
					|| pass1->getListingFile(line, 3).compare("CLEAR") == 0) {
				opcode = table->searchOpTab(pass1->getListingFile(line, 3))
						+ "00";
				decimal_opcode = conve->hexToDec(opcode);
				int r1 = table->searchRegTable(pass1->getListingFile(line, 5))
						* 16;
				int decimal_objectCode = decimal_opcode + r1;
				objectCode = conve->decToHexa(decimal_objectCode);
			} else if (table->isFormat2(pass1->getListingFile(line, 3))) {
				/**
				 Algorithm:
				 ----------
				 Object Code = Opcode * 100 + R1 * 10 + R2

				 */
				int comma_position = pass1->getListingFile(line, 5).find(",");
				opcode = table->searchOpTab(pass1->getListingFile(line, 3))
						+ "00";
				decimal_opcode = conve->hexToDec(opcode);
				int r1 = table->searchRegTable(
						pass1->getListingFile(line, 5).substr(0,
								comma_position)) * 16;
				int r2 = table->searchRegTable(
						pass1->getListingFile(line, 5).substr(
								comma_position + 1, operand_size - 1));
				int decimal_objectCode = decimal_opcode + r1 + r2;
				objectCode = conve->decToHexa(decimal_objectCode);
			}

			else if (pass1->getListingFile(line, 3).find("+") == -1) // Format 3
					{
				if (niFlag.compare("11") == 0)   //Direct Adressing
						{
					if (bFlag.compare("0") == 0 && pFlag.compare("0") == 0) // Constant is used
							{
						decimal_displacement = directAddressConstant(operand,
								line);
					}

					else    //Label is used
					{

						targetAdress = directAddressLabel(operand, line);

                if (!conve->eqIgnoreCase(pass1->getListingFile(line + 1, 3), "BASE") &&
                    !conve->eqIgnoreCase(pass1->getListingFile(line + 1, 3), "LTORG"))

							programCounter = pass1->getListingFile(line + 1, 1);
						else
							programCounter = pass1->getListingFile(line + 2, 1);

                decimal_displacement = conve->hexToDec(targetAdress) - conve->hexToDec(programCounter);

                //If displacement is larger than its range, use base register;
                if (decimal_displacement > 2047 || decimal_displacement < -2048) {
                    if (isBaseUsed) {
                        bFlag = "1";
                        pFlag = "0";
                        decimal_displacement = conve->hexToDec(
                                targetAdress) - conve->hexToDec(base);
                        if (decimal_displacement > 4095
                                || decimal_displacement < 0) {
                            is_error = true;
                            pass1->setListingFile(line, 7,
                                    "Not Enough Space for adress");
                            cerr << "Error. Not Enough Space for adress"
                                    << endl;
                            return;
                        }
                    } else {
                        is_error = true;
                        pass1->setListingFile(line, 7,
                                "Not Enough Space for adress");
								cerr << "Error. Not Enough Space for adress"
										<< endl;
								return;
							}
						}

						else if (decimal_displacement < 0) {
							char buffer[33];
							sprintf(buffer, "%x", decimal_displacement);
							programCounter = "";
							for (int i = 5; i < 8; i++)
								programCounter = programCounter + buffer[i];
							decimal_displacement = conve->hexToDec(
									programCounter);
						}
					}
				} else if (niFlag.compare("01") == 0
						|| niFlag.compare("10") == 0) //Immendiate or Indirect Adressing
								{
					if (bFlag.compare("0") == 0 && pFlag.compare("0") == 0) // Constant is used
                    {
						if(pass1->getListingFile(line, 5).at(0) == '#')
                        {
                          	decimal_displacement = conve->convertToInteger(
								pass1->getListingFile(line, 5).substr(1));

                        }
                        else
                        {
                        decimal_displacement = conve->hexToDec(
								pass1->getListingFile(line, 5).substr(1));
                        }


					} else  //Label is used
					{

						targetAdress = table->searchSymTab(
								pass1->getListingFile(line, 5).substr(1));

						if (!conve->eqIgnoreCase(
								pass1->getListingFile(line + 1, 3), "BASE")
								&& !conve->eqIgnoreCase(
										pass1->getListingFile(line + 1, 3),
										"LTORG"))
							programCounter = pass1->getListingFile(line + 1, 1);
						else
							programCounter = pass1->getListingFile(line + 2, 1);

						decimal_displacement = conve->hexToDec(targetAdress)
								- conve->hexToDec(programCounter);
						//If displacement is larger than its range, use base register;
						if (decimal_displacement > 2047
								|| decimal_displacement < -2048) {
							if (isBaseUsed) {
								bFlag = "1";
								pFlag = "0";
								decimal_displacement = conve->hexToDec(
										targetAdress) - conve->hexToDec(base);
								if (decimal_displacement > 4095
										|| decimal_displacement < 0) {

									is_error = true;
									pass1->setListingFile(line, 7,
											"Not Enough Space for adress");
									cerr << "Error. Not Enough Space for adress"
											<< endl;
									return;
								}
							} else {
								is_error = true;
								pass1->setListingFile(line, 7,
										"Not Enough Space for adress");
								cerr << "Error. Not Enough Space for adress"
										<< endl;
								return;
							}
						} else if (decimal_displacement < 0) {
							char buffer[33];
							sprintf(buffer, "%x", decimal_displacement);
							programCounter = "";
							for (int i = 5; i < 8; i++) {
								programCounter = programCounter + buffer[i];
							}
							decimal_displacement = conve->hexToDec(
									programCounter);
						}
					}
				}

				flag = conve->binToHex(niFlag + xFlag + bFlag + pFlag + eFlag)
						+ "000";
				decimal_flag = conve->hexToDec(flag);
				opcode = table->searchOpTab(pass1->getListingFile(line, 3))
						+ "0000";
				decimal_opcode = conve->hexToDec(opcode);
				int decimal_objectCode = decimal_opcode + decimal_flag
						+ decimal_displacement;
				objectCode = conve->decToHexa(decimal_objectCode);
				while (objectCode.length() < 6)
					objectCode = "0" + objectCode;
			}

			else    // Format 4
			{
				if (niFlag.compare("11") == 0)   //Direct Adressing
						{
					if (isNumber(pass1->getListingFile(line, 5))) // Constant is used
							{
						decimal_displacement = directAddressConstant(operand,
								line);
					} else    //Label is used
					{
						displacement = directAddressLabel(operand, line);
						decimal_displacement = conve->hexToDec(displacement);
					}
				} else if (niFlag.compare("01") == 0
						|| niFlag.compare("10") == 0) //Immendiate or
													  //Indirect Adressing
								{
					 if(isNumber(pass1->getListingFile(line, 5))) //constant
                  {
                      if(pass1->getListingFile(line, 5).at(0) == '#')
                        {
                          	decimal_displacement = conve->convertToInteger(
								pass1->getListingFile(line, 5).substr(1));

                        }
                        else
                        {
                        decimal_displacement = conve->hexToDec(
								pass1->getListingFile(line, 5).substr(1));
                        }

                  }
                  else   //label
                  {
                      displacement = table->searchSymTab(
							pass1->getListingFile(line, 5).substr(1));
					decimal_displacement = conve->hexToDec(displacement);
                  }

                }

				flag = conve->binToHex(niFlag + xFlag + bFlag + pFlag + eFlag)
						+ "00000";
				decimal_flag = conve->hexToDec(flag);
				opcode = table->searchOpTab(

				pass1->getListingFile(line, 3).substr(1)) + "000000";
				decimal_opcode = conve->hexToDec(opcode);
				int decimal_objectCode = decimal_displacement + decimal_flag
						+ decimal_opcode;
				objectCode = conve->decToHexa(decimal_objectCode);
				while (objectCode.length() < 8)
					objectCode = "0" + objectCode;
			}

		}


		if (niFlag == "00") {
			pass1->setListingFile(line, 4, "");

		}

		pass1->setListingFile(line, 7, objectCode);

		if (objectCode == "" || objectCode.at(0) == '-')
			appendTextRecord(pass1->getListingFile(line, 1), objectCode, false);

		else
			appendTextRecord(pass1->getListingFile(line, 1), objectCode, true);


	}

}

bool SICXEPass2::isNumber(string operand) {
	if ((int) operand.at(1) >= 48/** Min value of number*/
	&& (int) operand.at(1) <= 57 /** Max value*/) {
		return true;
	}

	return false;

}

void SICXEPass2::setFlags(string operand, int line) {

	if (operand.compare("") == 0 || pass1->getListingFile(line, 5).compare("*") == 0) {
		niFlag = "11";
		xFlag = "0";
		bFlag = "0";
		pFlag = "0";
		eFlag = "0";

		return;
	}

	if (table->isDirective(pass1->getListingFile(line, 3))
			|| table->isFormat2(pass1->getListingFile(line, 3))) {

		niFlag = "00";
		xFlag = "0";
		bFlag = "0";
		pFlag = "0";
		eFlag = "0";
		return;
	}

	if (operand.at(0) == '@')    //Indirect Adressing
			{
		niFlag = "10";

	} else if (operand.at(0) == '#')     //Immediate Adressing
			{
		niFlag = "01";

	} else  //Direct Adressing
	{
		niFlag = "11";

		if (operand.find(",") != -1 && operand.find("X") != -1) {
			xFlag = "1";
		} else {
			xFlag = "0";

		}

	}

	if (isNumber(operand) || eFlag.compare("1") == 0) {
		if (xFlag.compare("") == 0)
			xFlag = "0";

		bFlag = "0";
		pFlag = "0";
	} else {
		if (xFlag.compare("") == 0)
			xFlag = "0";

		bFlag = "0";
		pFlag = "1";

	}

}

string SICXEPass2::setLiterals(string operand) {

	if (operand.size() == 0)
		return "---";

	int i = 0;
	string objectCode = "";
	int operand_size = operand.size();

	if (operand.at(i) == '=')
		i++;

	if (operand.at(i) == 'C') {
		i += 2;

		while (i < operand_size - 1) {

			objectCode = objectCode + conve->decToHexa((int) operand.at(i));

			i++;
		}

	} else if (operand.at(0) == 'X') {
		int i = 2;
		while (i < operand_size - 1) {
			objectCode = objectCode + operand.at(i);
			i++;
		}

	} else {
		objectCode = "-----";

	}
	return objectCode;

}

int SICXEPass2::directAddressConstant(string operand, int line) {

	if (xFlag.compare("1") == 0)    // Indexed
			{
		return conve->hexToDec(
				pass1->getListingFile(line, 5).substr(0,
						pass1->getListingFile(line, 5).size() - 2));

	} else {
		return conve->hexToDec(operand);

	}

}

string SICXEPass2::directAddressLabel(string operand, int line) {
	if (xFlag.compare("1") == 0)    // Indexed
    {
     	return table->searchSymTab(
				pass1->getListingFile(line, 5).substr(0,
						pass1->getListingFile(line, 5).size() - 2));

	} else {
		return table->searchSymTab(operand);

	}

}

// this method to parse string such as "BUFFER , BUFEND , LENGTH"
//and will put it in array like this
//result[]={"BUFFER" , "BUFEND" , "LENGTH"};
string* SICXEPass2::parseOperand(string def, int *length) {
	string* result = new string[100];
	string str = "";
	int counterOfDefs = 0;
	for (int i = 0; i < def.length(); i++) {
		if (def.at(i) == ' ')
			continue;
		if (def.at(i) != ',') {
			str += def.at(i);
		} else if (def.at(i) == ',') {
			result[counterOfDefs] = str;
			counterOfDefs++;
			str = "";
		}
	}
	result[counterOfDefs] = str;
	counterOfDefs++;
	*length = counterOfDefs;
	return result;
}

void SICXEPass2::getOperands(string s, string arr[], int* length) {
	string str = "";
	int counter = 0;
	int i = 0;
	if (s[0] == '-')
		str = "-";
	else
		str = "+";
	for (i = 0; i < s.length(); i++) {
		if (i == 0 && s[i] == '-') {

		} else {
			if (s[i] == ',') {
				break;
			}
			if (s[i] == '+') {
				arr[counter] = str;
				counter++;
				str = "+";
			} else if (s[i] == '-') {
				arr[counter] = str;
				counter++;
				str = "-";

			} else {
				str = str + s[i];
			}
		}
	}
	*length = counter + 1;
	arr[counter] = str;
}

string stretchString(string str, int space) {
	if (str.length() == 0 || str.compare("") == 0)
		return "      ";
	if (str.length() > space) //length of the string is greater than the space then return it again
		return str;
	int numberOfSpacesLost = space - str.length();
	for (int i = 0; i < numberOfSpacesLost; i++) {
		str = " " + str;
	}
	return str;
}

int SICXEPass2::numberOfOperands(string s) {
	int i = 0;
	int n = 1;
	for (i = 0; i < s.length(); i++) {
		if (s[i] == '-' || s[i] == '+')
			n++;
	}
	return n;
}

bool SICXEPass2::destroyFile(string message) {
	objectFile.close();
	objectFile.open("OBJFILE.o");
	objectFile << "";
	objectFile.close();
	currentRecord = "";
	error = message;
	cerr << error << endl;
	return false;
}

void SICXEPass2::newSection() {
	if (objectFile.is_open())
		objectFile << endl << endl << endl;
}

void SICXEPass2::flushRecord() {
	if (currentRecord.length() > 73)
		destroyFile("Invalid Record length");
	if (objectFile.is_open())
		objectFile << currentRecord << endl;
	currentRecord = "";
	currentStartingAdd = "";
}

bool SICXEPass2::headerRecord(string programName, string startAdd,
		string endAdd) {
	if (programName.length() > 6)
		return destroyFile(
				"Invalid Program name length [must be between 0-6 inclusive]");
	string temp = currentRecord;
	string tempAddr = currentStartingAdd;
	currentRecord = "H";

	for (int i = programName.length(); i < 6; i++)
		programName += " ";

	currentRecord += programName + "^";

	if (startAdd.compare("") == 0 || startAdd.length() > 6
			|| conve->hexToDec(startAdd) >= pow(2.0, 20.0)
			|| conve->hexToDec(startAdd) < 0)
		return destroyFile("Invalid starting address");

	for (int i = startAdd.length(); i < 6; i++)
		startAdd = "0" + startAdd;

	startAddress = startAdd;

	for (int i = endAdd.length(); i < 6; i++)
		endAdd = "0" + endAdd;

	endAddress = endAdd;

	currentRecord += startAdd + "^";
	if (endAdd.compare("") == 0 || conve->hexToDec(endAdd) >= pow(2.0, 20.0)) {
		return destroyFile("Invalid end address");
		return false;
	}

	string length = conve->decToHexa(
			conve->hexToDec(endAdd) - conve->hexToDec(startAdd));
	if (length.compare("") == 0 || length.length() > 6
			|| conve->hexToDec(startAdd) >= pow(2.0, 20.0)
			|| conve->hexToDec(length) < 0)
		return destroyFile("Invalid program length");

	for (int i = length.length(); i < 6; i++)
		length = "0" + length;
	currentRecord += length;
	if (currentRecord.length() > 22)
		return destroyFile("Invalid Record length");
	flushRecord();
	header = true;

	if (externalDefinitions.size()) {
		currentRecord = "D";
		for (it = externalDefinitions.begin(); it < externalDefinitions.end();
				it++)
			currentRecord += *it;
		if (currentRecord.length() > 73)
			return destroyFile("Too many External Definitions");
		flushRecord();
	}

	if (externalReferences.size()) {
		currentRecord = "R";
		for (it = externalReferences.begin(); it < externalReferences.end();
				it++)
			currentRecord += *it;
		if (currentRecord.length() > 73)
			return destroyFile("Too many External References");
		flushRecord();
	}
	externalDefinitions.clear();
	externalReferences.clear();
	currentRecord = temp;
	currentStartingAdd = tempAddr;
	return true;
}

bool SICXEPass2::flushTextRecord() {
	if (currentRecord.compare("") == 0)
		return false;
	int length = currentRecord.length();
	if (currentRecord.length() % 2 == 1)
		return destroyFile("invalid object code generated");
	length = length / 2;
	string len = conve->decToHexa(length);
	if (len.length() > 2)
		return destroyFile("invalid record length generation");

	for (int i = len.length(); i < 2; i++)
		len = "0" + len;
	currentRecord = "T" + currentStartingAdd + "^" + len + "^" + currentRecord;
	flushRecord();
	currentRecord = "";
	currentStartingAdd = "";
	return true;
}

bool SICXEPass2::appendTextRecord(string locationCounter, string objectCode,
		bool executable) {
	if (objectCode.compare("") != 0 && objectCode.at(0) == '-')
		return false;
	if (objectCode.length() % 2 == 1)
		return destroyFile("invalid object code generated");

	if (locationCounter.compare("") == 0 || locationCounter.length() > 6
			|| conve->hexToDec(locationCounter) >= pow(2.0, 20.0)
			|| conve->hexToDec(locationCounter) < 0)
		return destroyFile("Invalid instruction address");
	for (int i = locationCounter.length(); i < 6; i++)
		locationCounter = "0" + locationCounter;
	if (currentStartingAdd.compare("") == 0)
		currentStartingAdd = locationCounter;

	if (currentRecord.length() + objectCode.length() > 60) {
		flushTextRecord();
		currentRecord = objectCode;
		currentStartingAdd = locationCounter;
	}
	currentRecord += objectCode;
	if (executable && firstExecutable.compare("") == 0) {
		firstExecutable = locationCounter;
		for (int i = firstExecutable.length(); i < 6; i++)
			firstExecutable = "0" + firstExecutable;
	}
}

bool SICXEPass2::endRecord() {
	if (!header)
		return destroyFile("invalid program structure");
	if (firstExecutable.compare("") == 0 || firstExecutable.length() > 6
			|| conve->hexToDec(firstExecutable) > pow(2.0, 20.0)
			|| conve->hexToDec(firstExecutable) < 0)
		return destroyFile("Invalid Starting location address");
	if (currentRecord != "")
		flushTextRecord();
	for (it = modificationRecords.begin(); it < modificationRecords.end();
			it++) {
		currentRecord = *it;
		flushRecord();
	}
	modificationRecords.clear();
	for (int i = firstExecutable.length(); i < 6; i++)
		firstExecutable = "0" + firstExecutable;
	currentRecord = "E" + firstExecutable;
	firstExecutable = "";
	flushRecord();
}

bool SICXEPass2::modificationRecord(string address, string length,
		string details) {
	if (!header)
		return destroyFile("invalid program structure");

	for (int i = address.length(); i < 6; i++)
		address = "0" + address;
	if (address.length() > 6 || address.compare(startAddress) < 0 || address.compare(endAddress) > 0)
		return destroyFile("Invalid address in a modification record");

	for (int i = length.length(); i < 2; i++)
		length = "0" + length;

	if (length.length() > 2
			|| (length.compare("05") != 0 && length.compare("06") != 0))
		return destroyFile("Invalid length in half bytes");

	if (length.compare("06") == 0
			&& (details.compare("") == 0
					|| (details.at(0) != '+' && details.at(0) != '-')))
		return destroyFile("Invalid modification record");
	modificationRecords.push_back("M" + address + "^" + length + details);
}

bool SICXEPass2::addExternalDefinition(string name, string address) {
	if (address.compare("") == 0 || address.length() > 6
			|| conve->hexToDec(address) > pow(2.0, 20.0)
			|| conve->hexToDec(address) < 0)
		return destroyFile("invalid address");
	for (int i = address.length(); i < 6; i++)
		address = "0" + address;
	if (name.compare("") == 0 || name.length() > 6)
		return destroyFile("Invalid Definition name");
	for (int i = name.length(); i < 6; i++)
		name += " ";

	externalDefinitions.push_back(name + address);
}

bool SICXEPass2::addExternalReference(string name) {
	if (name.compare("") == 0 || name.length() > 6)
		return destroyFile("Invalid Definition name");
	for (int i = name.length(); i < 6; i++)
		name += " ";
	externalReferences.push_back(name);
}
