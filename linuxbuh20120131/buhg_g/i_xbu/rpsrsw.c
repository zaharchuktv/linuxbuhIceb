/*$Id: rpsrsw.c,v 1.7 2011-02-21 07:35:57 sasa Exp $*/
/*11.11.2008	10.08.2006	Белых А.И.	rpsrsw.c
Расчёт по счёту с развёрнутым сальдо
Стартовые и конечные сальд свёрнуты
*/
#include <stdlib.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

extern short	startgodb; /*Стартовый год*/
extern SQL_baza bd;

void rpsrsw(const char *shet,
short dn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_spisok *sp_kontr,
class iceb_u_spisok *sp_grup,
class iceb_u_double *deb_saldon,
class iceb_u_double *kre_saldon,
class iceb_u_double *deb_oborot,
class iceb_u_double *kre_oborot,
class iceb_u_double *deb_saldok,
class iceb_u_double *kre_saldok,
GtkWidget *wpredok)
{

short godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

class SQLCURSOR cur;
SQL_str row;
class SQLCURSOR cur1;
SQL_str row1;
char strsql[512];
int kolstr=0;
sprintf(strsql,"select kodkon from Skontr where ns='%s'",shet);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  sp_kontr->plus(row[0]);
  sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sp_grup->plus("");
   }  
  else
   sp_grup->plus(row1[0]);
 }

deb_saldon->make_class(sp_kontr->kolih());
kre_saldon->make_class(sp_kontr->kolih());

deb_oborot->make_class(sp_kontr->kolih());
kre_oborot->make_class(sp_kontr->kolih());

deb_saldok->make_class(sp_kontr->kolih());
kre_saldok->make_class(sp_kontr->kolih());

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
  if((nomer_kontr=sp_kontr->find_r(row[0])) < 0)
   continue;  

  deb_saldon->plus(atof(row[1]),nomer_kontr);
  kre_saldon->plus(atof(row[2]),nomer_kontr);
  
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
    if((nomer_kontr=sp_kontr->find_r(row[0])) < 0)
     continue;
    if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0)
     {
      deb_saldon->plus(atof(row[1]),nomer_kontr);
      kre_saldon->plus(atof(row[2]),nomer_kontr);
     }
    else
     {
      deb_oborot->plus(atof(row[1]),nomer_kontr);
      kre_oborot->plus(atof(row[2]),nomer_kontr);
     }
     
   }

  iceb_u_dpm(&d,&m,&g,1);
 }
//Сворачиваем начальное сальдо
double deb=0.;
double kre=0.;
for(int i=0 ; i < sp_kontr->kolih(); i++)
 {
  deb=deb_saldon->ravno(i);
  kre=kre_saldon->ravno(i);
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
  deb_saldon->new_plus(deb,i);
  kre_saldon->new_plus(kre,i);

 }

//Вычисляем конечное сальдо
for(int i=0 ; i < sp_kontr->kolih(); i++)
 {
  deb=deb_oborot->ravno(i);
  kre=kre_oborot->ravno(i);

  deb+=deb_saldon->ravno(i);
  kre+=kre_saldon->ravno(i);
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
  deb_saldok->plus(deb,i);
  kre_saldok->plus(kre,i);
 }

}
