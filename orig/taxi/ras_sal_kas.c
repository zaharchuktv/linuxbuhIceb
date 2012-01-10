/*$Id: ras_sal_kas.c,v 1.5 2011-02-21 07:36:21 sasa Exp $*/
/*06.05.2007	04.01.2006	Белых А.И.	ras_sal_kas.c
Расчёт сальдо по кассе
*/
#include <stdlib.h>
#include "i_rest.h"

extern short start_god_rest;
extern SQL_baza bd;

double ras_sal_kas(int kod_kas,GtkWidget *wpredok)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);
int kolstr=0;
char strsql[300];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select 	datz,suma,nomd from Restkas where nk=%d  and kodz=0 and datz >= '%04d-01-01'",
kod_kas,start_god_rest);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

short d,m,g;
double saldo=0.;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(start_god_rest < g && iceb_u_SRAV("000",row[2],0) == 0)
   continue;
  saldo+=atof(row[1]);
 }
return(saldo);
}
