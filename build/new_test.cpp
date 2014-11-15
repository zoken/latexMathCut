#include "mathinterface.h"
#include <iostream>
using namespace std;
int main(){
    const char* sentence = "1+1数学";
     Jieba_Cut jiebacut("../dict/extra_dict/jieba.dict.small.utf8");
     MathSegmentTool mwt ;
     mwt.regChineseCutTool(&jiebacut);
     std::vector<WordPos*> results;
     mwt.fullCut(sentence, 0,strlen(sentence), results);
     std:vector<WordPos*>::iterator _iter;
     for(_iter = results.begin() ; _iter!=results.end();_iter++){
        cout << (*_iter)->word << endl;
     }
}
