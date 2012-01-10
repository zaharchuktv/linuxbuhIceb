/*$Id: ras_sal_kl_t.c,v 1.3 2011-02-21 07:36:21 sasa Exp $*/
/*03.02.2006	20.12.2004	Белых А.И.	ras_sal_kl_t.c
Расчёт сальдо по клиету с погашением оплаченных документов для работы без графической библиотеки
Возвращает сальдо по клиенту
*/
#include <stdlib.h>
#include "i_kontr_vv.h"

extern short start_god_rest;
extern SQL_baza bd;

double ras_sal_kl_t(char *kodkl,
double *ost_na_saldo) //Сальдо перед первым неоплаченным документом
{
char strsql[500];


*ost_na_saldo=0.;
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select datz,nomd,suma from Restkas where datz >= '%04d-01-01' \
and kodkl='%s'",start_god_rest,kodkl);
int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  t_msql_error(&bd,strsql);
  exit(1);
 }
short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(start_god_rest < g && iceb_u_SRAV("000",row[1],0) == 0)
   continue;
  *ost_na_saldo+=atof(row[2]);
 }


return(otm_opl_doks_t(kodkl,ost_na_saldo));
}
