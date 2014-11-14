extern "C"{
#include "tools.h"
}
#include <iostream>
using namespace std;
int main(int argc, char*  args[]){
    cout << generateDivAnswers(args[1]) << endl;
    return 0 ;
}
