#include <iostream>
#include "SICXEPass2.h"

using namespace std;

int main(int argc,char*argv[])
{
    if(argc>1)
    {
        SICXEPass2 * pass_2 = new SICXEPass2(argv[1]);
        pass_2->pass2();
        cout<<"Input File is = "<<argv[1]<<endl;
    }
    else{
        SICXEPass2 * pass_2 = new SICXEPass2("input.txt");
        pass_2->pass2();
    }

    return 0;
}
