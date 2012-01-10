/*$Id: upl_ptw.c,v 1.5 2011-02-21 07:35:58 sasa Exp $*/
/*31.03.2008	02.05.2005	Белых А.И.	upl_ptw.c
Подпрограмма определения количества переданного топлива другим водителям
*/
#include <stdlib.h>
#include "buhg_g.h"

extern SQL_baza bd;

double upl_ptw(short dn,short mn,short gn,
short dk,short mk,short gk,
char *kod_avt,
char *kod_vod,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
double suma=0.;
int kolstr;

//Смотрим передачи топлива другим водителям
sprintf(strsql,"select kolih,nomd from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' and kap=%s and kodsp=%s and tz=1",
gn,mn,dn,gk,mk,dk,kod_avt,kod_vod);

//printw("\nupl_pt-%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  suma+=atof(row[0]); 
 }
if(suma != 0.)
 suma=iceb_u_okrug(suma,0.001);
return(suma);
}
