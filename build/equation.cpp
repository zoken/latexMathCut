/* equation.c - Translate TeX equations

Copyright (C) 1995-2002 The Free Software Foundation

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

This file is available from http://sourceforge.net/projects/latex2rtf/
 
Authors:
    1995-1997 Ralf Schlatterbeck
    1998-2000 Georg Lehner
    2001-2002 Scott Prahl
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

extern "C"{
#include "tools.h"
#include "util.h"
}
#include "latexparser.h"
#include "equation.h"

void Equation::regParser(Parser* _parser){
	this->parser = _parser;
}

char * Equation::SlurpDollarEquation(void)
/******************************************************************************
 purpose   : reads an equation delimited by $...$
 			 this routine is needed to handle $ x \mbox{if $x$} $ 
 ******************************************************************************/
{
int brace=0;
int slash=0;
int i;
char *s, *t, *u;

	s = (char*)malloc(1024*sizeof(char));
	t = s;
	
	for (i=0; i<1024; i++) {
		*t = parser->getTexChar();
		if (*t == '\\') 
			slash++;
		else if (*t == '{' && (slash % 2) == 0 &&
			!(i>5 && strncmp(t-5,"\\left{", 6)==0) &&
			!(i>6 && strncmp(t-6,"\\right{", 7)==0))
				brace++;
		else if (*t == '}' && (slash % 2) == 0 &&
			!(i>5 && !strncmp(t-5,"\\left}", 6)) &&
			!(i>6 && !strncmp(t-6,"\\right}", 7)))
				brace--;
		else if (*t == '$' && (slash % 2) == 0 && brace == 0) {
			break;
		} else
			slash = 0;
		t++;
	}
	*t = '\0';
	
	u = strdup(s);	/* return much smaller string */
	free(s);		/* release the big string */
	return u;
}

void Equation::SlurpEquation(int code, char **pre, char **eq, char **post)
{
	int true_code = code & ~ON;
	
	switch (true_code) {
	
		case EQN_MATH:
			diagnostics(4, "SlurpEquation() ... \\begin{math}");
			*pre  = strdup("\\begin{math}");
			*post = strdup("\\end{math}");
			*eq = parser->getTexUntil(*post,0);
			break;
			
		case EQN_DOLLAR:
			diagnostics(4, "SlurpEquation() ... $");
			*pre  = strdup("$");
			*post = strdup("$");
			*eq = SlurpDollarEquation();
			break;
	
		case EQN_RND_OPEN:
			diagnostics(4, "SlurpEquation() ... \\(");
			*pre  = strdup("\\(");
			*post = strdup("\\)");
			*eq = parser->getTexUntil(*post,0);
			break;
	
		case EQN_DISPLAYMATH:
			diagnostics(4,"SlurpEquation -- displaymath");
			*pre  = strdup("\\begin{displaymath}");
			*post = strdup("\\end{displaymath}");
			*eq = parser->getTexUntil(*post,0);
			break;
	
		case EQN_EQUATION_STAR:
			diagnostics(4,"SlurpEquation() -- equation*");
			*pre  = strdup("\\begin{equation*}");
			*post = strdup("\\end{equation*}");
			*eq = parser->getTexUntil(*post,0);
			break;
	
		case EQN_DOLLAR_DOLLAR:
			diagnostics(4,"SlurpEquation() -- $$");
			*pre  = strdup("$$");
			*post = strdup("$$");
			*eq = parser->getTexUntil(*post,0);
			break;
	
		case EQN_BRACKET_OPEN:
			diagnostics(4,"SlurpEquation()-- \\[");
			*pre  = strdup("\\[");
			*post = strdup("\\]");
			*eq = parser->getTexUntil(*post,0);
			break;
	
		case EQN_EQUATION:
			diagnostics(4,"SlurpEquation() -- equation");
			*pre  = strdup("\\begin{equation}");
			*post = strdup("\\end{equation}");
			*eq = parser->getTexUntil(*post,0);
			break;
	
		case EQN_ARRAY_STAR:
			diagnostics(4,"Entering CmdDisplayMath -- eqnarray* ");
			*pre  = strdup("\\begin{eqnarray*}");
			*post = strdup("\\end{eqnarray*}");
			*eq = parser->getTexUntil(*post,0);
			break;
	
		case EQN_ARRAY:
			diagnostics(4,"SlurpEquation() --- eqnarray");
			*pre  = strdup("\\begin{eqnarray}");
			*post = strdup("\\end{eqnarray}");
			*eq = parser->getTexUntil(*post,0);
			break;
	}
}

