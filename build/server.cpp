#include <unistd.h>
#include <algorithm>
#include <string>
#include <ctype.h>
#include <string.h>
#include "Limonp/Config.hpp"
#include "MPSegment.hpp"
#include "HMMSegment.hpp"
#include "MixSegment.hpp"

using namespace CppJieba;

int main(){
    cout << "TEST CUT WORDS" << endl ;
    string sentence = "i am zhongliang";
    vector<string> words ;
    MixSegment _segment("../dict/jieba.dict.utf8","../dict/hmm_model.utf8","./");
    cout << "" << endl;
    _segment.cut(sentence,words);
    vector<string>::iterator it;
    for(it=words.begin();it!=words.end();it++)
        cout<<*it<< endl;
    return 0;
}


