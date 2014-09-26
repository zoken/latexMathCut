#define STACKSIZE 500

class MathStack{
    private:
        
        int   stack[STACKSIZE];
        int   top ;
        int   BraceLevel;
        int   BasicPush(int lev, int brack);
        int   BasicPop(int *lev, int *brack);
        int   getStackRecursionLevel(void);
    public:
        void  InitializeStack(void);
        void  PushLevels(void);
        int   PopLevels(void);
        void  CleanStack(void);
        void  PushBrace(void);
        int   PopBrace(void);
};
