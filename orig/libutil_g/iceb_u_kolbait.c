/*$Id: iceb_u_kolbait.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*07.04.2010	30.09.2009	Белых А.И.	iceb_u_kolbait.c
Возвращает количество байт которые занимает указанное количество символов в строке
*/
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "iceb_util.h"
int iceb_u_kolbait(int kolih_simv,const char *str)
{
if(g_utf8_validate(str,-1,NULL) == TRUE) /*текст в utf8 кодировке*/
 {
  int kolprob=kolih_simv-g_utf8_strlen(str,-1);
  gchar strout[strlen(str)+1];
  memset(strout,'\0',sizeof(strout));
  g_utf8_strncpy(strout,str,kolih_simv);
  int kolihbait=strlen(strout);
  if(kolprob > 0)
   kolihbait+=kolprob;
  return(kolihbait);
 }

return(kolih_simv);

}