int Equation::EquationNeedsFields(char *eq)
/******************************************************************************
 purpose   : Determine if equation needs EQ field for RTF conversion
 ******************************************************************************/
{
	if (strstr(eq,"\\frac")) return 1;
	if (strstr(eq,"\\sum")) return 1;
	if (strstr(eq,"\\int")) return 1;
	if (strstr(eq,"\\iint")) return 1;
	if (strstr(eq,"\\iiint")) return 1;
	if (strstr(eq,"\\prod")) return 1;
	if (strstr(eq,"\\begin{array}")) return 1;
	if (strstr(eq,"\\left")) return 1;
	if (strstr(eq,"\\right")) return 1;
	if (strstr(eq,"\\root")) return 1;
	if (strstr(eq,"\\sqrt")) return 1;
	if (strstr(eq,"\\over")) return 1;
	if (strstr(eq,"\\stackrel")) return 1;
	if (strstr(eq,"\\dfrac")) return 1;
	if (strstr(eq,"\\lim")) return 1;
	if (strstr(eq,"\\liminf")) return 1;
	if (strstr(eq,"\\limsup")) return 1;
	return 0;
}

char* Equation::CmdEquation(int code)
/******************************************************************************
 purpose   : Handle everything associated with equations
 ******************************************************************************/
{
	char *pre, *eq, *post;
	int inline_equation, true_code;
	SlurpEquation(code,&pre,&eq,&post);
	return eq;
	//do something to cut equation.
	
}

char* Equation::CmdRoot(int code)
/******************************************************************************
 purpose: converts \sqrt{x} or \root[\alpha]{x+y}
******************************************************************************/
{
	char           *root;
	char           *power;
	

	power = parser->getBracketParam();
	root = parser->getBraceParam();

	return strdup_together(power,root);
}

char* Equation::CmdFraction(int code)
/******************************************************************************
 purpose: converts \frac{x}{y} (following Taupin's implementation in ltx2rtf)
******************************************************************************/
{
	char  *denominator, *numerator, *nptr, *dptr;
	int start,end ;
	start = parser->getIndex();
	numerator = parser->getBraceParam();
	nptr = strdup_noendblanks(numerator);
	parser->skipSpaces();
	denominator = parser->getBraceParam();
	dptr = strdup_noendblanks(denominator);
	end = parser->getIndex();
	return parser->getChars(start,end);
}
void Equation::CmdChars(){
    char cThis;
    cThis = parser->getTexChar();
    while((cThis>='A'&&cThis<='Z')||(cThis>='a'&&cThis<='z')){
        cThis = parser->getTexChar();
    }
    parser->ungetTexChar();
}
char* Equation::CmdLim(int code)
/******************************************************************************
 purpose: handles \lim
parameter: 0=\lim, 1=\limsup, 2=\liminf
 ******************************************************************************/
{
	char cThis, *s, *lower_limit=NULL;

	cThis = parser->getNonBlank();
    if (cThis == '_')
        lower_limit = parser->getBraceParam();
    else
        parser->ungetTexChar();

	return lower_limit;
}

char* Equation::CmdIntegral(int code)
/******************************************************************************
 purpose: converts integral symbol + the "exponent" and "subscript" fields
parameter: type of operand
 ******************************************************************************/
{
	char           *upper_limit = NULL;
	char           *lower_limit = NULL;
	char            cThis;

	/* is there an exponent/subscript ? */
	cThis = parser->getNonBlank();

	if (cThis == '_')
		lower_limit = parser->getBraceParam();
	else if (cThis == '^')
		upper_limit = parser->getBraceParam();
	else
		parser->ungetTexChar();

	if (upper_limit || lower_limit) {
		cThis = parser->getNonBlank();
		if (cThis == '_')
			lower_limit = parser->getBraceParam();
		else if (cThis == '^')
			upper_limit = parser->getBraceParam();
		else
			parser->ungetTexChar();
	}
	return strdup_together(lower_limit,upper_limit);;
}

char* Equation::CmdSuperscript(int code)
/******************************************************************************
 purpose   : Handles superscripts ^\alpha, ^a, ^{a} and \textsuperscript{a}
 ******************************************************************************/
{
	char	*s = NULL;
	s = parser->getBraceParam();
	return s;
}

