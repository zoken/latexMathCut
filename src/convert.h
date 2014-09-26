#define MODE_VERTICAL              0
#define MODE_INTERNAL_VERTICAL     1
#define MODE_HORIZONTAL            2
#define MODE_RESTRICTED_HORIZONTAL 3
#define MODE_MATH                  4
#define MODE_DISPLAYMATH           5

#include"stack.h"

class Converter{
    public:
        void SetTexMode(int mode);
        int  GetTexMode(void);
        void ConvertString(char *string,MathStack* mstack);
        void ConvertAllttString(char *s,MathStack* mstack);
        void Convert(MathStack* mstack);
};
