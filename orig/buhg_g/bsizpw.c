/*$Id: bsizpw.c,v 1.6 2011-02-21 07:35:51 sasa Exp $*/
/*06.07.2008	21.10.2005	Белых А.И.	bsizpw.c
Получение балансовой стоимости и износа за период
Если вернули 0-нашли запись
	     1-не нашли
*/
#include <stdlib.h>
#include        "buhg_g.h"


extern short	startgoduos; /*Стартовый год*/
extern SQL_baza bd;

int bsizpw(int in, //Инвентарный номер
int ceh, //Цех/участок
short dn,short mn,short gn,
short dk,short mk,short gk,
class bsiz_data *data,
GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row;
class SQLCURSOR curr;
char		strsql[512];
double		bss,izz,bssby,izzby;
short		d,m,g;
short		tip;
char		startos[32];

data->clear_data();

sprintf(strsql,"select datd,tipz,bs,iz,kodop,bsby,izby from Uosdok1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and innom=%d \
and podr=%d and podt=1 order by datd asc",startgoduos,1,1,gk,mk,dk,in,ceh);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }



if(kolstr == 0)
  return(1);

strncpy(startos,gettext("ОСТ"),sizeof(startos)-1);
while(cur.read_cursor(&row) != 0)
 {
//  if(in == 2)
//   printw("%s %s %s %s %s %s %s\n",row[0],row[0],row[1],row[2],row[3],row[4],row[5]);
  tip=atoi(row[1]);
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  bss=atof(row[2]);
  izz=atof(row[3]);  
  bssby=atof(row[5]);
  izzby=atof(row[6]);  

  if(tip == 2)
   {
    bss*=(-1.);
    izz*=(-1.);
    bssby*=(-1.);
    izzby*=(-1.);
   }

  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0) //до начала периода
   {
    data->nu.bs_nah_per+=bss;
    data->nu.izmen_iznosa_nah_per+=izz;

    data->bu.bs_nah_per+=bssby;
    data->bu.izmen_iznosa_nah_per+=izzby;
   }
  else
   {
    if(tip == 2)
     {
      data->nu.bs_rash_za_per+=bss;
      data->bu.bs_rash_za_per+=bssby;
      data->nu.iznos_rash_za_per+=izz;
      data->bu.iznos_rash_za_per+=izzby;
     }
    else
     {
      if(startgoduos == g && iceb_u_SRAV(row[4],startos,0) == 0)
       {
        //это стартовый приход
       }
      else
       {
        data->nu.bs_prih_za_per+=bss;
        data->bu.bs_prih_za_per+=bssby;
        data->nu.iznos_prih_za_per+=izz;
        data->bu.iznos_prih_za_per+=izzby;
       }
     }
   } 
  data->nu.bs_kon_per+=bss;
  data->nu.izmen_iznosa_kon_per+=izz;

  data->bu.bs_kon_per+=bssby;
  data->bu.izmen_iznosa_kon_per+=izzby;
 }
//  if(in == 2)
//    OSTANOV();
/*Узнаем амортизацию для налогового учета*/
if(startgoduos != gn)
  sprintf(strsql,"select god,mes,suma from Uosamor where \
((god = %d and mes >= %d) or \
(god > %d and god < %d) or \
(god = %d and mes <= %d)) and \
innom=%d and podr=%d and suma <> 0. order by god,mes asc",
startgoduos,1,startgoduos,gk,gk,mk,in,ceh);
else
  sprintf(strsql,"select god,mes,suma from Uosamor where \
god = %d and mes >= %d and  mes <= %d and \
innom=%d and podr=%d and suma <> 0. order by god,mes asc",
startgoduos,1,mk,in,ceh);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 }
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s\n",row[0],row[1],row[2]);
  m=atoi(row[1]);
  g=atoi(row[0]);

  bss=atof(row[2]);
  if(iceb_u_sravmydat(1,m,g,1,mn,gn) < 0)
    data->nu.amort_nah_per+=bss;

  data->nu.amort_kon_per+=bss;

 }
//printw("%d.%d %f-%f\n",mn,gn,data->amort_nah_per,data->amort_kon_per);


/*Узнаем амортизацию для бухгалтерского учета*/
if(startgoduos != gn)
  sprintf(strsql,"select god,mes,suma from Uosamor1 where \
((god = %d and mes >= %d) or \
(god > %d and god < %d) or \
(god = %d and mes <= %d)) and \
innom=%d and podr=%d and suma <> 0. order by god,mes asc",
startgoduos,1,startgoduos,gk,gk,mk,in,ceh);
else
  sprintf(strsql,"select god,mes,suma from Uosamor1 where \
god = %d and mes >= %d and  mes <= %d and \
innom=%d and podr=%d and suma <> 0. order by god,mes asc",
startgoduos,1,mk,in,ceh);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 }

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {

  m=atoi(row[1]);
  g=atoi(row[0]);

  bss=atof(row[2]);
  if(iceb_u_sravmydat(1,m,g,1,mn,gn) < 0)
    data->bu.amort_nah_per+=bss;

  data->bu.amort_kon_per+=bss;

 }

data->nu.bs_raznica=data->nu.bs_kon_per-data->nu.bs_nah_per;
data->nu.izmen_iznosa_raznica=data->nu.izmen_iznosa_kon_per-data->nu.izmen_iznosa_nah_per;
data->nu.amort_raznica=data->nu.amort_kon_per-data->nu.amort_nah_per;

data->bu.bs_raznica=data->bu.bs_kon_per-data->bu.bs_nah_per;
data->bu.izmen_iznosa_raznica=data->bu.izmen_iznosa_kon_per-data->bu.izmen_iznosa_nah_per;
data->bu.amort_raznica=data->bu.amort_kon_per-data->bu.amort_nah_per;
/***************
printw("Инвен.ном=%d Подр=%d\n\
bs_nah_per=%.2f\n\
izmen_iznosa_nah_per=%.2f\n\
amort_nah_per=%.2f\n\
bs_kon_per=%.2f\n\
izmen_iznosa_kon_per=%.2f\n\
amort_kon_per=%.2f\n\
bs_prih_za_per=%.2f\n\
bs_rash_za_per=%.2f\n\
iznos_prih_za_per=%.2f\n\
iznos_rash_za_per=%.2f\n\
bs_raznica=%.2f\n\
izmen_iznosa_raznica=%.2f\n\
amort_raznica=%.2f\n",
in,ceh,
data->bu.bs_nah_per,
data->bu.izmen_iznosa_nah_per,
data->bu.amort_nah_per,
data->bu.bs_kon_per,
data->bu.izmen_iznosa_kon_per,
data->bu.amort_kon_per,
data->bu.bs_prih_za_per,
data->bu.bs_rash_za_per,
data->bu.iznos_prih_za_per,
data->bu.iznos_rash_za_per,
data->bu.bs_raznica,
data->bu.izmen_iznosa_raznica,
data->bu.amort_raznica);
OSTANOV();
********************/
return(0);

}
