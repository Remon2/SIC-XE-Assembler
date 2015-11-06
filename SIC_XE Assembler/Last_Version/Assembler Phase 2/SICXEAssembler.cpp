#include <iostream>
#include "SICXEPass2.h"

using namespace std;

int main() {
	SICXEPass2 * pass_2 = new SICXEPass2(); //Input file Name, boolean isFixedFormat
	pass_2->pass2();
	return 0;
}
