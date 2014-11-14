#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
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
/************************************
从sentence中获取flag标签对应的内容
*************************************/
char* getNextJSONString(char* sentence , const char* start_flag ,const char* end_flag){
    if(sentence == NULL||start_flag == NULL|| end_flag == NULL )
        return NULL ;
    char* str_start_pos = strstr(sentence , start_flag);
    if(str_start_pos == NULL)
        return NULL;
    char* str_end_pos = strstr(str_start_pos , end_flag);
    if(str_end_pos == NULL)
        return NULL ;
    int len = str_end_pos - str_start_pos - strlen(start_flag);
    str_start_pos = str_start_pos + strlen(start_flag);
    char* ret = (char*)malloc(sizeof(char)*(len+3));
    ret[0] = '\"';
    memcpy(ret+1, str_start_pos, len);
    ret[len+1]='\"';
    ret[len+2] = '\0';
    str_start_pos = NULL ;
    str_end_pos = NULL ;
    return ret ;
}
char* generateDivAnswers(char* answer){
    const char* o_start = "<o>"; 
    const char* o_end = "</o>";
    const char* c_start = "<c>";
    const char* c_end = "</c>";
    const char* str_start = "<string>";
    const char* str_end = "</string>";
    
    //define result_buf
    char* result_buf = (char*)malloc(sizeof(char)*(strlen(answer) + 1024)); //content+label.
    result_buf[0] = '\0';
    
    //handle c_content
    char* c_content = getNextJSONString(answer, c_start, c_end);
    char* final_answer = NULL ;
    if (c_content != NULL) {
        final_answer = getNextJSONString(c_content, str_start, str_end);
    //if answer exists . then add answer flag to final_answer.
        if (final_answer != NULL) {
            const char* answer_label_start = "\"Answers:\":[";
            const char* answer_label_end = "]";
            strcat(result_buf, answer_label_start);
            strcat(result_buf, final_answer);
            strcat(result_buf, answer_label_end);
            free(final_answer);
        }
        free(c_content);
    }
    
    //handle o_content.
    char* o_content = getNextJSONString(answer, o_start, o_end);
    if (o_content != NULL) {
        char* o_str_start = strstr(o_content, str_start);
        char* tmp_option ;
        strcat(result_buf, "\"Options\":["); //add option start.
        bool start = true ;
    
        //get options from o_content.
        while (o_str_start != NULL ) {
            tmp_option = getNextJSONString(o_str_start, str_start, str_end);
            if (tmp_option != NULL) {
                if(start == true){
                    start = false ;
                } else {
                    strcat(result_buf, ",");
                }
                strcat(result_buf, tmp_option);
                free(tmp_option);
            }
            o_str_start += strlen(str_start);
            o_str_start = strstr(o_str_start, str_start);
        }
        strcat(result_buf, "]");
        free(o_content);
    }
    
    //constuct result_str;
    char* result_str = (char*)malloc(sizeof(char)*(strlen(result_buf)+1));
    strcpy(result_str, result_buf);
    free(result_buf);
    result_buf = NULL ;
    return result_str ;
}
