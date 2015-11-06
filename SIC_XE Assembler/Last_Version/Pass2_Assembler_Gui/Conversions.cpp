#include "Conversions.h"
#include "SICXEPass1.h"

using namespace std;

Conversions::Conversions(){}

Conversions::~Conversions(){}


int Conversions::hexToDec(string hexa)
{
   return binToDec(hexToBin(hexa));

}

int Conversions::binToDec(string rvrs)
{
    string binary;
    for(int i=0;i<rvrs.length();i++)
            binary+=rvrs.at(rvrs.length()-i-1);
    int decimal=0;
    for(int i=0;i<binary.length();i++)
    {
        int number = (int)binary.at(i)-48;
        if(number==0||number==1)
            decimal+=number*pow(2, i);
        else
            return -1;
    }
    return decimal;
}

string Conversions::hexToBin(string hexa)
{
    stringstream ss;
    ss << hex << hexa;
    unsigned n;
    ss >> n;
    bitset<50> b(n);
    return b.to_string<char,std::string::traits_type,std::string::allocator_type>();

}

string Conversions::binToHex(string binary)
{
    long int longint=0;
    string buf = binary, output="";
    stringstream ss;
    int len=buf.size();
    for(int i=0;i<len;i++)
        longint+=( buf[len-i-1]-48) * pow(2,i);
    ss<<setbase(16);
    output = ss.str();
    ss<<longint;
    output += ss.str();
    return output;
}


string Conversions::decToHexa(int num)
{
    long int rem[50],i=0,length=0;
    string r="";
    if(num==0)
    	return "0";
    while(num>0)
    {
        rem[i]=num%16;
        num=num/16;
        i++;
        length++;
    }

    for(i=length-1;i>=0;i--)
  {
    switch(rem[i])
    {
      case 10:
          r+="A";break;
      case 11:
          r+="B";break;
      case 12:
          r+="C";break;
      case 13:
          r+="D";break;
      case 14:
          r+="E";break;
      case 15:
          r+="F";break;
      default :
         r+=convertToString(rem[i]);break;
    }
  }
  return r;
}

string Conversions::capitalize(string small)
{

    locale settings;
    string converted;
    for(int i = 0; i < small.size(); ++i)
        converted += (toupper(small[i], settings));
       // cout<<"Converted "<<converted<<endl;
    return converted;
}

bool Conversions::eqIgnoreCase(string str1, string str2)
{
    string str1C= capitalize(str1);
    string str2C= capitalize(str2);
    return (str1C==str2C);
}

string Conversions::expandHexa(string hexa, int length)
{
    if(hexa=="")
	{
		hexa="0000";
		return hexa;
	}
	else
		if(hexa.length()<length)
		{//expantion
		    int nx=hexa.length();
		    for (int l=0;l<(length-nx);l++)
				hexa+="0";
		    string aa =hexa.substr(nx);
			hexa=aa+hexa.substr(0,nx);
		}

	return hexa;
}

string Conversions::expandString(string line, int length)
{
    if(line.length()<length)
    {
        int x = line.length();
        for(int i=0;i<(length-x+1);i++)
            line+=" ";
        string a = line.substr(x);
        line = a+ line.substr(0, x);
    }
    return line;
}

string Conversions::convertToString(int number)
{
    string result;
    ostringstream convert;
    convert << number;
    result = convert.str();
    return result;
}

int Conversions::convertToInteger(string num)
{
    try{
    istringstream buffer(num);
    int value;
    buffer >> value;
    return value;
    }catch(int e){return -1;}
}

int Conversions::hexaToDec(const char* hexStr )
{
    int hexInt;
    sscanf( hexStr, "%x", &hexInt );
    return hexInt;
}


string Conversions::operateLocctr(string locctr, char oper, int operand)
{
    int l = hexaToDec(locctr.c_str());
    string result="";
    if(oper=='+')
        result = decToHexa(l+operand);
    else if(oper=='-')
        result = decToHexa(l-operand);
    result = expandHexa(result, 4);
    return result;
}

string Conversions::asciiToHex(string c)
{
    string b="";
    for(int i=0;i<c.size();i++)
    {
        switch(c.at(i))
        {
			case'A':
				b+= "41";break;
			case'B':
				b+= "42";break;
			case'C':
				b+= "43";break;
			case'D':
				b+= "44";break;
			case'E':
				b+= "45";break;
			case'F':
				b+= "46";break;
			case'G':
				b+= "47";break;
			case'H':
			    b+= "48";break;
			case'I':
				b+= "49";break;
			case'J':
				b+= "4A";break;
			case'K':
				b+= "4B";break;
			case'L':
			    b+= "4C";break;
			case'M':
                b+= "4D";break;
			case'N':
                b+= "4E";break;
			case'O':
                b+= "4F";break;
			case'P':
				b+= "50";break;
            case'Q':
			    b+= "51";break;
			case'R':
                b+= "52";break;
			case'S':
                b+= "53";break;
			case'T':
                b+= "54";break;
			case'U':
                b+= "55";break;
            case'V':
			    b+= "56";break;
			case'W':
                b+= "57";break;
			case'X':
                b+= "58";break;
			case'Y':
                b+= "59";break;
			case'Z':
				b+= "5A";break;

            case'0':
			    b+= "30";break;
			case'1':
                b+= "31";break;
			case'2':
                b+= "32";break;
			case'3':
                b+= "33";break;
			case'4':
				b+= "34";break;
            case'5':
                b+= "35";break;
			case'6':
                b+= "36";break;
			case'7':
				b+= "37";break;
            case'8':
                b+= "38";break;
			case'9':
                b+= "39";break;
			case' ':
				b+= "00";break;

            case'a':
				b+= "61";break;
			case'b':
				b+= "62";break;
			case'c':
				b+= "63";break;
			case'd':
				b+= "64";break;
			case'e':
				b+= "65";break;
			case'f':
				b+= "66";break;
			case'g':
				b+= "67";break;
			case'h':
			    b+= "68";break;
			case'i':
				b+= "69";break;
			case'j':
				b+= "6A";break;
			case'k':
				b+= "6B";break;
			case'l':
			    b+= "6C";break;
			case'm':
                b+= "6D";break;
			case'n':
                b+= "6E";break;
			case'o':
                b+= "6F";break;
			case'p':
				b+= "70";break;
            case'q':
			    b+= "71";break;
			case'r':
                b+= "72";break;
			case's':
                b+= "73";break;
			case't':
                b+= "74";break;
			case'u':
				b+= "75";break;
            case'v':
			    b+= "76";break;
			case'w':
                b+= "77";break;
			case'x':
                b+= "78";break;
			case'y':
                b+= "79";break;
			case'z':
                b+= "7A";break;
        }
    }
	return b;
}


bool Conversions::isLiteral(string name)
{
    if(name.size()>4&&name.at(0)=='='&&(name.at(2)=='\'')&&(name.at(name.size()-1)=='\'')
       &&(name.at(1)=='c'||name.at(1)=='C'||name.at(1)=='x'||name.at(1)=='X'))
        return true;
    return false;
}

