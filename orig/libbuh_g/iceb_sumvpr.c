/*$Id: iceb_sumvpr.c,v 1.8 2011-02-21 07:36:08 sasa Exp $*/
/*24.11.2008	32.09.1998	Белых А.И.	iceb_sumvpr.c
Узнать сумму уже выполненых проводок
*/
#include        <stdlib.h>
#include        "iceb_libbuh.h"

extern SQL_baza	bd;

double iceb_sumvpr(const char *sh,const char *shk,
short d,short m,short g,const char *nn,int skk,const char *kom,time_t vrem,
int metkaprov,const char *kto)
{
double		suma=0.;
char		strsql[500];
SQL_str         row;
long		kolstr;

sprintf(strsql,"select deb from Prov where val=%d and kto='%s' and \
pod=%d and nomd='%s' and datd='%d-%02d-%02d' and sh='%s' and shk='%s' \
and vrem <> %ld and komen='%s' and deb <> 0.",metkaprov,
kto,
skk,nn,g,m,d,sh,shk,vrem,kom);

SQLCURSOR cur;  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,"iceb_sumvpr",strsql,NULL);

if(kolstr == 0)
  return(0.);

while(cur.read_cursor(&row) != 0)
 {
/*
  if(fabs(atof(row[1])) > 0.009)
     continue;
*/
  suma+=atof(row[0]);

 }
/*
printw("suma=%f\n",suma);
OSTANOV();
*/
suma=iceb_u_okrug(suma,0.01);
return(suma);

}
