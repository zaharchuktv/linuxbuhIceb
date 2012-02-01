/*$Id: rpshet_rsw.c,v 1.8 2011-02-21 07:35:57 sasa Exp $*/
/*11.11.2008	10.02.2006	Белых А.И.	rpshet_rsw.c
Расчёт сальдо по счёту с развернутым сальдо
*/
#include <stdlib.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

extern short	startgodb; /*Стартовый год*/
extern SQL_baza bd;

void rpshet_rsw(const char *shet,
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

SQLCURSOR cur;
SQL_str row;
char strsql[512];
int kolstr=0;
sprintf(strsql,"select kodkon from Skontr where ns='%s'",shet);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
class iceb_u_spisok sp_kontr;
while(cur.read_cursor(&row) != 0)
  sp_kontr.plus(row[0]);
  
class iceb_u_double deb_saldon;
class iceb_u_double kre_saldon;

class iceb_u_double deb_oborot;
class iceb_u_double kre_oborot;

deb_saldon.make_class(sp_kontr.kolih());
kre_saldon.make_class(sp_kontr.kolih());

deb_oborot.make_class(sp_kontr.kolih());
kre_oborot.make_class(sp_kontr.kolih());

/*Узнаем начальное сальдо по каждой организации*/
sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d and ns='%s'",godn,shet);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

int nomer_kontr=0;

while(cur.read_cursor(&row) != 0)
 {
  if((nomer_kontr=sp_kontr.find_r(row[0])) < 0)
   continue;  

  deb_saldon.plus(atof(row[1]),nomer_kontr);
  kre_saldon.plus(atof(row[2]),nomer_kontr);
  
 }


short d=1;
short m=1;
short g=godn;

while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {

  sprintf(strsql,"select kodkon,deb,kre from Prov \
where datp='%d-%02d-%02d' and sh='%s' and val=0",
  g,m,d,shet);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }
  while(cur.read_cursor(&row) != 0)
   {
//    printw("%s %s %s\n",row[0],row[1],row[2]);    
    if((nomer_kontr=sp_kontr.find_r(row[0])) < 0)
     continue;
    if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0)
     {
      deb_saldon.plus(atof(row[1]),nomer_kontr);
      kre_saldon.plus(atof(row[2]),nomer_kontr);
     }
    else
     {
      deb_oborot.plus(atof(row[1]),nomer_kontr);
      kre_oborot.plus(atof(row[2]),nomer_kontr);
     }
     
   }

  iceb_u_dpm(&d,&m,&g,1);
 }
//Сворачиваем начальное сальдо
double deb=0.;
double kre=0.;
for(int i=0 ; i < sp_kontr.kolih(); i++)
 {
  deb=deb_saldon.ravno(i);
  kre=kre_saldon.ravno(i);
  if(deb > kre)
   {
    deb-=kre;
    kre=0.;  
   }
  else
   {
    kre-=deb;
    deb=0.;
   }
  deb_saldon.new_plus(deb,i);
  kre_saldon.new_plus(kre,i);

  deb_kre[0]+=deb;
  deb_kre[1]+=kre; 

 }

//Вычисляем конечное сальдо
for(int i=0 ; i < sp_kontr.kolih(); i++)
 {
  deb=deb_oborot.ravno(i);
  kre=kre_oborot.ravno(i);

  deb_kre[2]+=deb;
  deb_kre[3]+=kre;
  
  deb+=deb_saldon.ravno(i);
  kre+=kre_saldon.ravno(i);
  //Сворачиваем конечное сальдо
  if(deb > kre)
   {
    deb-=kre;
    kre=0.;
   }
  else
   {
    kre-=deb;
    deb=0.;
   }
  deb_kre[4]+=deb;
  deb_kre[5]+=kre;
 }
}
