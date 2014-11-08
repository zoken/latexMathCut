#include "chineseCut.h"
using namespace std;
void Jieba_Cut::cut(const char* word,vector<string>& words){
    if(_fullsegment!=NULL){
        _fullsegment->cut(word,words);
    }else{
        _segment->cut(word,words);
    }
}
