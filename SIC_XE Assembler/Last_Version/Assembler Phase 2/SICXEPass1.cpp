#include "SICXEPass1.h"
#include "Conversions.h"
#include "Tables.h"

using namespace std;

/*
 Design Decision:
 ==================

 1) Listing File Array will be implemented as following:
 listingFile[0] -> Line Number
 listingFile[1] -> Location Counter
 listingFile[2] -> Label _If found_
 listingFile[3] -> Operation Code
 listingFile[4] -> Flags
 listingFile[5] -> Operands
 listingFile[6] -> Comments _If found_
 listingFile[7] -> Object Code for pass2
 listingFile[8] -> Modification Boolean for pass2

 2) Comment Lines will be put at listingFile[3] and the listingFile[3] will be set to "-2"

 3) Invalidl Lines will be put at listingFile[3] and the listingFile[3] will be set to "-1"

 4)Verify Line is an integer Function returns 0 if Comment Line, 1 if Invalid Line , 2 if validLine

 5)Split is a Vector of String Function (normally) returns values of
 field[0] -> Label if exist
 field[1] -> Operation Code , or other conventions
 field[2] -> Operands if exist
 field[3] -> Comment if exist

 6)Split Function returns at field[1] a -1 if -2 if Comment Line, -2 if Invalid Line

 7)evaluateExpression Function returns "error" if the expression is invalid, otherwise returns the locctr pointed by
 this expression as a string in all cases :) .

 8) Assumption of END line to be the last line of the intermediate file
 */

Conversions *c;
Tables *t;

SICXEPass1::SICXEPass1() {
}

SICXEPass1::SICXEPass1(string fileInput) {
	inputFile = fileInput; //will be got from the user
	locctr = "0000";
	lineNum = 0;
	isBased = false;
	isStarted = false;
	isEnded = false;
	c = new Conversions();
	t = new Tables();
}
SICXEPass1::~SICXEPass1() {
}

