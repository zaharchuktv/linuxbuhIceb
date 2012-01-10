/* $Id: puzapw.c,v 1.7 2011-02-21 07:35:56 sasa Exp $ */
/*14.12.2007    05.07.1996      Белых А.И.      puzapw.c
Проверка возможности удаления записи в документе
или всего документа
Если вернули 0-можно удалять
	     1-нельзя
Если после документа есть записи значит документ удалять нельзя разрывается цепочка документов
*/
#include        "buhg_g.h"

extern SQL_baza bd;

int puzapw(int in,int podd,short d,short m,short g,const char *ndk,GtkWidget *wpredok)
{
int             i,voz;
long		kolstr;
SQL_str         row;
char		strsql[512];


sprintf(strsql,"select nomd from Uosdok1 where datd >= '%04d-%02d-%02d' and podr=%d and innom=%d order by datd asc, tipz desc",
g,m,d,podd,in);


class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
  return(0);

voz=i=0;
while(cur.read_cursor(&row) != 0)
 {
  if(i == 1)
   {
    voz++;
    break;
   }   

  if(iceb_u_SRAV(ndk,row[0],0) == 0)
   i=1;

 }

return(voz);
}
