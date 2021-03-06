/* util.c - handy strings routines

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
#include <string.h>
extern "C"{
#include "util.h"
#include "tools.h"
}
/******************************************************************************
 purpose:  returns a new string with n characters from s (with '\0' at the end)
******************************************************************************/
/*
char *   
strndup(char *src, int n)
{
	char *dst;
	
	dst = (char *) calloc(n+1,sizeof(char));
	if (dst==NULL) return NULL;
		
	strncpy(dst,src,n);

	return dst;
}
*/

char *  
strdup_together(char *s, char *t)
/******************************************************************************
 purpose:  returns a new string consisting of s+t
******************************************************************************/
{
	char * both;
	
	if (s==NULL) {
		if (t==NULL) return NULL;
		return strdup(t);
	}
	
	if (t==NULL)
		return strdup(s);
		
	both = (char*)malloc(strlen(s) + strlen(t) + 1);
//	if (both == NULL)
//		diagnostics(ERROR, "Could not allocate memory for both strings.");

	strcpy(both, s);
	strcat(both, t);
	return both;
}

char *
strdup_noblanks(char *s)
/******************************************************************************
 purpose:  duplicates a string without including spaces or newlines
******************************************************************************/
{
char *p, *dup;
	if (s==NULL) return NULL;
	while (*s == ' ' || *s == '\n') s++;	/* skip to non blank */
	dup = (char*)malloc(strlen(s) + 1);
	p = dup;
	while (*s) {
		*p = *s;
		if (*p != ' ' && *p != '\n') p++;	/* increment if non-blank */
		s++;
	}
	*p = '\0';		
	return dup;
}

char * 
strdup_nobadchars(char * text)
/*************************************************************************
purpose: duplicate text with only a..z A..Z 0..9 and _
 ************************************************************************/
{
	char *dup, *s;
	
	dup = strdup_noblanks(text);
	s = dup;
	
	while (*s) {
		if (!('a' <= *s && *s <= 'z') &&
		    !('A' <= *s && *s <= 'Z') &&
		    !('0' <= *s && *s <= '9'))
			*s = '_';
		s++;
	}

	return dup;
}

char * 
strdup_noendblanks(char * s)
/******************************************************************************
 purpose:  duplicates a string without spaces or newlines at front or end
******************************************************************************/
{
char *p, *t;

	if (s==NULL) return NULL;
	if (*s=='\0') return strdup("");
	
	t=s;
	while (*t == ' ' || *t == '\n') t++;	/* first non blank char*/
	
	p = s + strlen(s) -1;
	while (p >= t && (*p == ' ' || *p == '\n')) p--;	/* last non blank char */
	
	if (t>p) return strdup("");
	return strndup(t,p-t+1);
}
/*
char *
ExtractLabelTag(char *text)
*/
/******************************************************************************
  purpose: return a copy of tag from \label{tag} in the string text
 ******************************************************************************/
 /*
{
	char *s, *label_with_spaces, *label;
	
	s = strstr(text,"\\label{");
	if (!s) s = strstr(text,"\\label ");
	if (!s) return NULL;
	
	s += strlen("\\label");
	PushSource(NULL,s);
	label_with_spaces = getBraceParam();
	PopSource();
	label = strdup_nobadchars(label_with_spaces);
	free(label_with_spaces);

	diagnostics(4, "LabelTag = <%s>", (label) ? label : "missing");
	return label;
}
*/

#ifdef USER_SCALE

static double user_png_scale=-1.0;

double getUserPngScale()
{ if(user_png_scale==-1)user_png_scale=g_user_eq_scale>0?g_user_eq_scale:1.0;
  return user_png_scale;
}

double setUserPngScale(double sc)
{ double rc=user_png_scale;
  if(sc>0) user_png_scale=sc;
  else     user_png_scale=g_user_eq_scale;
  return rc;
}

#endif
