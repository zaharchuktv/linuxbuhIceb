/*$Id: iceb_tokoi8u.c,v 1.2 2011-01-13 13:49:59 sasa Exp $*/
/*08.03.2010	08.03.2010	Белых А.И.	iceb_tokoi8u.c
Конвертация строки в кодировку koi8u
*/
#include <glib.h>
#include "iceb_libbuh.h"

extern class iceb_u_str kodirovka_iceb; /*определяется в iceb_start.c iceb_nastsys.c*/

const char *iceb_tokoi8u(const char *fromstr)
{
if(iceb_u_strstrm(kodirovka_iceb.ravno(),"koi8") == 1)
 return(fromstr);
 
static gchar *tostr=NULL;
if(tostr != NULL)
 g_free(tostr);

gchar fromkod[56];
gchar tokod[56];
gchar fallbak[8];

strncpy(fromkod,kodirovka_iceb.ravno(),sizeof(fromkod)-1);
strcpy(tokod,"KOI8-U");
strcpy(fallbak,"?");

gsize bytes_read;
gsize bytes_written;
if((tostr=g_convert_with_fallback(fromstr,-1,tokod,fromkod,fallbak,&bytes_read,&bytes_written,NULL)) == NULL)
 {
  printf("%s-Ошибка перекодировки в koi8u!\nСтрока=%s\n",__FUNCTION__,fromstr);
  return(fromstr);
 }
return(tostr);
}
