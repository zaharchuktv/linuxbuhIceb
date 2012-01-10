/*$Id: ras_sal_kl.c,v 1.11 2011-02-21 07:36:21 sasa Exp $*/
/*14.11.2008	20.12.2004	Белых А.И.	ras_sal_kl.c
Расчёт сальдо по клиету с погашением оплаченных документов
Возвращает сальдо по клиенту
*/
#include <stdlib.h>
#include "i_rest.h"

extern short start_god_rest;
extern SQL_baza bd;

double ras_sal_kl(const char *kodkl,
double *ost_na_saldo, //Сальдо перед первым неоплаченным документом
double *saldo_po_kase, //Сальдо по кассе (по наличным деньгам)
GtkWidget *wpredok)
{
char strsql[500];


*ost_na_saldo=0.;
SQL_str row;
SQLCURSOR cur;
*saldo_po_kase=0.;

sprintf(strsql,"select datz,nomd,suma,kodz from Restkas where datz >= '%04d-01-01' \
and kodkl='%s'",start_god_rest,kodkl);
int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  exit(1);
 }
short d,m,g;
double suma=0.;
int kodz=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(start_god_rest < g && iceb_u_SRAV("000",row[1],0) == 0)
   continue;
  suma=atof(row[2]);
  kodz=atoi(row[3]);

  if(kodz == 0 || kodz == 2)
   *saldo_po_kase+=suma;
     
  *ost_na_saldo+=suma;
 }

double suma_po_dok=0.;
double saldo=otm_opl_doks(kodkl,ost_na_saldo,&suma_po_dok,wpredok);

*saldo_po_kase-=suma_po_dok;
if(*saldo_po_kase < 0.) //Вся наличка списана
 *saldo_po_kase=0.;
return(saldo);

}
