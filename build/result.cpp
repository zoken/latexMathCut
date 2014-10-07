#include "result.h"
#include <string.h>
#include <stdlib.h>
using namespace std;

void Result::PushWord(char* _word)
/***
    purpose: add word into stack.
***/
{
    ResStack rs;
    rs._word_content = _word ;
    stacks.push_back(rs);

}
char* Result::PopWord()
/****
    purpose: finish a word;
****/
{
    if(stacks.empty())
        return NULL;
    else{
        ResStack rs = stacks.back();
        stacks.pop_back();
        return rs._word_content;
    }
}
void Result::WriteEquation(char* equation)
/*****
    purpose: fill word;
*****/
{
    if(stacks.empty())
        return ;
    list<ResStack>::iterator iter ;
    for(iter = stacks.begin();iter!=stacks.end();iter++){
        iter->_word_content = strcat((*iter)._word_content,equation);
    }
}
