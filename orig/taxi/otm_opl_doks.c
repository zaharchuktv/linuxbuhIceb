/*$Id: otm_opl_doks.c,v 1.8 2011-02-21 07:36:21 sasa Exp $*/
/*06.11.2007	20.12.2004	Белых А.И.	otm_opl_doks.c
Отметка оплаченных документов
Возвращает сальдо
*/
#include "i_rest.h"

extern int  nomer_kas;  //номер кассы
extern SQL_baza bd;
extern uid_t    kod_operatora;

double otm_opl_doks(const char *kodkl,double *ost_na_saldo,
double *suma_po_dokum,
GtkWidget *wpredok)
{
*suma_po_dokum=0.;
char strsql[500];
SQL_str row;
SQLCURSOR cur;
int kolstr;

sprintf(strsql,"select nomd,datd,podr from Restdok where kodkl='%s' and mo=0",
kodkl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(*ost_na_saldo);
 }

short d,m,g;
double suma_dok;

double saldo=*ost_na_saldo;
while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[1],2);

  suma_dok=sumapsh(g,row[0],NULL,wpredok);

  saldo-=suma_dok;
  *suma_po_dokum+=suma_dok;      

  if(*ost_na_saldo < suma_dok)
   continue;
  
  if(otm_opl_dok(g,row[0],suma_dok,wpredok) != 0)
   continue;

  *ost_na_saldo-=suma_dok;
 }
return(saldo); 
}
