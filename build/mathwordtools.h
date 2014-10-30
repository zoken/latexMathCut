#include "stdlib.h"
#include <list>
#define CHINESE_USE 1
#define JIEBA_CUT 1
#define TC_CUT 2

class MathWordTool{
    private:
    public:
        void cut(const char* sentence,std::list<char*>* result);
        void init();
        void setChineseCutMode(int Mode);

};
