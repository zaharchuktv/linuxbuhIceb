/*$Id: sumapsh_t.c,v 1.4 2011-02-21 07:36:21 sasa Exp $*/
/*12.01.2006	05.09.2004	Белых А.И.	sumapsh_t.c
Получение суммы по счёту (Терминальная версия)
*/
#include <stdlib.h>
#include "i_kontr_vv.h"


extern SQL_baza	bd;



double  sumapsh_t(short gd,const char *nomd)
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
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
 }      

sprintf(strsql,"select skl,kolih,cena,ps from Restdok1 where datd='%d-%d-%d' \
and nomd='%s'",g,m,d,nomd);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  t_msql_error(&bd,strsql);
  return(0.);
 }
double suma=0.;
double cena;
double cena_sk;
double suma_skidki=0.;
double kolih;
while(cur.read_cursor(&row) != 0)
 {
  cena=atof(row[2]);
  kolih=atof(row[1]);
  procent_sk=atof(row[3]);

  cena_sk=cena-cena*procent_sk/100.;
  cena_sk=iceb_u_okrug(cena_sk,0.01);

  suma_skidki+=kolih*(cena-cena_sk);

  suma+=kolih*cena_sk;
 }
 

suma_skidki=iceb_u_okrug(suma_skidki,0.01);

return(suma); 
}