char* Equation::CmdSubscript(int code)
/******************************************************************************
 purpose   : Handles superscripts ^\alpha, ^a, ^{a}
 ******************************************************************************/
{
	char	*s = NULL;
	s = parser->getBraceParam();
	return s;
}

char* Equation::CmdLeftRight(int code)
/******************************************************************************
 purpose   : Handles \left \right
 			 to properly handle \left. or \right. would require prescanning the
 			 entire equation.  
 ******************************************************************************/
{ 
    char* start = "left";
	char* stop = "right";
    char* cmd ;
    char c;
    int startindex, endindex ;

    startindex = parser->getIndex();
    endindex = -1;
    c = parser->getTexChar();
    while((c=parser->getTexChar())!='\0'){
        if(c=='\\'){
            cmd = parser->getCmd();
            if(strcmp(cmd,start)==0){
                CmdLeftRight(code);
            }else if(strcmp(cmd,stop)==0){
                endindex = parser->getIndex()-6;//\right = 6//because index++
                c = parser->getTexChar();
                if(c!='.')
                    parser->ungetTexChar();
                break;
            }
        }
    }
    if(endindex == -1)
        endindex = parser->getIndex();
	return parser->getChars(startindex,endindex);
}

char* Equation::CmdArray(int code)
/******************************************************************************
 purpose   : Handles \begin{array}[c]{ccc} ... \end{array}
 ******************************************************************************/
{
	char * v_align, * col_align, *s;
	int n=0;

	if (code & ON) {
		v_align = parser->getBracketParam();
		col_align = parser->getBraceParam();
		diagnostics(4, "CmdArray() ... \\begin{array}[%s]{%s}", v_align?v_align:"", col_align);
		if (v_align) free(v_align);
		
		s = col_align;
		while (*s) {
			if (*s == 'c' || *s == 'l' || *s == 'r' ) n++;
			s++;
		}
		free(col_align);
		return "(";
		
	} else {
		return ")";
	}
}

char* Equation::CmdStackrel(int code)
/******************************************************************************
 purpose   : Handles \stackrel{a}{=}
 ******************************************************************************/
{
	char * numer, *denom;
	numer = parser->getBraceParam();
	denom = parser->getBraceParam();
	diagnostics(4, "CmdStackrel() ... \\stackrel{%s}{%s}", numer,denom);
	return strdup_together(numer,denom) ;
}
char* Equation::CmdKuoHao(char start,char end){
    int need = 1;
    char cThis ;
    int begin = parser->getIndex();
    if(start=='('){
        while((cThis = parser->getTexChar())!='\0'){
            if(cThis == '\357'){
                cThis = parser->getTexChar();
                if(cThis == '\274'){
                    cThis = parser->getTexChar();
                    if(cThis == '\210')
                        need++;
                    else if(cThis == '\211'){
                        need--;
                        if(need<=0){
                            return parser->getChars(begin,parser->getIndex()-3);
                        }
                    }
                }
            }else if(cThis == ')'){
                need--;
                if(need <= 0 )
                    return parser->getChars(begin,parser->getIndex()-1);
            }else if(cThis =='('){
                need++;
            }
        }
    }else{
        while((cThis = parser->getTexChar())!='\0'){
            if(cThis == end){
                need--;
                if(need <= 0 )
                    break;
            }else if(cThis == start){
                need++;
            }
        }
    }
    return parser->getChars(begin,parser->getIndex()-1);
}

char* Equation::CmdBeginEnd(int code){
    char* stop = "end";
    int need = 1 ; 
    char* start = "begin";
    char c ;
    char* cmd ;
    char* tmpcontent;
    int startindex,endindex;
    while((c=parser->getTexChar())!='{');//{array}
    tmpcontent = parser->getDelimitedText('{','}',false);
    if((c=parser->getTexChar())=='{');//{1} // no space betwwen [array}
        tmpcontent = strdup_together(tmpcontent,parser->getDelimitedText('{','}',false));
    startindex = parser->getIndex();
    endindex = -1 ;
    while((c=parser->getTexChar())!='\0'){
        if(c=='\\'){
            cmd = parser->getCmd();
            if(strcmp(cmd,start)==0){
                CmdBeginEnd(code);
            }else if(strcmp(cmd,stop)==0){
                endindex = parser->getIndex()-4;//\end
                while((c=parser->getTexChar())!='{');
                parser->getDelimitedText('{','}',false);
                break;
            }
        }
    }
    if(endindex == -1)
        endindex = parser->getIndex();
    return parser->getChars(startindex,endindex);
}
