#include <iostream>
#include "latexparser.h"
#include "equation.h"

int main(int argc,char** argv){
	Parser p ; 
	p.setSentence("$1+1$");
	Equation q ;
	q.regParser(&p);
    char c=  p.getTexChar();
    std::cout << c << std::endl ;
	q.CmdEquation(EQN_DOLLAR|ON);
    return 0;
}
