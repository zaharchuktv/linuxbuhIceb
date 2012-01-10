/* $Id: pudokw.c,v 1.9 2011-02-21 07:35:56 sasa Exp $ */
/*23.04.2008    05.07.1996      Белых А.И.      pudokw.c
Проверка удаления всего документа
Если вернули 0-можно удалять
	     1-нельзя
*/
#include <stdlib.h>
#include        "buhg_g.h"

extern SQL_baza bd;

int pudokw(int podd,short d,short m,short g,const char *ndk,short tz,GtkWidget *wpredok)
{
short           voz;
int            in;
long		kolstr;
SQL_str         row;
char		strsql[512];


/*Смотрим заблокированы ли проводки к этому документа*/
if(iceb_pbp(3,d,m,g,ndk,podd,tz,gettext("Удалить документ невозможно!"),wpredok) != 0)
 return(1);

sprintf(strsql,"select innom from Uosdok1 where datd = '%04d-%02d-%02d' \
and podr=%d and nomd='%s' and tipz=%d",
g,m,d,podd,ndk,tz);



class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  return(0);
 }
voz=0;
while(cur.read_cursor(&row) != 0)
 {
  in=atoi(row[0]);
  if(puzapw(in,podd,d,m,g,ndk,wpredok) != 0)
   {
    voz++;
    break;
   }
 }

return(voz);
}
