/*$Id: rpshet_ssw.c,v 1.8 2011-02-21 07:35:57 sasa Exp $*/
/*12.09.2009	10.02.2006	Белых А.И.	rpshet_ssw.c
Расчёт по счёту со свёрнутым сальдо
*/
#include <stdlib.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

extern short	startgodb; /*Стартовый год*/
extern SQL_baza bd;

void rpshet_ssw(const char *shet,
short dn,short mn,short gn,
short dk,short mk,short gk,
double *deb_kre, //Массив [6]
GtkWidget *wpredok)
{
for(int i=0; i < 6; i++)
  deb_kre[i]=0;

short godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

/*Узнаем начальное сальдо по счету*/
char strsql[512];
sprintf(strsql,"select ns,deb,kre from Saldo where kkk='0' and gs=%d and \
ns='%s'",godn,shet);

SQL_str row;
int kolstr=0;
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  deb_kre[0]+=atof(row[1]);
  deb_kre[1]+=atof(row[2]);
 }

short d=1;
short m=1;
short g=godn;
double  debetsh=0.,kreditsh=0.;

while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  debetsh=kreditsh=0.;

  sprintf(strsql,"select deb,kre from Prov \
  where val=0 and datp='%d-%02d-%02d' and sh='%s'",g,m,d,shet);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }

  while(cur.read_cursor(&row) != 0)
   {
    debetsh+=atof(row[0]);
    kreditsh+=atof(row[1]);
   }


  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0)
   {
    deb_kre[0]+=debetsh;
    deb_kre[1]+=kreditsh;
   }
  else
   {
    deb_kre[2]+=debetsh;
    deb_kre[3]+=kreditsh;
   }
  iceb_u_dpm(&d,&m,&g,1);
 }

//Сворачиваем начальное сальдо
if(deb_kre[0] > deb_kre[1])
 {
  deb_kre[0]-=deb_kre[1];
  deb_kre[1]=0.;
 }
else
 {
  deb_kre[1]-=deb_kre[0];
  deb_kre[0]=0.;
 }

//Вычисляем конечное сальдо развёрнутое
deb_kre[4]+=deb_kre[0]+deb_kre[2];
deb_kre[5]+=deb_kre[1]+deb_kre[3];

//Сворачиваем конечное сальдо
if(deb_kre[4] > deb_kre[5])
 {
  deb_kre[4]-=deb_kre[5];
  deb_kre[5]=0.;
 }
else
 {
  deb_kre[5]-=deb_kre[4];
  deb_kre[4]=0.;
 }

}