bool SICXEPass1::pass1() {
	bool isValid = true;
	ifstream file(inputFile.c_str());
	if (!file.is_open()) {
		cerr << "couldn't open Input file\n";
		return false;
	} else {
		while (!file.eof()) {
			getline(file, line);
			istringstream instruction(line);
			line.erase(remove(line.begin(), line.end(), '\n'), line.end());
			int result = verifyLine(line);
			if (result == 2) //Not a comment Line or Invalid Line or a Literal, either a directive or operation line
					{
				//First Capitalize all the Inputs to avoid Case Sensetivity
				label = c->capitalize(listingFile[lineNum][2]);
				operation = c->capitalize(listingFile[lineNum][3]);
				if (label != "" && operation != "CSECT")
					t->insertSymTab(label, (c->expandHexa(locctr, 4)));
				else if (label != "" && operation == "CSECT")
					t->insertSymTabCSECT(label, (c->expandHexa(locctr, 4)));

				operand = c->capitalize(listingFile[lineNum][5]);
				if (!t->isDirective(operation)
						&& t->searchOpTab(operation) != "") //Normal Operation Line Not a directive
								{
					if (operation.at(0) == '+') //Format 4
						temp = operation.substr(1);
					else
						temp = operation;
					//Now temp holds the operation itself
					if (operation.at(0) == '+') //4 bytes instructions
						locctr = c->operateLocctr(locctr, '+', 4);
					else if (c->eqIgnoreCase(temp, "ADDR")
							|| c->eqIgnoreCase(temp, "CLEAR")
							|| c->eqIgnoreCase(temp, "COMPR")
							|| c->eqIgnoreCase(temp, "DIVR")
							|| c->eqIgnoreCase(temp, "MULR")
							|| c->eqIgnoreCase(temp, "RMO")
							|| c->eqIgnoreCase(temp, "SUBR")
							|| c->eqIgnoreCase(temp, "TIXR")) //2 bytes instructions
						locctr = c->operateLocctr(locctr, '+', 2);
					else
						//3 bytes instructions
						locctr = c->operateLocctr(locctr, '+', 3);
				} else //Directive
				{
					if (operation == "START") //The START line
							{
						if (operand != "")
							locctr = operand;
						else
							locctr = "0000";
						locctr = c->expandHexa(locctr, 4);
						listingFile[lineNum][0] = c->convertToString(lineNum);
						listingFile[lineNum][1] = c->expandHexa(locctr, 4);
						isStarted = true;
						starts.push_back(label);
					} else if (operation == "BYTE") {
						int size = operand.size();
						int ch = operand.find("C'");
						int hex = operand.find("X'");
						if (ch == 0 && hex != 0) //Size - C'' three bytes :)
							locctr = c->operateLocctr(locctr, '+', (size - 3));
						else if (ch != 0 && hex == 0) {
							size -= 3;
							int length = 0;
							if (size % 2 == 0)
								length = size / 2;
							else
								length = (size / 2) + 1;
							locctr = c->operateLocctr(locctr, '+', length);
						}
					} else if (operation == "WORD")
						locctr = c->operateLocctr(locctr, '+', 3);
					else if (operation == "RESB") {
						int length = c->convertToInteger(operand);
						locctr = c->operateLocctr(locctr, '+', length);
					} else if (operation == "RESW") {
						int length = c->convertToInteger(operand);
						locctr = c->operateLocctr(locctr, '+', (3 * length));
					} else if (operation == "EQU") {
						string expression = evaluateExpression(operand);
						listingFile[lineNum][1] = expression;
					} else if (operation == "ORG") {
						string expression = evaluateExpression(operand);
						listingFile[lineNum][1] = expression;
						locctr = expression;
					} else if (operation == "END") {
						ends.push_back(locctr);
						isEnded = true;
						lineNum++;
						for (int i = 0; i < 1000 && t->LITTAB[i][0] != "\0";
								i++) {
							if (t->LITTAB[i][3] == ""
									&& !isLiteralFoundListingFile(lineNum,
											t->LITTAB[i][0])) {
								//Setting Location Counter
								t->LITTAB[i][3] = locctr;
								listingFile[lineNum][0] = c->convertToString(
										lineNum);
								listingFile[lineNum][1] = locctr;
								listingFile[lineNum][2] = "*";
								listingFile[lineNum][3] = "-3";
								listingFile[lineNum][4] = t->LITTAB[i][0];
								listingFile[lineNum][7] = c->expandHexa(
										t->LITTAB[i][1], 6);
								lineNum++;
								if (t->LITTAB[i][0] == "=*")
									locctr = c->operateLocctr(locctr, '+', 3);
								else {
									operand = t->LITTAB[i][0].substr(1);
									int size = operand.size();
									int ch = operand.find("C'");
									int hex = operand.find("X'");
									if (ch == 0 && hex != 0) //Size - C'' three bytes :)
										locctr = c->operateLocctr(locctr, '+',
												(size - 3));
									else if (ch != 0 && hex == 0) {
										size -= 3;
										int length = 0;
										if (size % 2 == 0)
											length = size / 2;
										else
											length = (size / 2) + 1;
										locctr = c->operateLocctr(locctr, '+',
												length);
									}
								}
							}
						}
						break;
					} else if (operation == "BASE" || operation == "NOBASE"
							|| operation == "EXTREF") {
						listingFile[lineNum][1] = "";
						if (operation == "EXTREF") {
							istringstream ss(operand);
							vector<string> tokens;
							while (getline(ss, operand, ','))
								tokens.push_back(operand);
							externals.push_back(tokens);
						}

					} else if (operation == "CSECT") {
						listingFile[lineNum][1] = "0000";
						ends.push_back(locctr);
						starts.push_back(label);
						locctr = "0000";
						t->increaseSectionNumber();
					} else if (operation == "EXTDEF") {
						istringstream ss(operand);
						vector<string> tokens;
						while (getline(ss, operand, ','))
							tokens.push_back(operand);
						externalReferences.push_back(tokens);
					} else if (operation == "LTORG") {
						listingFile[lineNum][1] = "";
						lineNum++;
						for (int i = 0; i < 1000 && t->LITTAB[i][0] != "\0";
								i++) {
							if (t->LITTAB[i][3] == ""
									&& !isLiteralFoundListingFile(lineNum,
											t->LITTAB[i][0])) {
								//Setting Location Counter
								t->LITTAB[i][3] = locctr;
								listingFile[lineNum][0] = c->convertToString(
										lineNum);
								listingFile[lineNum][1] = locctr;
								listingFile[lineNum][2] = "*";
								listingFile[lineNum][3] = "-3";
								listingFile[lineNum][4] = t->LITTAB[i][0];
								listingFile[lineNum][7] = c->expandHexa(
										t->LITTAB[i][1], 6);
								lineNum++;
								if (t->LITTAB[i][0] == "=*")
									locctr = c->operateLocctr(locctr, '+', 3);
								else {
									operand = t->LITTAB[i][0].substr(1);
									int size = operand.size();
									int ch = operand.find("C'");
									int hex = operand.find("X'");
									if (ch == 0 && hex != 0) //Size - C'' three bytes :)
										locctr = c->operateLocctr(locctr, '+',
												(size - 3));
									else if (ch != 0 && hex == 0) {
										size -= 3;
										int length = 0;
										if (size % 2 == 0)
											length = size / 2;
										else
											length = (size / 2) + 1;
										locctr = c->operateLocctr(locctr, '+',
												length);
									}
								}
							}
						}
						continue;
					}
				}
			}
			isValid &= (result != 1); //Still true until one validation come invalid
			lineNum++;
		}
	}
	generateIntermediateFile();
	return isValid;
}

