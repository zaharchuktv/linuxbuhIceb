/*$Id: iceb_u_strncpy.c,v 1.2 2011-01-13 13:50:05 sasa Exp $*/
/*08.04.2010	08.04.2010	Белых А.И.	iceb_u_strncpy.c
копирование заданного количества символов
*/
#include <glib.h>
#include <string.h>
int iceb_u_strncpy(char *tocop,const char *fromcop,int kolsimv)
{

if(g_utf8_validate(fromcop,-1,NULL) == TRUE) /*текст в utf8 кодировке*/
 {
  g_utf8_strncpy(tocop,fromcop,kolsimv);
 }
else
 strncpy(tocop,fromcop,kolsimv);

return(0);
}
