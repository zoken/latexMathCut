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

void Equation::CmdEquation(int code)
/******************************************************************************
 purpose   : Handle everything associated with equations
 ******************************************************************************/
{
	char *pre, *eq, *post;
	int inline_equation, true_code;

//	true_code = code & ~ON;	
			
//	if (!(code & ON)) return ;

	SlurpEquation(code,&pre,&eq,&post);
	
	diagnostics(4, "Entering CmdEquation --------%x\n<%s>\n<%s>\n<%s>",code,pre,eq,post);
    printf("equations:%s/n",eq);
	inline_equation = (true_code == EQN_MATH) || (true_code == EQN_DOLLAR) || (true_code == EQN_RND_OPEN);
	
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

	numerator = parser->getBraceParam();
	nptr = strdup_noendblanks(numerator);
	parser->skipSpaces();
	denominator = parser->getBraceParam();
	dptr = strdup_noendblanks(denominator);

	return strdup_together(nptr,denominator);
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

	if (code == 0)
		s=strdup("lim");
	else if (code == 1)
		s=strdup("lim sup");
	else
		s=strdup("lim inf");
	return strdup_together(s,lower_limit);
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
	char delim;

	delim = parser->getTexChar();
	if (delim == '\\')			/* might be \{ or \} */
		delim = parser->getTexChar();
	return "\left||\right";
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
