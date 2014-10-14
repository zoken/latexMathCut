#include "result.h"

struct Content{
    char* content ;
    int op ;
    struct Content *next ;
};

class MathWordTool{
    private:
        
    public:
        void cut(const char* sentence,list<char*>* result);
};
