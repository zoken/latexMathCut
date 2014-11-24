#include "mathinterface.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "latexparser.h"
#include "equation.h"

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
/***************************************
如果是，则返回next_index
否则，返回当前index
***************************************/
int isMathWord(const char* sentence , int start_index , int sentencelen ){
    int i = start_index; 
    if(start_index >= sentencelen)
        return start_index ;
    else{
        if((sentence[i]>='a'&&sentence[i]<='z')||(sentence[i]>='A'&&sentence[i]<='Z')||(sentence[i]>='0'&&sentence[i]<='9')){
            return i+1;
        }else if((start_index+2) < sentencelen){
            if(sentence[i] == '\342' && sentence[i+1] == '\210' && sentence[i+2] =='\240')
                return i+3 ;
            else if(sentence[i] == '\342' && sentence[i+1] == '\226' && sentence[i+2] =='\263')
                return i+3 ;
            else 
                return i;
        } else if (sentence[i] == '\\' || sentence[i] == '$'){
            return i+1;
        } else{
            return i;
        }
    }
}
/************************************************
获取从index开始的一个数学单词，如没有，则返回NULL
*************************************************/
char* getMathWord(char* sentence , int index , int sentencelen ){
    int i = index ;
    int last = index ;
    char* ret = NULL ;
    while(last!=(i = isMathWord(sentence , last , sentencelen))){
        last =  i;
    }
    if(i!=index){
        ret = (char*)malloc(sizeof(char)*(i - index + 1 ));
        memcpy(ret, sentence+index , i-index);
        ret[i-index] = '\0';
    }
    return ret ;
}
/*****************************
    method:handleContent.本部分用于处理数学公式不可分的部分。提取头尾中可能成为数学单词的部分，加入表达式处理的结构
        中间部分使用中文分词chineseCut
    word:需要处理的单词
    current:表达式处理的结构
    resultlist:最终结果
    内存增量：头、尾、中间部分（调用中文分词的话，则释放）
****************************/
void MathSegmentTool::handleContent(char* word, EQS** current, int start_index , vector<WordPos*>& results){
    if(word == NULL)
        return ;
    else if(strlen(word)<=0)
        return ;
    int right = strlen(word);
    //提取word头部的数学单词
    char* childword = getMathWord(word, 0 ,right);
    WordPos* tmpwordpos ;
    if(childword!= NULL){
        EQS::AddContent(childword,start_index , current);
        if (cutMode == CUT_MODE_SIMPLE || cutMode == CUT_MODE_MIX ) {
            tmpwordpos = new WordPos();
            tmpwordpos->word = childword ;
            tmpwordpos->pos = start_index ;
            results.push_back(tmpwordpos);
        }
    }
    int left_end = childword == NULL ? 0 :strlen(childword);
    int i = left_end ;
    int last  = i ;
    int right_start = -1;
    while(i < right){
        if((i=isMathWord(word,last,right))==last){
            right_start = -1 ;
            i++;
        }else if(right_start<0){
            right_start = last ;
        }
        last = i ;
    }
    if(right_start > 0){
        childword = (char*)malloc(sizeof(char)*(right+1 - right_start));
        strcpy(childword, word+right_start);
        EQS::AddContent(childword,start_index+right_start,current);
        if (cutMode == CUT_MODE_SIMPLE || cutMode == CUT_MODE_MIX) {
            tmpwordpos = new WordPos();
            tmpwordpos->word = childword ;
            tmpwordpos->pos = start_index + right_start ;
            results.push_back(tmpwordpos);
        }
    }
    right_start = right_start < 0 ? right : right_start ;
    if(left_end < right_start){
        childword = (char*)malloc(sizeof(char)*(right_start+1 - left_end));
        memcpy(childword, word+left_end , right_start-left_end);
        childword[right_start - left_end] = '\0';
        start_index = start_index + left_end ;
        cutChineseWord(childword, start_index , results);
        free(childword);
    }
    childword = NULL ;
}
/**************************************
获取某些词在句子中的位置,返回wordpos
**************************************/
void findWordPoses(char* sentence ,int start_index , vector<char*>& words , vector<WordPos*>& result){
    vector<char*>::iterator iter,count_iter;
    vector<int> starts ;
    vector<int> ends ;
    int i,timeCount ,tmppos;
    int len = strlen(sentence);
    char* tmp_sentence ;
    WordPos* tmpwordpos = NULL ;
    for(iter = words.begin(); iter!=words.end();iter++){
    //查找*iter已出现的次数    
        timeCount = 0 ;
        for(count_iter = words.begin() ; count_iter!=iter ;count_iter++){
            if(strcmp(*iter,*count_iter)==0){
                timeCount++;
            }
        }
        tmp_sentence = sentence ;
        for( i = 0 ; i < timeCount ; i++){
            tmp_sentence = strstr(tmp_sentence , *iter);
            if(tmp_sentence == NULL){
                break ;
            }
            tmp_sentence++;//指向下一段字符串，以便寻找下一个出现的单词
        }
        if(tmp_sentence ==NULL)
            continue ;
        else{
            tmp_sentence = strstr(tmp_sentence , *iter);
            if(tmp_sentence == NULL)
                tmppos = -1;
            else{
                tmp_sentence = strstr(tmp_sentence , *iter);
                if(tmp_sentence == NULL)
                    tmppos = -1;    
                else{
                    tmppos = len - strlen(tmp_sentence);
                }
                tmppos += start_index;
                tmpwordpos = new  WordPos();
                tmpwordpos->word = *iter ;
                tmpwordpos->pos = tmppos ;
                result.push_back(tmpwordpos);
            }
        }
    }
}
void combineEquations(EQS* head , vector<WordPos*>& results){
/*************************************************************
method：combineEquations
用途：合并公式表达式
输入EQS列表，输出可以结合的数学表达式，并存入切词结果results
内存处理：释放整个公式链表，释放所有非空EQS.content
****************a*********************************************/
    int currentMaxOp = 0 ;
    EQS *maxEquations,*current,*tofree_eq ;
    char* tmp ;
    if(head == NULL)
        return;
    while(head->nexteq!=NULL){
        current = head ;
        maxEquations = head ;
        currentMaxOp = 0;
        while(current->nexteq!=NULL){
            if(current->op == 0)
                break ;
            if(current->op > currentMaxOp){
                currentMaxOp = current->op ;
                maxEquations = current ;
            }
            current = current->nexteq ;
        }
        if(head == current){
            head = current->nexteq ;
            delete current;
            continue ;
        }
        current = maxEquations;
        if(current->content==NULL||current->nexteq->content==NULL){
            head = current->nexteq; //error operate.
            free(current);
            continue ;
        }
        int currentLen = strlen(current->content);
        int nextLen = strlen(current->nexteq->content);
        if(current->beiyong_opname=='\0'){
            tmp = (char*)malloc(sizeof(char)*(currentLen+nextLen+2));
            strcpy(tmp,current->content);
            tmp[currentLen] = current->opname ;
            strcpy(tmp+currentLen+1,current->nexteq->content);
        }else{
            tmp = (char*)malloc(sizeof(char)*(currentLen+nextLen+3));
            strcpy(tmp,current->content);
            tmp[currentLen] = current->opname ;
            tmp[currentLen+1] = current->beiyong_opname;
            strcpy(tmp+currentLen+2,current->nexteq->content);
        }
        current->content = tmp ;
        tmp = (char*)malloc(sizeof(char)*(strlen(tmp)+1));
        strcpy(tmp,current->content);
        current->op = current->nexteq->op ;
        current->beiyong_opname = current->nexteq->beiyong_opname;
        current->opname = current->nexteq->opname ;
        tofree_eq = current->nexteq ;
        current->nexteq = current->nexteq->nexteq;
        delete tofree_eq;
        WordPos* wordpos = new WordPos();
        wordpos->word = tmp ; 
        wordpos->pos = current->pos ;
        results.push_back(wordpos);
    }
}
/******************************************************
*   中文分词函数。通过制定CHINESE_USE
*   在编译阶段指定分词的工具。初始化操作在外部执行
******************************************************/
void MathSegmentTool::cutChineseWord(char* content, int start_index , vector<WordPos*>& results){
    if(_segment == NULL)
        return ;
    vector<char*> tmpwords ;
    _segment->cut(content,tmpwords);
    findWordPoses(content ,start_index , tmpwords , results);
    tmpwords.clear();
}
void MathSegmentTool::cut(const char* sentence,vector<WordPos*>& results){
    cut(sentence , results, 0);
}
/****************************************************************
*** 分词主函数。本函数执行包括两个功能：                     ***
*** 1. 区分公式和中文，并调用不同模块                        ***
*** 2. 对数学公式执行词法分析                                ***
***************************************************************/
void MathSegmentTool::cut(const char* sentence, vector<WordPos*>& results , int index){
    if(sentence == NULL)
        return ;
    else if(strlen(sentence)<=0)
        return ;
	Parser p ;
	p.setSentence(sentence);
	Equation q ;
    WordPos* tmp_wordpos ;
	q.regParser(&p);
    char cThis = '\n',cEnd = '\n', cNext = '\n', cNext_Next = '\0';
    int buflastindex = 0, bufcurrentindex = 0;
    int op = 0 ; //must init. else op!=0 will execute with error message.
    int tmp_index = 0 ;
    char *word , *tmp , *cmd ;
    EQS *head,*current; 
    head = new EQS();
    current = head ;
    while((cThis = p.getRawTexChar())!='\0'){
        bufcurrentindex = p.getIndex();
        switch(cThis){
            case '$':
                /*****************************************
                update at 2014.11.17 , latex is not a full expression.
                ******************************************/
                word = NULL ;
                //if we have content which has not been handled at the head of '$' sentence ;
                if(bufcurrentindex > (buflastindex+1)){
                    bufcurrentindex = p.getIndex();
                    word = p.getChars(buflastindex,bufcurrentindex-1);
                    handleContent(word,&current, buflastindex+index , results);
                }
                buflastindex = bufcurrentindex;//escape '$';
                /*******************
                cNext = p.getRawTexChar();
                if (cNext == '$'){
                    tmp_index = p.getIndex();
                    word = q.CmdEquation(EQN_DOLLAR_DOLLAR | ON);
                }else {
                    p.ungetTexChar();
                    tmp_index = p.getIndex();
                    word = q.CmdEquation(EQN_DOLLAR | ON);
                }
                //get the end index of '$' sentence ;
                bufcurrentindex = p.getIndex();
                EQS::AddContent(p.getChars(buflastindex,bufcurrentindex),buflastindex+index , &current); //reg '$' sentence to current level's content ; to be used as operate content.
                cut(word,results,index+tmp_index);// cut the '$' sentence into small pieces.
                buflastindex = bufcurrentindex ;
                *******************************/
                break;
            case '\\':
                // handle the content before cmd.
                word = NULL ;
                if(bufcurrentindex > buflastindex+1){
                    bufcurrentindex = p.getIndex();
                    word = p.getChars(buflastindex,bufcurrentindex-1);
                    handleContent(word,&current, buflastindex+index ,results);
                }
                //move index to '\' as the head of cmd sentence.
                buflastindex = bufcurrentindex -1; 
                cNext = p.getRawTexChar();
                if(cNext == '\\'){//only is a gap.
                    buflastindex = p.getIndex();
                    break ;
                }else{
                    p.ungetTexChar();
                }
                //get Current Cmd .
                cmd = p.getCmd();
                if(cmd == NULL)
                    continue ;
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
                    cNext = p.getRawTexChar();
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
                bufcurrentindex = p.getIndex();
                tmp_wordpos = new WordPos();
                tmp_wordpos->word = p.getChars(buflastindex,bufcurrentindex);
                tmp_wordpos->pos = buflastindex+index ;
                results.push_back(tmp_wordpos);
                EQS::AddContent(tmp_wordpos->word , tmp_wordpos->pos,&current);
                if(param != NULL)
                    cut(param,results,tmp_wordpos->pos+strlen(cmd)+1);
                buflastindex = bufcurrentindex ;
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
                cNext = p.getRawTexChar();
                if(cNext == '\274'){
                    cNext = p.getRawTexChar();
                    if(cNext == '\210'){//中文括号
                        cThis = '(';
                        cEnd = '?';
                    }else if(cNext == '\214'){//中文逗号
                        if(bufcurrentindex > (buflastindex+1)){
                            word = p.getChars(buflastindex, bufcurrentindex-1);
                            handleContent(word,&current, buflastindex+index, results);
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
                cNext = p.getRawTexChar();
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
            //公式中的三角形和角度应该是两种不同的操作，一种是向后合并，一种是向前合并
            //角度不做处理，只处理三角形
            case '\342': 
                cNext = p.getRawTexChar();
                cNext_Next = p.getRawTexChar();
                //角度和三角形
                //角度为\342,\210,\240
                if((cNext == '\226'&&cNext_Next == '\263')){
                    bufcurrentindex = p.getIndex();
                    word = p.getChars(buflastindex,bufcurrentindex-3);
                    handleContent(word,&current,buflastindex+index, results);
                    buflastindex = bufcurrentindex -3 ;
                    p.getMathWord();
                    bufcurrentindex = p.getIndex();
                    word = p.getChars(buflastindex,bufcurrentindex);
                    tmp_wordpos = new WordPos();
                    tmp_wordpos->word = word ;
                    tmp_wordpos->pos = buflastindex+index ; 
                    results.push_back(tmp_wordpos);
                    EQS::AddContent(word,buflastindex+index,&current);
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
                    handleContent(word,&current,buflastindex+index, results);
                    buflastindex = p.getIndex();
                }
                break;                
            default:
                break ;
        }
        if(cEnd!='\n'){
            if(bufcurrentindex > (buflastindex+1)){
                word = p.getChars(buflastindex, bufcurrentindex-1);
                handleContent(word,&current, buflastindex+index,results);
            }
            if(cThis == '(' && cEnd == '?' )
                buflastindex = p.getIndex()-3;
            else
                buflastindex = p.getIndex()-1;
            tmp_index = p.getIndex(); 
            word = q.CmdKuoHao(cThis,cEnd);
            bufcurrentindex = p.getIndex();
            tmp_wordpos = new WordPos();
            tmp_wordpos->word = p.getChars(buflastindex,bufcurrentindex);
            tmp_wordpos->pos = buflastindex+index ;
            results.push_back(tmp_wordpos);
            EQS::AddContent(tmp_wordpos->word, tmp_wordpos->pos ,&current);
            cut(word,results,index+tmp_index);
            cEnd = '\n';//handle '{}' and so on.
            buflastindex = p.getIndex();
        }
        if(op!=0){ 
            if(bufcurrentindex > (buflastindex+1)){
                 tmp = p.getChars(buflastindex, bufcurrentindex-1);
                 handleContent(tmp,&current,buflastindex+index, results);
            } 
            cNext = p.getRawTexChar();
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
    if(buflastindex < bufcurrentindex)
        handleContent(p.getChars(buflastindex,bufcurrentindex),&current,buflastindex+index, results);
    current->nexteq = NULL ;
    if (cutMode == CUT_MODE_COMPLEX || cutMode == CUT_MODE_MIX) {
        combineEquations(head,results);
    }
}
#define MATH_MODE 0 
#define CHIN_MODE  1 
int isChinWord(const char* sentence, int index, int strlen){
    int i = index; 
    for (;i < strlen;){
        if (sentence[i]&0x80){
            if (sentence[i+1]&0x40){
                i = i + 2 ;
            } else {
                break;
            }
        } else if(sentence[i]&0xDF){
            if (sentence[i+1]&0x5E){
                i = i + 2 ;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    return i ;
}

void MathSegmentTool::fullCut(const char* sentence, int start, int strlen, vector<WordPos*>& results){
    int currentmode = -1;
    int lastmode = -1;

    //设置数学和中文的buf
    char math_buf[1024];
    char chin_buf[1024];
    char* tmp_sentence = (char*)malloc(sizeof(char)*1024);
    int math_index = 0 ;
    int chin_index = 0;
    int current_index = 0 ;

    int i, j;
    for (i = start ; i < strlen ; ){
        //对括号特殊处理，截取，并调用fullCut

        //判断是不是数学字符，如果是，则对前面的中文执行分词操作
        //将当前数据加入数学buf。包含对特殊数学字符的处理，如中文加号等
        current_index = isMathWord(sentence, i, strlen);
        if (current_index > i ){
            currentmode = MATH_MODE;
            if (lastmode == CHIN_MODE){
                chin_buf[chin_index]='\0';
                //调用中文分词
                strcpy(tmp_sentence,chin_buf);
                cutChineseWord(tmp_sentence, i-math_index, results);
                chin_index = 0;
            }
            for (j = i ; j < current_index ; j++){
                math_buf[math_index++] = sentence[j];
            }
            lastmode = currentmode;
            i = current_index ;
            continue ;
        }
        //如果不是数学字符，则判断是否为中文字符，如果是，则加入中文buf
        //并对前面的数学公式执行分词操作
        current_index = isChinWord(sentence, i, strlen);
        if (current_index > i){
            currentmode = CHIN_MODE ;
            if (lastmode == MATH_MODE){
                math_buf[math_index] = '\0';
                //调用数学分词
                strcpy(tmp_sentence, math_buf);
                cut(tmp_sentence, results, i-chin_index);
                math_index = 0 ;
            }
            for (j = i ; j < current_index ; j++){
                chin_buf[chin_index++] = sentence[j];
            }
            lastmode = currentmode;
            i = current_index ;
            continue ;
        }
        //不是中文也不是数字，这时候参考前一个字符的编码归属
        if (lastmode == MATH_MODE){
            math_buf[math_index++] = sentence[i];
        } else if(lastmode == CHIN_MODE){
            chin_buf[chin_index++] = sentence[i];
        }
        i++;
    }
    if (math_index > 0 ){
        math_buf[math_index] = '\0';
        strcpy(tmp_sentence, math_buf);
        cut(tmp_sentence, results, i-math_index);
    }
    if (chin_index > 0 ){
        chin_buf[chin_index] = '\0';
        strcpy(tmp_sentence, chin_buf);
        cutChineseWord(tmp_sentence, i-chin_index, results);
    }
}
