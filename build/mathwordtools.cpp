#include "mathwordtools.h"
#include <stdlib.h>
#include <string.h>
#include "latexparser.h"
#include "equation.h"
extern "C"{
#include "util.h"
#include "tools.h"
}
char* getCmd(Parser *p){
	char buf[20];
	int num = 0 ;
	char c ;
	c = p->getTexChar();
	while(c<='z'&&c>='a'){
		buf[num++] = c ;
		c = p->getTexChar();
	}
	p->ungetTexChar();
	buf[num++]='\0';
	char* ret = (char*)malloc(sizeof(char)*num);
	strcpy(ret,buf);
	return ret ;
}
void MathWordTool::cut(const char* sentence, list<char*>* result){
	Parser p ;
	p.setSentence(sentence);
	Equation q ;
	q.regParser(&p);
    
    char            cThis = '\n';
	char            cLast = '\0';
	char            cNext;
	int				mode, count,index ;
	char* cmd;
	char buf[100]; //un CmdEquation.
	int bufIndex = 0 ;
//    result->PushWord("");
	while ((cThis = p.getTexChar()) && cThis != '\0') {
		switch (cThis) {

		case '\\':
			//TranslateCommand();
			//?
//			buf[bufIndex]='\0';
//			result->WriteEquation(buf);
			bufIndex=0;
	//		printf("words:buf=%s\n",buf);
			cmd = getCmd(&p);
			index = p.getIndex();
//			result->PushWord(cmd);
			char* param ;
			if(strcmp(cmd,"frac")==0||strcmp(cmd,"dfrac")==0||strcmp(cmd,"Frac")==0){
				param = q.CmdFraction(0);
			}else if(strcmp(cmd,"sqrt")==0||strcmp(cmd,"root")==0){
				param = q.CmdRoot(0);
			}else if(strcmp(cmd,"lim")==0||strcmp(cmd,"limsup")==0||strcmp(cmd,"liminf")==0){
				param = q.CmdLim(0);
			}else if(strcmp(cmd,"sum")==0||strcmp(cmd,"prod")==0||strcmp(cmd,"int")==0||strcmp(cmd,"iint")==0){
				param = q.CmdIntegral(0);
			}else if(strcmp(cmd,"left")==0||strcmp(cmd,"right")==0){
				param = q.CmdLeftRight(0);
			}else if(strcmp(cmd,"stackrel")==0){
				param = q.CmdStackrel(0);
			}else if(strcmp(cmd,"textsuperscript")==0){
				param = q.CmdSuperscript(0);
			}else{
				param="";
			}
//			result->WriteEquation(param);
//			result->words.push_back(result->PopWord());
			printf("words:cmd=%s,param=%s\n",cmd,param);
            result->push_back(strdup_together(cmd,param));
			p.setIndex(index);
			break;
			
			
		case '{':
			//PushBrace();
			break;
			
		case '}':
			//PopBrace();
			break;

		case ' ':
			//continue ;
			break;
			
		case '\n': 
			//as ' ' continue ;
			break;


		case '$':
			index = p.getIndex();
			cNext = p.getTexChar();
			char* eq ;
			if (cNext == '$'){
				eq = q.CmdEquation(EQN_DOLLAR_DOLLAR | ON);
			}
			else {
				p.ungetTexChar();
				eq = q.CmdEquation(EQN_DOLLAR | ON);
			}
            result->push_back(eq);
//			result->words.push_back(eq);
			p.setIndex(index);
			/* 
			   Formulas need to close all Convert() operations when they end 
			   This works for \begin{equation} but not $$ since the BraceLevel
			   and environments don't get pushed properly.  We do it explicitly here.
			*/
			/*
			if (GetTexMode() == MODE_MATH || GetTexMode() == MODE_DISPLAYMATH)
				PushBrace();
			else {
				ret = RecursionLevel - PopBrace();
				if (ret > 0) {
					ret--;
					RecursionLevel--;
					diagnostics(5, "Exiting Convert via Math ret = %d", ret);
					return;
				}
			}
			*/
			break;

		case '&':
			// add new equation?
			break ;

		case '~':
			//continue;
			break;			
			
		case '^':		
			q.CmdSuperscript(0);//ping fang ?
			break;

		case '_':		
			q.CmdSubscript(0);
			break;

		case '-':
			//what is it?
			break;
						
		case '\'':
			//continue ;
			break;
			
		case '`':
			//continue ;
			break;
			
		case '\"':
			//continue ;
			break;

		case '<':
			//continue ;
			break;

		case '>':
			//continue ;
			break;

		case '!':
			//continue ;
			break;	
			
		case '?':
			//continue ;
			break;
			
		case ':':
			//continue ;
			break;	

		case '.':
			// continue ;
			break;
			
		case '\t':
			diagnostics(WARNING, "This should not happen, ignoring \\t");
			cThis = ' ';
			break;
			
		case '\r':
			diagnostics(WARNING, "This should not happen, ignoring \\r");
			cThis = ' ';
			break;
			
		case '%':
			diagnostics(WARNING, "This should not happen, ignoring %%");
			cThis = ' ';
			break;

		case '(':
			//continue ;
			break;
			
		case ')':
			//continue ;
			break;

		case ';':
			//continue ;
			break;

		case ',':
			//continue ;
			break;
		case '=':
//			buf[bufIndex]='\0';
			break ;
		default:
			//continue;
			//add to buf.
//			buf[bufIndex++]=cThis;
			break;
		}
		cLast = cThis;
	}
}