bool SICXEPass1::isLiteralFoundListingFile(int lineNum, string literal) {
	for (int i = 0; i < 1000 && listingFile[i][0] != "\0"; i++)
		if (listingFile[i][3] == "-3" && listingFile[i][4] == literal)
			return true;
	return false;
}

bool SICXEPass1::isExternalReference(string lb) {
	for (int i = 0; i < externalReferences.size(); i++)
		for (int j = 0; j < externalReferences.at(i).size(); j++)
			if (externalReferences.at(i).size() > 0
					&& c->eqIgnoreCase(externalReferences.at(i).at(0), lb))
				return true;
	return false;
}

bool SICXEPass1::checkOperandCharacters(string operand) {
	if (operand.size() > 0 && operand.at(0) == '=') {
		if (operand.size()>2 && operand.at(1) >= '0' && operand.at(1) <= '9')
			return true;
		if (operand.size() < 4)
			return false;
		if (operand.at(1) != 'c' && operand.at(1) != 'C' && operand.at(1) != 'X'
				&& operand.at(1) != 'x')
			return false;
		if (operand.at(2) != '\'')
			return false;
		return true;
	}
	for (int i = 0; i < operand.size(); i++) {
		if (!(operand.at(i) >= 'A' && operand.at(i) <= 'Z')
				&& !(operand.at(i) >= 'a' && operand.at(i) <= 'z')
				&& !(operand.at(i) >= '0' && operand.at(i) <= '9')
				&& operand.at(i) != '@' && operand.at(i) != '#'
				&& operand.at(i) != ',' && operand.at(i) != '*'
				&& operand.at(i) != '-' && operand.at(i) != '+'
				&& operand.at(i) != '\'')
			return false;
	}
	return true;
}
int SICXEPass1::verifyLine(string line) //Check start and the End and the operands
		{
	vector<string> fields = split(line);
	string ext = "EXTREF";
	string def = "EXTDEF";
	if (!isComment(line) && ext.compare(c->capitalize(fields[1])) != 0
			&& def.compare(c->capitalize(fields[1])) != 0)
		if (fields[0].size() > 8 || fields[1].size() > 6
				|| fields[2].size() > 18 || fields[3].size() > 31) {
			listingFile[lineNum][0] = c->convertToString(lineNum);
			listingFile[lineNum][1] = locctr;
			listingFile[lineNum][3] = "-1";
			listingFile[lineNum][4] = line;
			lineNum++;
			listingFile[lineNum][0] = c->convertToString(lineNum);
			listingFile[lineNum][1] = "";
			listingFile[lineNum][3] = "-1";
			listingFile[lineNum][4] = "**** Error: Invalid Fields Size";
			return 1;
		}
	if (!isComment(line) && !checkOperandCharacters(fields[2])) {
		listingFile[lineNum][0] = c->convertToString(lineNum);
		listingFile[lineNum][1] = locctr;
		listingFile[lineNum][3] = "-1";
		listingFile[lineNum][4] = line;
		lineNum++;
		listingFile[lineNum][0] = c->convertToString(lineNum);
		listingFile[lineNum][1] = "";
		listingFile[lineNum][3] = "-1";
		listingFile[lineNum][4] = "**** Error: Invalid Operand";
		return 1;
	}
	if (fields[1] != "-2" && !c->eqIgnoreCase(fields[1], "START")
			&& !isStarted) {
		cerr << "Invalid File Format" << endl;
		exit(EXIT_FAILURE);
	}
	if (fields[1] == "-2") //Comment Line
			{
		listingFile[lineNum][0] = c->convertToString(lineNum);
		listingFile[lineNum][1] = "";
		listingFile[lineNum][3] = "-2";
		listingFile[lineNum][4] = line;
		return 0;
	} else if (fields[1] == "-1"
			|| (t->searchOpTab(fields[1]) == "" && !t->isDirective(fields[1])))
			//If splitting returned error or not found in neither of Operation Tables or Directives, then it must be invalid
			{
		listingFile[lineNum][0] = c->convertToString(lineNum);
		listingFile[lineNum][1] = locctr;
		listingFile[lineNum][3] = "-1";
		listingFile[lineNum][4] = line;
		lineNum++;
		listingFile[lineNum][0] = c->convertToString(lineNum);
		listingFile[lineNum][1] = "";
		listingFile[lineNum][3] = "-1";
		listingFile[lineNum][4] = "**** Error: Invalid Operation Code";
		return 1;
	} else if (fields[0] != "" && t->searchSymTab(fields[0]) != ""
			&& t->searchSymTabControlSection(fields[0]) == t->sectionNum) {
		listingFile[lineNum][0] = c->convertToString(lineNum);
		listingFile[lineNum][1] = locctr;
		listingFile[lineNum][3] = "-1";
		listingFile[lineNum][4] = line;
		lineNum++;
		listingFile[lineNum][0] = c->convertToString(lineNum);
		listingFile[lineNum][1] = "";
		listingFile[lineNum][3] = "-1";
		listingFile[lineNum][4] = "**** Error: Duplicate Symbol";
		return 1;
	} else if (!checkOperands(fields[1], fields[2])) {
		listingFile[lineNum][0] = c->convertToString(lineNum);
		listingFile[lineNum][1] = locctr;
		listingFile[lineNum][3] = "-1";
		listingFile[lineNum][4] = line;
		lineNum++;
		listingFile[lineNum][0] = c->convertToString(lineNum);
		listingFile[lineNum][1] = "";
		listingFile[lineNum][3] = "-1";
		listingFile[lineNum][4] = "**** Error: Invalid Operand Field";
		return 1;
	} else {
		listingFile[lineNum][0] = c->convertToString(lineNum);
		listingFile[lineNum][1] = locctr;
		listingFile[lineNum][2] = fields[0];
		listingFile[lineNum][3] = fields[1];
		listingFile[lineNum][5] = fields[2];
		listingFile[lineNum][6] = fields[3];
		return 2;
	}
	return 2;
}

