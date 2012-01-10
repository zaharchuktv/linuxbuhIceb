/*$Id: sumapsh.c,v 1.9 2011-02-21 07:36:21 sasa Exp $*/
/*04.01.2007	05.09.2004	Белых А.И.	sumapsh.c
Получение суммы по счёту
*/
#include <stdlib.h>
#include "i_rest.h"


extern SQL_baza	bd;



double  sumapsh(short gd,const char *nomd,
const char *sklad, //Склады, которые должны войти в отчет или ничего 
GtkWidget *wpredok)
{
char strsql[300];
SQLCURSOR cur;
int       kolstr;
SQL_str   row;
short d=0,m=0,g=0;

//читаем полную дату
sprintf(strsql,"select datd from Restdok where god=%d and nomd='%s'",
gd,nomd);
float procent_sk=0.;
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
 }      

sprintf(strsql,"select skl,kolih,cena,ps from Restdok1 where datd='%d-%d-%d' \
and nomd='%s'",g,m,d,nomd);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
double suma=0.;
double cena;
double cena_sk;
double suma_skidki=0.;
double kolih;
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(sklad,row[0],0,0) != 0)
   continue; 
  cena=atof(row[2]);
  kolih=atof(row[1]);
  procent_sk=atof(row[3]);

  cena_sk=cena-cena*procent_sk/100.;
  cena_sk=iceb_u_okrug(cena_sk,0.01);

  suma_skidki+=kolih*(cena-cena_sk);

  suma+=kolih*cena_sk;
 }
 

suma_skidki=iceb_u_okrug(suma_skidki,0.01);
suma=iceb_u_okrug(suma,0.01);
return(suma); 
}
