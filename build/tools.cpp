#include <sys/time.h>
#include <time.h>

extern "C"
{
#include "tools.h"
}
void diagnostics(int level, char *format,...){
    ;
}
double getTimeMillis(){
    struct timeval val ;
    gettimeofday(&val,NULL);
    double ret = ((double)val.tv_sec)*1000+((double)val.tv_usec)/1000;
    return ret ;
}
