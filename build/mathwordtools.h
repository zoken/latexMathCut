#include "stdlib.h"
#include <list>
#include <vector>
#include "chineseCut.h"
#include "eqs.h"
#include "wordpos.h"
#define CHINESE_USE 1
#define JIEBA_CUT 1
#define TC_CUT 2

class MathWordTool{
    private:
        ChineseCut *_segment ;
        void handleContent(char*,EQS**,std::list<char*>*);
        void findWordPoses(char* sentence ,std::list<char*>& words ,std::vector<WordPos*>& result);
    public:
        void cut(const char* sentence,std::list<char*>* result);
        void cut(const char* sentence, std::vector<WordPos*>& result);
        void regChineseCutTool(ChineseCut* chinesecut){
            _segment = chinesecut;
        }
        void chineseCut(const char* word,std::list<char*>* resultlist);
};
