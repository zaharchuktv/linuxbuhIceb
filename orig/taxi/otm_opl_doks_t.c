/*$Id: otm_opl_doks_t.c,v 1.5 2011-02-21 07:36:21 sasa Exp $*/
/*03.02.2006	20.12.2004	Белых А.И.	otm_opl_doks_t.c
Отметка оплаченных документов
Возвращает сальдо
*/
#include "i_kontr_vv.h"

extern int  nomer_kas;  //номер кассы
extern SQL_baza bd;
extern uid_t    kod_operatora;

double otm_opl_doks_t(const char *kodkl,double *ost_na_saldo)
{

char strsql[500];
SQL_str row;
SQLCURSOR cur;
int kolstr;

sprintf(strsql,"select nomd,datd,podr from Restdok where kodkl='%s' and mo=0",
kodkl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  t_msql_error(&bd,strsql);
  return(*ost_na_saldo);
 }

short d,m,g;
double suma_dok;
/*
short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);
iceb_u_str vrem_tek;
vrem_tek.plus(iceb_u_vrem_tek());
*/
double saldo=*ost_na_saldo;
while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[1],2);

  suma_dok=sumapsh_t(g,row[0]);

  saldo-=suma_dok;

  if(*ost_na_saldo < suma_dok)
   continue;
  
  if(otm_opl_dok_t(g,row[0],suma_dok) != 0)
   continue;

  *ost_na_saldo-=suma_dok;
      
 }
return(saldo); 
}

