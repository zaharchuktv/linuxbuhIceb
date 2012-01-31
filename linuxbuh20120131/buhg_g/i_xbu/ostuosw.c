/* $Id: ostuosw.c,v 1.9 2011-02-21 07:35:55 sasa Exp $ */
/*09.12.2008    24.01.1994      Белых А.И.      ostuosw.c
Просматриваем карточку и определяем все приходы, расходы
и остаток за нужный период
		       С начала года         Период
		       включая период
		       кол.       сумма      кол.  сумма
Остаток начальный   0  000         000       000    000
Приход              1  000         000       000    000
Расход              2  000         000       000    000
Остаток             3  000         000       000    000
*/
#include <stdlib.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

extern short	startgoduos; /*Стартовый год*/
extern SQL_baza bd;

void  ostuosw(int in,short dn,short mn,short gn,
short dk,short mk,short gk,
int pod_mo, /*код подразделения или материально ответственного Проверяется если не равен нолю*/
int metka_pm, /*0- по подразделению 1-по материально ответственному*/
class ostuosw_data *uos_ost,
GtkWidget *wpredok)
{
short		tipz;
short		d,m,g;
short           prf;
double          cnn=0.,cnnby=0.;
short           dp,mp,gp;
long		kolstr;
SQL_str         row;
char		strsql[512];
double		bs=0.,bsby=0.;
int		kol;

/*
printw("ostuos in=%ld podd=%d %d.%d.%d %d.%d.%d metka=%d\n",
in,podd,dn,mn,gn,dk,mk,gk,metka);
*/

dp=dn;
mp=mn;
gp=gn;

iceb_u_dpm(&dp,&mp,&gp,2);

uos_ost->clear();

sprintf(strsql,"select datd,tipz,kol,bs,kodop,kodol,bsby,podr from Uosdok1 where \
innom=%d and datd >= '%d-%d-%d' and datd <= '%d-%d-%d' and \
podt=1",in,startgoduos,1,1,gk,mk,dk);

/*
if(in == 4)
 printw("%s\n",strsql);
*/
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;
   
cnn=cnnby=0.;
prf=0;

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
  if(pod_mo != 0)
   {
    if(metka_pm == 0) /*по подразделению*/
     if(atoi(row[7]) != pod_mo)
      continue;
    if(metka_pm == 1) /*по материально-ответственному*/
     if(atoi(row[5]) != pod_mo)
      continue;
   } 
  
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  tipz=atoi(row[1]);
  kol=atoi(row[2]);
  bs=atof(row[3]);
  bsby=atof(row[6]);
  
  if(tipz == 1 && iceb_u_SRAV(row[4],gettext("ОСТ"),0) == 0)
   {
    uos_ost->ostg[0]+=kol;
    uos_ost->ostgc[0]+=bs;
    uos_ost->ostgcby[0]+=bsby;
    continue;
   }

  uos_ost->ostg[tipz]+=kol;
  uos_ost->ostgc[tipz]+=bs;
  uos_ost->ostgcby[tipz]+=bsby;

    /*Период*/
/**********
  if(SRAV1(gn,mn,dn,g,m,d) >= 0)
   if(SRAV1(gk,mk,dk,g,m,d) <= 0)
***************/
  if(iceb_u_sravmydat(dn,mn,gn,d,m,g) <= 0)
   if(iceb_u_sravmydat(dk,mk,gk,d,m,g) >= 0)
     {
      if(tipz == 2)
	kol*=(-1);
      uos_ost->ostm[tipz]+=kol;
      uos_ost->ostmc[tipz]+=bs;
      uos_ost->ostmcby[tipz]+=bsby;
     }

    /*До периода*/
/*  if(SRAV1(gp,mp,dp,g,m,d) <= 0)*/
  if(iceb_u_sravmydat(dp,mp,gp,g,m,d) >= 0)
   {
    prf+=kol;
    if(tipz == 2)
     {
      bs*=(-1);
      bsby*=(-1);
     }
    cnn+=bs;
    cnnby+=bsby;
   }

 }

uos_ost->ostg[3]=uos_ost->ostg[0]+uos_ost->ostg[1]-uos_ost->ostg[2];
uos_ost->ostm[0]=uos_ost->ostg[0]+prf;
uos_ost->ostm[3]=uos_ost->ostm[0]+uos_ost->ostm[1]-uos_ost->ostm[2];

uos_ost->ostgc[3]=uos_ost->ostgc[0]+uos_ost->ostgc[1]-uos_ost->ostgc[2];
uos_ost->ostmc[0]=uos_ost->ostgc[0]+cnn;
uos_ost->ostmc[3]=uos_ost->ostmc[0]+uos_ost->ostmc[1]-uos_ost->ostmc[2];

uos_ost->ostgcby[3]=uos_ost->ostgcby[0]+uos_ost->ostgcby[1]-uos_ost->ostgcby[2];
uos_ost->ostmcby[0]=uos_ost->ostgcby[0]+cnnby;
uos_ost->ostmcby[3]=uos_ost->ostmcby[0]+uos_ost->ostmcby[1]-uos_ost->ostmcby[2];

/**************
if(in == 4)
 {
printw("\n\
uos_ost->ostm= %d %d %d %d\n\
uos_ost->ostg= %d %d %d %d\n\
uos_ost->ostmc= %.2f %.2f %.2f %.2f\n\
uos_ost->ostgc= %.2f %.2f %.2f %.2f\n",
uos_ost->ostm[0],uos_ost->ostm[1],uos_ost->ostm[2],uos_ost->ostm[3],
uos_ost->ostg[0],uos_ost->ostg[1],uos_ost->ostg[2],uos_ost->ostg[3],
uos_ost->ostmc[0],uos_ost->ostmc[1],uos_ost->ostmc[2],uos_ost->ostmc[3],
uos_ost->ostgc[0],uos_ost->ostgc[1],uos_ost->ostgc[2],uos_ost->ostgc[3]);
OSTANOV();
 }
*************/
}
