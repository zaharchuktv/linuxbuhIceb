/*$Id: provpstw.c,v 1.6 2011-02-21 07:35:56 sasa Exp $*/
/*17.03.2008	17.03.2008	Белых А.И.	provpstw.c
Проверка полноты списания топлива в путевом листе
*/
#include <math.h>
#include <stdlib.h>
#include "buhg_g.h"

extern SQL_baza bd;

void provpstw(short dd,short md,short gd, //Дата
const char *nomd, //Номер документа
int podr, //Подразделение
GtkWidget *wpredok)
{
char	strsql[512];
int	kolstr=0;
SQL_str row;
int nomer=0;
int kolmt=0;
int metkasp=1; //0-не полнстью списано 1- полностью

//printw("\nprovpst-%d.%d.%d %s %s\n",dd,md,gd,nomd,podr);

class iceb_u_spisok KST; //Коды списанного в путевом листе топлива
class iceb_u_double KOL;  //Количество списанного топлива

spst(dd,md,gd,nomd,podr,&KST,&KOL,wpredok);

kolmt=KST.kolih();
if(kolmt > 0)
 {
  SQLCURSOR cur;
  for(int ii=0; ii < kolmt; ii++)
   {
    double kolsp=0.;
    
    sprintf(strsql,"select kt,zt,ztzg,ztgr,ztmh from Upldok2 where datd='%d-%d-%d' and \
  kp=%d and nomd='%s' and kt='%s'",
    gd,md,dd,podr,nomd,KST.ravno(ii));
    
    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
      continue;
     }
    class iceb_u_spisok repl;

    while(cur.read_cursor(&row) != 0)
     {
      kolsp+=atof(row[1])+atof(row[2])+atof(row[3])+atof(row[4]);
     }

   int metkanr=0;
   if(fabs(kolsp - KOL.ravno(ii)) > 0.0009)
    {
      metkanr++;
      metkasp=0;
      if(metkanr == 1)
         repl.plus(gettext("Не полностью списано топливо по объектам списания !"));
             
       sprintf(strsql,"%s %.3f != %.3f",
       KST.ravno(nomer),
       kolsp,
       KOL.ravno(ii));
       
       repl.plus(strsql);
       iceb_menu_soob(&repl,wpredok);
     }
   }
 }
else
  metkasp=0;

sprintf(strsql,"update Upldok set mt=%d where datd='%d-%d-%d' and kp=%d and nomd='%s'",
metkasp,gd,md,dd,podr,nomd);
iceb_sql_zapis(strsql,1,1,wpredok);

}
