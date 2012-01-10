/* $Id: bsiz23pw.c,v 1.9 2011-02-21 07:35:51 sasa Exp $ */
/*16.12.2009    03.10.2004      Белых А.И.      bsiz23pw.c
Получение балансовой стоимсости и износа за период для 
групп у которых амортизация расчитывается в целом по группе
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <errno.h>
#include        "buhg_g.h"
#include <unistd.h>

extern short	startgoduos; /*Стартовый год*/
extern SQL_baza bd;

void bsiz23pw(char kodgr[],
short dn,short mn,short gn,
short dk,short mk,short gk,
class bsiz_data *data,
GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row;
SQLCURSOR curr;
char		strsql[512];
char            bros[512];
char            bros1[512];
short           d,m,g;
short		tp;

int		innom;
char		pdr[24];
double		pbs,piz;

data->nu.clear_data();
class iceb_u_str imaf("uosnast.alx");

sprintf(strsql,"Стартовая балансовая стоимость для группы %s",kodgr);
if(iceb_poldan(strsql,bros,imaf.ravno(),wpredok) != 0)
 {
  
  sprintf(bros1,"Не найдена %s\n%s",strsql,imaf.ravno());
  iceb_menu_soob(bros1,wpredok);
 }

data->nu.bs_nah_per+=iceb_u_atof(bros);
data->nu.bs_kon_per+=iceb_u_atof(bros);

sprintf(strsql,"Стартовый износ для группы %s",kodgr);
if(iceb_poldan(strsql,bros,imaf.ravno(),wpredok) != 0)
 {
  sprintf(bros1,"Не найден %s",strsql);
  iceb_menu_soob(bros1,wpredok);
 }

data->nu.izmen_iznosa_nah_per+=iceb_u_atof(bros);
data->nu.izmen_iznosa_kon_per+=iceb_u_atof(bros);

/*
printw("bsiz23--%s %d.%d.%d\n",kodgr,dp,mp,gp);
refresh();
*/

sprintf(strsql,"select datd,tipz,innom,podr,bs,iz,nomd,kodop from Uosdok1 where \
datd >= '%d-%d-%d' and datd <= '%d-%d-%d' and podt=1 \
order by datd asc, tipz desc",startgoduos,1,1,gk,mk,dk);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),wpredok);
  return;
 }




while(cur.read_cursor(&row) != 0)
 {
/*****
  printw("%s %s %s %s %s %s %s\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
  refresh();
**********/
  if(iceb_u_SRAV(gettext("ОСТ"),row[7],0) == 0)
    continue;
      
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  tp=atoi(row[1]);
  innom=atol(row[2]);
  strncpy(pdr,row[3],sizeof(pdr)-1);
  pbs=atof(row[4]);
  piz=atof(row[5]);
  
  class poiinpdw_data rekin;
  /*Читаем переменные данные на конец периода так как мог быть приход в периоде*/
  poiinpdw(innom,mk,gk,&rekin,wpredok);

  if(iceb_u_SRAV(kodgr,rekin.hna.ravno(),0) != 0)
   continue;
  
  if(tp == 2)
   {
    pbs*=(-1.);
    piz*=(-1.);
   }
  
  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0) //до начала периода
   {
    data->nu.bs_nah_per+=pbs;
    data->nu.izmen_iznosa_nah_per+=piz;
   }
  else
   {
    if(tp == 2)
     {
      data->nu.bs_rash_za_per+=pbs;
      data->nu.iznos_rash_za_per+=piz;
     }
    else
     {
      if(startgoduos == g && iceb_u_SRAV(row[7],gettext("ОСТ"),0) == 0)
       {
        //это стартовый приход
       }
      else
       {
        data->nu.bs_prih_za_per+=pbs;
        data->nu.iznos_prih_za_per+=piz;
       }
     }
   }
  data->nu.bs_kon_per+=pbs;
  data->nu.izmen_iznosa_kon_per+=piz;

 }


/*
printw("bs23=%.2f iz-%.2f\n",*bs,*iz);
refresh();
OSTANOV();
*/

/*Узнаем амортизацию*/
if(startgoduos != 0 && startgoduos != gk)
  sprintf(strsql,"select suma,mes,god,innom from Uosamor where \
((god = %d and mes >= %d) or \
(god > %d and god < %d) or \
(god = %d and mes <= %d)) and \
innom=%d and suma <> 0. order by god,mes asc",
startgoduos,1,startgoduos,gk,gk,mk,atoi(kodgr)*(-1));
else
  sprintf(strsql,"select suma,mes,god,innom from Uosamor where \
god = %d and mes >= %d and  mes <= %d and \
innom=%d and suma <> 0. order by god,mes asc",gk,1,mk,atoi(kodgr)*(-1));


//printw("\n%s\n",strsql);
//refresh();


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  m=atoi(row[1]);
  g=atoi(row[2]);
  piz=atof(row[0]);  
  if(iceb_u_sravmydat(1,m,g,1,mn,gn) < 0)
    data->nu.amort_nah_per+=piz;

  data->nu.amort_kon_per+=piz;

 }

data->nu.bs_raznica=data->nu.bs_kon_per-data->nu.bs_nah_per;
data->nu.izmen_iznosa_raznica=data->nu.izmen_iznosa_kon_per-data->nu.izmen_iznosa_nah_per;
data->nu.amort_raznica=data->nu.amort_kon_per-data->nu.amort_nah_per;

/******************
printw("Код группы налогового учёта %s\n\
%10.2f %10.2f %10.2f\n\
%10.2f %10.2f %10.2f\n\
%10.2f %10.2f %10.2f\n",
kodgr,
data->nu.bs_nah_per,data->nu.izmen_iznosa_nah_per,data->nu.amort_nah_per,
data->nu.bs_kon_per,data->nu.izmen_iznosa_kon_per,data->nu.amort_kon_per,
data->nu.bs_raznica,data->nu.izmen_iznosa_raznica,data->nu.amort_raznica);
OSTANOV();
****************/

}