int i;
void SICXEPass1::goThroughDelimeters(string line) {
	while (i < line.size() && (line[i] == ' ' || line[i] == '\t'))
		i++;
}

char SICXEPass1::nextToken(string line) {
	int j = i;
	while (j < line.size() && (line[j] == ' ' || line[j] == '\t'))
		j++;
	if (j < line.size())
		return line[j];
	return '!';
}

bool SICXEPass1::isComment(string line) {

	for (int i = 0; i < line.size(); i++) {
		if (line.at(i) != ' ' && line.at(i) != '\t' && line.at(i) != '.')
			return false;
		if (line.at(i) == '.')
			return true;
	}
	return true;
}

string SICXEPass1::goThroughWords(string line, bool delimeters) {
	string token = "";
	while (i < line.size()
			&& (!delimeters || (line[i] != ' ' && line[i] != '\t')))
		token += line[i++];
	return token;
}

string SICXEPass1::tokenExtraction(string line, bool delimeters, bool plus) {
	if (delimeters)
		goThroughDelimeters(line);
	string token = "";
	token += goThroughWords(line, delimeters);
	char next = nextToken(line);
	while (i < line.size() && token.at(0) == '='
			&& token.at(token.size() - 1) != '\''){
		if(token.size()>1 && token.at(1)>='0' && token.at(1)<='9')
			return token;
		if (token.find('\'') != -1)
			token += tokenExtraction(line, false, plus);
		else
			token += tokenExtraction(line, true, plus);
	}
	next = nextToken(line);

	if (i < line.size() && (next == ',' || line.at(i - 1) == ',')) {
		token += tokenExtraction(line, true, plus);
		next = nextToken(line);
		if (token[token.size() - 1] == ',')
			token += tokenExtraction(line, true, plus);
	}

	if (i < line.size() && (next == '-' || line.at(i - 1) == '-')) {
		token += tokenExtraction(line, true, plus);
		next = nextToken(line);
		if (token[token.size() - 1] == '-')
			token += tokenExtraction(line, true, plus);
	}
	if (plus && i < line.size() && (next == '+' || line.at(i - 1) == '+')) {
		token += tokenExtraction(line, true, plus);
		next = nextToken(line);
		if (token[token.size() - 1] == '+')
			token += tokenExtraction(line, true, plus);
	}
	if ((token[0] == '+' || token[0] == '@' || token[0] == '#')
			&& token.size() == 1) {
		token += tokenExtraction(line, true, plus);
	}
	return token;

}

