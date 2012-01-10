/* $Id: readkolkw.c,v 1.8 2011-02-21 07:35:57 sasa Exp $ */
/*11.11.2008	28.09.1997	Белых А.И.	readkolkw.c
Получение количества реально выданного по документа
*/
#include        <stdlib.h>
#include	"buhg_g.h"

extern SQL_baza	bd;

double readkolkw(int skl,int nk,short d,short m,short g,const char *nn,GtkWidget *wpredok)
{
double		kol;
SQL_str         row;
char		strsql[512];
long		kolstr;

/*Читаем в карточке количество реально выданное-полученное*/
sprintf(strsql,"select kolih from Zkart where datd='%d-%02d-%02d' and \
nomd='%s' and sklad=%d and nomk=%d",
g,m,d,nn,skl,nk);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr == 0)
  return(0.);
 
kol=0.;
while(cur.read_cursor(&row) != 0)
 {
   kol+=atof(row[0]);
 } 

return(kol);

}
