/* $Id: provkolw.c,v 1.7 2011-02-21 07:35:56 sasa Exp $ */
/*11.11.2008	20.06.2000	Белых А.И.	provkolw.c
Поверка количества материалла во встречном документе
*/
#include        <stdlib.h>
#include        "buhg_g.h"

extern SQL_baza bd;

double provkolw(short dd,short md,short gd,int skl,const char *nomd,GtkWidget *wpredok)
{
char		strsql[512];
long		kolstr;
SQL_str         row;
double		kolih;

sprintf(strsql,"select kolih from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s'",
gd,md,dd,skl,nomd);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr == 0)
 {
  return(0.);
 }
kolih=0.;
while(cur.read_cursor(&row) != 0)
 {
  kolih+=atof(row[0]);
 }
return(kolih);

}
