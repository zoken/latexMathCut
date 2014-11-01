#include "mathwordtools.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "latexparser.h"
#include "equation.h"
#include "Limonp/Config.hpp"
#include "MPSegment.hpp"
#include "HMMSegment.hpp"
#include "MixSegment.hpp"

#define BIGOP 2
#define MINOP 2
#define EQUALOP 2
#define BIGEQUALOP 2
#define MINEQUALOP 2
#define ADDOP 3
#define PLUSOP 3
#define MULOP 4
#define DIVOP 4
#define FLATOP 5

using namespace std;
/*****************************
    method:handleContent.本部分用于处理数学公式不可分的部分。提取头尾中可能成为数学单词的部分，加入表达式处理的结构
        中间部分使用中文分词chineseCut
    word:需要处理的单词
    current:表达式处理的结构
    resultlist:最终结果
    内存增量：头、尾、中间部分（调用中文分词的话，则释放）
****************************/
void MathWordTool::handleContent(char* word, EQS** current,list<char*>* resultlist){
    if(word == NULL)
        return ;
    else if(strlen(word)<=0)
        return ;
    int left = 0 ; 
    int right = strlen(word);
    int i = 0 ; 
    //提取word头部的数学单词
    for(i = 0 ; i < right ; i++){
        if((word[i]>='a'&&word[i]<='z')||(word[i]>='A'&&word[i]<='Z')
            ||(word[i]>='0'&&word[i]<='9')){
            ;
        }else{
            break ;
        }
    }
    //提取word尾部的数学单词
    left = i ;
    for(i = right-1 ; i >= left ; i--){
        if((word[i]>='a'&&word[i]<='z')||(word[i]>='A'&&word[i]<='Z')
            ||(word[i]>='0'&&word[i]<='9')){
            ;
        }else
            break;
    }
    char* tmp ; 
    if(i < left){
        //i == 0; // left == right
        EQS::AddContent(word,current);
        resultlist->push_back(word);
    }else{
        tmp = (char*)malloc(sizeof(char)*(left+1));
        memcpy(tmp,word,left);
        tmp[left] = '\0';
        EQS::AddContent(tmp,current);
        if(left > 0 )
            resultlist->push_back(tmp);
        tmp = (char*)malloc(sizeof(char)*(i+1-left+1));
        memcpy(tmp,word+left,i-left+1);
        tmp[i-left+1] = '\0';
        this->chineseCut(tmp,resultlist);
        free(tmp);
        tmp = NULL ;
        tmp = (char*)malloc(sizeof(char)*(right-i));
        strcpy(tmp,word+i+1);
        EQS::AddContent(tmp,current);
        if(i < (right-1))
            resultlist->push_back(tmp);
    }
}
bool isString(const char* sentence){
    int len,i;
    len = strlen(sentence);
    if(len <=0)
        return false ;
    for(i = 0 ; i < len ; i++){
        if((sentence[i]>='a'&&sentence[i]<='z')||(sentence[i]>='A'&&sentence[i]<='Z')
            ||(sentence[i]>='0'&&sentence[i]<='9'))
            continue ;
        else
            return false ;
    }
    return true ;
}
/******************************************************
*   中文分词函数。通过制定CHINESE_USE
*   在编译阶段指定分词的工具。初始化操作在外部执行
******************************************************/
void MathWordTool::chineseCut(const char* content, list<char*>* resultlist){
    if(_segment == NULL)
        return ;

    vector<string> words ;
    vector<string>::iterator it;
    char* word ;
    _segment->cut(content,words);
    for(it=words.begin();it!=words.end();it++){
        word = (char*)malloc(sizeof(char)*(strlen((*it).c_str())+1));
        strcpy(word,((*it).c_str()));
        resultlist->push_back(word);
    }
    words.clear();
}
/****************************************************************
*** 分词主函数。本函数执行包括两个功能：                     ***
*** 1. 区分公式和中文，并调用不同模块                        ***
*** 2. 对数学公式执行词法分析                                ***
***************************************************************/
void MathWordTool::cut(const char* sentence, list<char*>* resultlist){
    if(sentence == NULL)
        return ;
    else if(strlen(sentence)<=0)
        return ;
    else if(isString(sentence))
        return ;
	Parser p ;
	p.setSentence(sentence);
	Equation q ;
	q.regParser(&p);
    char cThis = '\n',cEnd = '\n', cNext = '\n', cNext_Next = '\0';
    int buflastindex = 0, bufcurrentindex = 0;
    int op = 0 ; //must init. else op!=0 will execute with error message.
    char *word , *tmp , *cmd ;
    EQS *head,*current; 
//    CppJieba::MixSegment _segment("../dict/jieba.dict.utf8","../dict/hmm_model.utf8","./");
    head = new EQS();
    current = head ;
    list<char*> currentResult;
    while((cThis = p.getTexChar())!='\0'){
        bufcurrentindex = p.getIndex();
        switch(cThis){
            case '$':
                word = NULL ;
                //if we have content which has not been handled at the head of '$' sentence ;
                if(bufcurrentindex > (buflastindex+1)){
                    bufcurrentindex = p.getIndex();
                    word = p.getChars(buflastindex,bufcurrentindex-1);
                    handleContent(word,&current,resultlist);
                }
                buflastindex = bufcurrentindex-1;//index buflastindex to the begin of '$' sentence
                cNext = p.getTexChar();
                if (cNext == '$'){
                    word = q.CmdEquation(EQN_DOLLAR_DOLLAR | ON);
                }else {
                    p.ungetTexChar();
                    word = q.CmdEquation(EQN_DOLLAR | ON);
                }
                //get the end index of '$' sentence ;
                bufcurrentindex = p.getIndex();
                EQS::AddContent(p.getChars(buflastindex,bufcurrentindex),&current); //reg '$' sentence to current level's content ; to be used as operate content.
                buflastindex = bufcurrentindex;// index begin index to next start.
                cut(word,resultlist);// cut the '$' sentence into small pieces.
                break;
            case '\\':
                // handle the content before cmd.
                word = NULL ;
                if(bufcurrentindex > buflastindex+1){
                    bufcurrentindex = p.getIndex();
                    word = p.getChars(buflastindex,bufcurrentindex-1);
                    handleContent(word,&current,resultlist);
                }
                //move index to '\' as the head of cmd sentence.
                buflastindex = bufcurrentindex -1; 
                cNext = p.getTexChar();
                if(cNext == '\\'){//only is a gap.
                    buflastindex = p.getIndex();
                    break ;
                }else{
                    p.ungetTexChar();
                }
                //get Current Cmd .
                cmd = p.getCmd();
                char* param ;
                if(strcmp(cmd,"frac")==0||strcmp(cmd,"dfrac")==0||strcmp(cmd,"Frac")==0){
                    param = q.CmdFraction(0);
                }else if(strcmp(cmd,"sqrt")==0||strcmp(cmd,"root")==0){
                    param = q.CmdRoot(0);
                }else if(strcmp(cmd,"lim")==0||strcmp(cmd,"limsup")==0||strcmp(cmd,"liminf")==0){
                    param = q.CmdLim(0);
                }else if(strcmp(cmd,"sum")==0||strcmp(cmd,"prod")==0||strcmp(cmd,"int")==0||strcmp(cmd,"iint")==0){
                    param = q.CmdIntegral(0);
                    if (param == NULL)
                        param = p.getChars(0,0);
                }else if(strcmp(cmd,"left")==0){
                    cNext = p.getTexChar();
                    if(cNext!='.')
                        p.ungetTexChar();
                    param = q.CmdLeftRight(0);
                }else if(strcmp(cmd,"stackrel")==0){
                    param = q.CmdStackrel(0);
                }else if(strcmp(cmd,"textsuperscript")==0){
                    param = q.CmdSuperscript(0);
                }else if(strcmp(cmd,"begin")==0){
                    param = q.CmdBeginEnd(0);
                }else if(strlen(cmd)>0){
                    //cmd has not been defined.
                    param = NULL ;
                }else{
                    break ;
                }
                /** cancel th eoperate before the main .
                if(word!=NULL&&strlen(word)>0){//dian cheng
                    EQS::AddContent(word,&current);
                    current->op = MULOP;
                    current->opname = ' ';
                    current->nexteq = new EQS();
                    current = current->nexteq;
                }
                */
                bufcurrentindex = p.getIndex();
                currentResult.push_back(p.getChars(buflastindex,bufcurrentindex));
                buflastindex = bufcurrentindex ;
                EQS::AddContent(currentResult.back(),&current);
                if(param != NULL)
                    cut(param,resultlist);
                break ;
            case '{':
                cEnd = '}';
                break;
            case '[':
                cEnd = ']';
                break;
            case '(':
                cEnd = ')';
                break;
            case '|':
                cEnd = '|';
                break;
            case '\357':
                cNext = p.getTexChar();
                if(cNext == '\274'){
                    cNext = p.getTexChar();
                    if(cNext == '\210'){//中文括号
                        cThis = '(';
                        cEnd = '?';
                    }else if(cNext == '\214'){//中文逗号
                        if(bufcurrentindex > (buflastindex+1)){
                            word = p.getChars(buflastindex, bufcurrentindex-1);
                            handleContent(word,&current,resultlist);
                            buflastindex = p.getIndex();
                        }
                    }else{
                        p.ungetTexChar();
                        p.ungetTexChar();
                    }
                }else{
                    p.ungetTexChar();
                }
                break;
            case '^':
                op = FLATOP ;
                break ;
            case '_':
                op = FLATOP;
                break;
            case '+':
                op = ADDOP;
                break; 
            case '-':
                op = PLUSOP;
                break;
            case '*':
                op = MULOP;
                break;
            case '/':
                op = DIVOP;
                break;
            case '\303':
                cNext = p.getTexChar();
                if(cNext=='\227'){//中文乘号
                    cThis = '*';
                    op = MULOP ;
                }else if(cNext=='\267'){ //中文除号
                    cThis = '/';
                    op = DIVOP ;
                }else{
                    p.ungetTexChar();
                }
                break;
            case '\342': 
                cNext = p.getTexChar();
                cNext_Next = p.getTexChar();
                //角度和三角形
                if((cNext == '\226'&&cNext_Next == '\263')||(cNext == '\210'&&cNext_Next=='\240')){
                    bufcurrentindex = p.getIndex();
                    word = p.getChars(buflastindex,bufcurrentindex-3);
                    handleContent(word,&current,resultlist);
                    buflastindex = bufcurrentindex -3 ;
                    p.getMathWord();
                    bufcurrentindex = p.getIndex();
                    word = p.getChars(buflastindex,bufcurrentindex);
                    EQS::AddContent(word,&current);
                    currentResult.push_back(word);
                    buflastindex= bufcurrentindex;
                }else if(cNext == '\200'&&cNext_Next =='\242'){
                    //点乘
                    cThis = '*';
                    op = MULOP;
                 }else{
                    p.ungetTexChar();
                    p.ungetTexChar();
                 }
                 break;
            case '>':
                op = BIGOP ;
                break;
            case '<':
                op = MINOP ;
                break;
            case '=':
                op = EQUALOP;
                break;
            case ',':
            case '.':
                if(bufcurrentindex > (buflastindex+1)){
                    word = p.getChars(buflastindex, bufcurrentindex-1);
                    handleContent(word,&current,resultlist);
                    buflastindex = p.getIndex();
                }
                break;                
            default:
                break ;
        }
        if(cEnd!='\n'){
            if(bufcurrentindex > (buflastindex+1)){
                word = p.getChars(buflastindex, bufcurrentindex-1);
                handleContent(word,&current,resultlist);
            }
            if(cThis == '(' && cEnd == '?' )
                buflastindex = p.getIndex()-3;
            else
                buflastindex = p.getIndex()-1;
            word = q.CmdKuoHao(cThis,cEnd);
            bufcurrentindex = p.getIndex();
            currentResult.push_back(p.getChars(buflastindex,bufcurrentindex));
            buflastindex = p.getIndex() ;
            EQS::AddContent(currentResult.back(),&current);
            cut(word,resultlist);
            cEnd = '\n';//handle '{}' and so on.
            buflastindex = p.getIndex();
        }
        if(op!=0){ 
            if(bufcurrentindex > (buflastindex+1)){
                 tmp = p.getChars(buflastindex, bufcurrentindex-1);
                 handleContent(tmp,&current,resultlist);
            } 
            cNext = p.getTexChar();
            if(cNext == '=')
                current->beiyong_opname = '=';
            else
                p.ungetTexChar();
            current->op = op ;
            current->opname = cThis ;
            current->nexteq = new EQS();
            current = current->nexteq ;
            buflastindex = p.getIndex();
            op = 0 ;
        }
    }
    //zhe bufen shifang caozuo zenme zuo 
    if(current->content!=NULL){
        current->op = 0 ;
        current->nexteq = new EQS();
        current = current->nexteq ;
    }
    handleContent(p.getChars(buflastindex,bufcurrentindex),&current,resultlist);
    current->nexteq = NULL ;
//    combineEquations(head,&currentResult);
    EQS::combineEquations(head,resultlist);
    list<char*>::iterator iter ;
    for(iter = currentResult.begin() ; iter != currentResult.end() ; iter++){
        resultlist->push_back(*iter);
    }
}
