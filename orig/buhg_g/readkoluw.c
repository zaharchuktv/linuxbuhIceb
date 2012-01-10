/* $Id: readkoluw.c,v 1.8 2011-02-21 07:35:57 sasa Exp $ */
/*15.08.2005	28.09.1997	Белых А.И.	readkoluw.c
Получение количества подтвержденного по документа для услуг
*/
#include <stdlib.h>
#include	"buhg_g.h"

extern SQL_baza	bd;

double readkoluw(short d,short m,short g,int podr,const char *nomd,
int tipz,int metka,int kodzap,int nomzap,GtkWidget *wpredok)
{
double		kol;
SQL_str         row;
char		strsql[512];
long		kolstr;

/*Читаем количество реально выданное-полученное*/
sprintf(strsql,"select kolih from Usldokum2 where datd='%04d-%02d-%02d' and \
nomd='%s' and podr=%d and tp=%d and metka=%d and kodzap=%d and nz=%d",
g,m,d,nomd,podr,tipz,metka,kodzap,nomzap);


SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr == 0)
  return(0.);
 
kol=0.;
while(cur.read_cursor(&row) != 0)
   kol+=atof(row[0]);

return(kol);

}
