#include "stdio.h"
#include <list>

using namespace std;

typedef struct reseult_stack{
    char* _word_content;
    struct result_stack *next;
}ResStack;

class Result{
    private:
        list<ResStack> stacks ;
    public:
        Result();
        void PushWord(char* _word);
        char* PopWord();
        void WriteEquation(char* _equation);

};