string SICXEPass1::deleteReturn(string str) {
	for (int i = 0; i < str.length(); ++i) {
		if (str[i] == '\r' || str[i] == '\n')
			str.erase(i--);
	}
	return str;
}
vector<string> SICXEPass1::split(string line) {
	line = deleteReturn(line);
	string tokens[4];
	i = 0;
	tokens[0] = tokenExtraction(line, true, false);
	tokens[1] = tokenExtraction(line, true, true);
	tokens[2] = tokenExtraction(line, true, true);
	tokens[3] = tokenExtraction(line, false, false);

	if (tokens[0][0] == '+' || t->searchOpTab(tokens[0]) != ""
			|| t->isDirective(tokens[0])) {
		i = 0;
		tokens[0] = tokenExtraction(line, true, false);
		tokens[1] = tokenExtraction(line, true, true);
		tokens[2] = tokenExtraction(line, false, true);
		tokens[3] = tokenExtraction(line, false, false);
		tokens[3] = tokens[2];
		tokens[2] = tokens[1];
		tokens[1] = tokens[0];
		tokens[0] = "";
	}
	if (isComment(line)) {
		tokens[0] = line;
		tokens[1] = "-2"; //Return a comment Line
		tokens[2] = "";
		tokens[3] = "";
	} else if (!t->isDirective(tokens[1]) && t->searchOpTab(tokens[1]) == "")
		tokens[1] = "-1";
	vector<string> r;
	r.push_back(tokens[0]);
	r.push_back(tokens[1]);
	r.push_back(tokens[2]);
	r.push_back(tokens[3]);
	return r;
}

