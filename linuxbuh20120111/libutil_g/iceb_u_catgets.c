/*$Id: iceb_u_catgets.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*17.02.2009	16.09.2003	Белых А.И.	iceb_u_catgets.c
Получение сообщения с перекодировкой в UTF кодировку
*/
#include   "iceb_util.h"

char *iceb_u_catgets(nl_catd fils,int sec,int str,const char *stroka)
{

return(iceb_u_toutf(catgets(fils,sec,str,stroka)));

}
