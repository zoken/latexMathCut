#include "chineseCut.h"
using namespace std;
void Jieba_Cut::cut(const char* word,vector<string>& words){
    _segment->cut(word,words);
}
