/* $Id: util.h,v 1.18 2002/05/02 14:47:20 prahl Exp $ */

//char *  strndup(char *s, int n);
char *  strdup_together(char *s, char *t);
char *	strdup_noblanks(char *s);
char *	strdup_nobadchars(char *s);
char *	strdup_noendblanks(char *s);
char *	ExtractLabelTag(char *text);


#ifdef USER_SCALE
double getUserPngScale();
double setUserPngScale(double sc);  /* return old value */
#endif

#define TRUE 1
#define FALSE 0
