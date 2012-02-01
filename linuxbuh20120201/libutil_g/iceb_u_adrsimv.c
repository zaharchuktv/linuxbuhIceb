/*$Id: iceb_u_adrsimv.c,v 1.2 2011-01-13 13:50:05 sasa Exp $*/
/*30.09.2009	30.09.2009	Белых А.И.	iceb_u_adrsimv.c
Возвращает адрес символа 
*/
#include <glib.h>

const char *iceb_u_adrsimv(int nomer_simv,const char *str)
{
if(g_utf8_validate(str,-1,NULL) == TRUE) /*текст в utf8 кодировке*/
 {
  return(g_utf8_offset_to_pointer(str,nomer_simv));
 }

return(&str[nomer_simv]);

}