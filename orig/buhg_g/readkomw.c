/* $Id: readkomw.c,v 1.10 2011-06-07 08:52:27 sasa Exp $ */
/*11.05.2006	12.05.2001	Белых А.И.	readkomw.c
Чтение комментария к платежным документам
*/
#include        "buhg_g.h"

extern SQL_baza bd;

void	readkomw(const char *tabl,short dd,short md,short gd,const char *nomdk,
class iceb_u_str *koment,GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row;
char		strsql[1024];
int i1;


koment->new_plus("");

if(iceb_u_SRAV(tabl,"Pltp",0) == 0)
  sprintf(strsql,"select zapis from Pltpz where datd='%04d-%02d-%02d' and \
nomd='%s' and mz=0 order by nz asc",gd,md,dd,nomdk);
if(iceb_u_SRAV(tabl,"Pltt",0) == 0)
  sprintf(strsql,"select zapis from Plttz where datd='%04d-%02d-%02d' and \
nomd='%s' and mz=0 order by nz asc",gd,md,dd,nomdk);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

memset(koment,'\0',sizeof(koment));
memset(strsql,'\0',sizeof(strsql));

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  i1 = strlen(row[0]);
  strncpy(strsql,row[0],sizeof(strsql)-1);
  if(strsql[i1-1] == '\n')
     strsql[i1-1]='\0';
  if(koment->getdlinna() <= 1)
   koment->new_plus(strsql);
  else
   koment->plus(strsql);

  koment->plus(" ");

 }

if(iceb_u_strlen(koment->ravno()) > 159)
 {  
  sprintf(strsql,gettext("Длинна комментария больше чем можно передать на %d символов"),
  iceb_u_strlen(koment->ravno())-159);
  iceb_menu_soob(strsql,wpredok);
 }

if(iceb_u_strlen(koment->ravno()) <= 3)
 {
  iceb_menu_soob(gettext("Длинна комментария меньше 3-х символов !"),wpredok);
 }
}