vector<string> SICXEPass1::splitTerms(string expression) {
	vector<string> terms;
	string currentTerm = "";
	for (int i = 0; i < expression.length(); i++) {
		if (expression.at(i) == '+' || expression.at(i) == '-') {
			terms.push_back(currentTerm);
			currentTerm = "";
		} else
			currentTerm += expression.at(i);
	}
	terms.push_back(currentTerm);
	for (int i = 0; i < terms.size(); i++) {
		string s = terms.at(i);
		s.erase(s.begin(),
				find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
	}
	return terms;
}

vector<string> SICXEPass1::splitOperators(string expression) {
	vector<string> oper;
	for (int i = 0; i < expression.length(); i++) {
		if (expression.at(i) == '+')
			oper.push_back("+");
		else if (expression.at(i) == '-')
			oper.push_back("-");
	}
	return oper;
}

string SICXEPass1::evaluateExpression(string expression) {
	string stringValue = "";
	vector<string> values;
	vector<string> terms = splitTerms(expression);
	for (int i = 0; i < terms.size(); i++) {
		if (c->convertToInteger(terms.at(i)) == 0) {
			if (terms.at(i) == "*")
				values.push_back(locctr);
			else if (t->searchSymTab(terms.at(i)) != "")
				values.push_back(t->searchSymTab(terms.at(i)));
			else
				return "error";
		} else
			values.push_back(terms.at(i));
	}
	vector<string> oper = splitOperators(expression);
	if ((oper.size() + 1) != values.size())
		return "error";
	int value = c->convertToInteger(values.at(0));
	for (int i = 1; i < values.size(); i++) {
		if (oper.at(i - 1) == "+")
			value += c->convertToInteger(values.at(i));
		else if (oper.at(i - 1) == "-")
			value -= c->convertToInteger(values.at(i));
	}
	if (value < 0)
		return "error";
	stringValue = c->convertToString(value);
	stringValue = c->expandHexa(stringValue, 4);
	return stringValue;
}

bool SICXEPass1::checkOperands(string operations, string operands) {
	/*1)Check the Correct Number of Operands for each operation:
	 RSUB/LTORG/BASE/NOBASE ---> No operands
	 COMPR/RMO/ADDR/SUBR/MULR/DIVR --> 2 Operands
	 else only one Operand
	 2)Check on the Operands values if Registers or expressions
	 3)Check if Literal to be correcly evaluated and inserted at LITTAB
	 */
	if (operands.size() > 0
			&& (c->eqIgnoreCase(operations, "BYTE") || operands.at(0) == '=')) {
		string tr = operands.substr(1);
		tr = c->capitalize(tr);
		int size = tr.size();
		int ch = tr.find("C'");
		int hex = tr.find("X'");
		if (ch != 0 && hex == 0) {
			size -= 3;
			if (size % 2 == 1)
				return false;
		}
	}
//First Check
	int commaFlag = operands.find(",");
	if ((c->eqIgnoreCase(operations, "RSUB")
			|| c->eqIgnoreCase(operations, "NOBASE")
			|| c->eqIgnoreCase(operations, "LTORG")) && operands != "")
		return false;
	else if ((c->eqIgnoreCase(operations, "RMO")
			|| c->eqIgnoreCase(operations, "COMPR")
			|| c->eqIgnoreCase(operations, "MULR")
			|| c->eqIgnoreCase(operations, "ADDR")
			|| c->eqIgnoreCase(operations, "SUBR")
			|| c->eqIgnoreCase(operations, "DIVR")) && commaFlag == -1)
		return false;

//Second Check
	else if (commaFlag >= 0) {
		istringstream ss(operands);
		string found;
		vector<string> tokens;
		while (getline(ss, found, ','))
			tokens.push_back(found);
		if (c->eqIgnoreCase(operations, "EXTREF")
				|| c->eqIgnoreCase(operations, "EXTDEF")) {
		} else if (tokens.size() != 2)
			return false;
		else if (c->eqIgnoreCase(tokens.at(1), "X"))
			return true;
		else {
			if (((t->searchRegTable(tokens.at(0)) != -1
					|| evaluateExpression(tokens.at(0)) == "error")
					&& ((t->searchRegTable(tokens.at(1)) != -1
							|| evaluateExpression(tokens.at(1)) == "error")))) {
			} else
				return false;
		}
	} else if (operands == "=*") {
		t->insertLITTAB(operands, locctr);
	} else if (operands.size() > 0 && operands.at(0) == '=') {
		if (t->isFoundLITTAB(operands)) {
		}
		if (!t->insertLITTAB(operands)) //Handle The Literals
				{
			string result = evaluateExpression(operands.substr(1));
			if (result == "error")
				return false;
			else
				t->insertLITTAB(operands, result);
		} else if (c->isLiteral(operands)) {
		} else {
			cout << "Here_2" << endl;
			return false;
		}

	}
	return true;
}

void SICXEPass1::generateIntermediateFile() {
	ofstream output;
	output.open("Intermediate_File.txt");
	if (output.is_open()) {
		for (int i = 0; i < lineNum; i++) {
			if (listingFile[i][3] == "-1" || listingFile[i][3] == "-2")
				output << c->expandString(listingFile[i][0], 1) << "\t"
						<< c->expandString(listingFile[i][1], 4) << "\t"
						<< listingFile[i][4] << endl;
			else if (listingFile[i][3] == "-3")
				output << c->expandString(listingFile[i][0], 1) << "\t"
						<< c->expandString(listingFile[i][1], 4) << "\t"
						<< c->expandString(listingFile[i][2], 4) << "\t\t"
						<< listingFile[i][4] << endl;
			else {
				output << c->expandString(listingFile[i][0], 1) << "\t"
						<< c->expandString(listingFile[i][1], 4) << "\t"
						<< c->expandString(listingFile[i][2], 8) << "\t"
						<< c->expandString(listingFile[i][3], 5) << "\t"
						<< c->expandString(listingFile[i][5], 17) << "\t"
						<< c->expandString(listingFile[i][6], 4) << "\n";
			}
		}
		output << "\n\n\t\tLiterals TABLE\n";
		output << "*****************************************\n";
		output << "*" << "\t" << "Name" << "\t" << "Value" << "\t" << "Length"
				<< "\t" << "Address*\n";
		for (int i = 0; i < 1000 && t->LITTAB[i][0] != "\0"; i++)
			if (t->LITTAB[i][3] != "")
				output << "\t" << t->LITTAB[i][0] << "\t" << t->LITTAB[i][1]
						<< "\t" << t->LITTAB[i][2] << "\t" << t->LITTAB[i][3]
						<< "\n";
		output << "*****************************************\n";
		output << "*****************************************\n";
		output << "\n\n\tSYMBOL TABLE\n";
		output << "**********************************************\n";
		output << "*   SYMBOL      *    ADDRESS    *Section Numer\n";
		output << "**********************************************\n";

		map<string, vector<string> > c1;
		map<string, vector<string> >::iterator it;
		for (it = t->symTab.begin(); it != t->symTab.end(); ++it) {
			string st1 = it->first;
			int comFlag = st1.find(",");
			if (st1.size() > 0 && st1 != "*" && st1.at(0) != '='
					&& t->searchRegTable(st1) == -1 && comFlag == -1) {
				vector<string> st2 = it->second;
				if (st1.size() < 15) {
					output << "*" << st1;
					for (int i = st1.size(); i < 14; ++i) {
						output << " ";
					}
					output << " |\t";
				}

				if (st2.size() < 15 && st2.size() >= 2) {
					output << st2.at(0) << " |\t" << st2.at(1);
					for (int i = st2.size(); i < 15; ++i) {
						output << " ";
					}
					output << "*\n";
				} else {
					for (int i = st2.size(); i < 15; ++i) {
						output << " ";
					}
					output << "*\n";

				}
			}
		}
		output << "**********************************************\n";
		output << "#End of pass 1 for SIC/XE Assembler#\n";

		output << c->expandString("Line", 1) << "\t"
				<< c->expandString("LocCtr", 2) << "\t"
				<< c->expandString("Labels", 8) << "\t"
				<< c->expandString("OpCode", 5) << "\t"
				<< c->expandString("Flags", 8) << "\t"
				<< c->expandString("Operands", 10) << "\t"
				<< c->expandString("Comments", 12) << "\t"
				<< c->expandString("ObjectCode", 12) << "\t"
				<< c->expandString("ModBoolean", 10) << "\n";

		for (int i = 0; i < lineNum; i++) {
			if (listingFile[i][3] == "-1" || listingFile[i][3] == "-2")
				output << c->expandString(listingFile[i][0], 1) << "\t"
						<< c->expandString(listingFile[i][1], 4) << "\t"
						<< listingFile[i][4] << endl;
			else if (listingFile[i][3] == "-3") {
				output << c->expandString(listingFile[i][0], 1) << "\t"
						<< c->expandString(listingFile[i][1], 9) << "\t"
						<< c->expandString(listingFile[i][2], 4) << "\t\t"
						<< listingFile[i][4] << "\t\t"
						<< c->expandString(listingFile[i][7], 52) << endl;
			} else {
				output << c->expandString(listingFile[i][0], 1) << "\t"
						<< c->expandString(listingFile[i][1], 9) << "\t"
						<< c->expandString(listingFile[i][2], 8) << "\t"
						<< c->expandString(listingFile[i][3], 5) << "\t"
						<< c->expandString(listingFile[i][4], 9) << "\t"
						<< c->expandString(listingFile[i][5], 9) << "\t"
						<< c->expandString(listingFile[i][6], 8) << "\t"
						<< c->expandString(listingFile[i][7], 12) << "\t"
						<< c->expandString(listingFile[i][8], 10) << "\n";
			}
		}

	} else {
		cerr << "Can not Write to Intermediate File" << endl;
		exit(EXIT_FAILURE);
	}
}

string SICXEPass1::getListingFile(int line, int column) {
	return listingFile[line][column];

}

void SICXEPass1::setListingFile(int line, int column, string objectCode) {
	listingFile[line][column] = objectCode;
}

Tables * SICXEPass1::getTable() {
	return t;
}
