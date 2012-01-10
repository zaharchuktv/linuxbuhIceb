/*$Id: iceb_eropbaz.c,v 1.9 2011-02-21 07:36:06 sasa Exp $*/
/*11.11.2008	22.12.2001	Белых А.И.	iceb_eropbaz.c
Выдача сообщения на экран об ошибке открытия базы данных
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include <gtk/gtk.h>
#include        "iceb_libbuh.h"

extern SQL_baza	bd;

void  iceb_eropbaz(const char *imabaz,int kto,const char *name,
int vih) //0-завершение работы прогаммы 1-нет
{
char bros[512];
char bros1[200];

sprintf(bros,"%s:%d %s",gettext("Логин"),kto,name);
sprintf(bros1,"%s %s:",gettext("Ошибка открытия базы данных"),imabaz);

iceb_msql_error(&bd,bros1,bros,NULL);

if(vih == 0)
 {
  exit(1);
 }
}
