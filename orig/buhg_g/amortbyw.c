/* $Id: amortbyw.c,v 1.11 2011-03-28 06:55:45 sasa Exp $ */
/*20.03.2011    06.12.2000      Белых А.И.      amortbyw.c
Расчет амортзационных отчислений
для заданного инвентарного номера за заданный месяц
Определить числиться ли инвентарны номер и в каком подразделении
он находиться на заданную дату. Если не числится расчет не проводим
Если числится смотрим настройку и делаем расчет
В месяце убытия амортизация считается в подразделении
где числился. В подразделении где прибыл амотизация считается со
следующего месяца
Если вернули 0 - расчитали
	     1 - нет
*/
#include <stdlib.h>
#include        <time.h>
#include        <pwd.h>
#include        <errno.h>
#include        "buhg_g.h"
#include <unistd.h>

extern float      nemi; /*Необлагаемый минимум*/

int amortbyw(long in,
short mr,short gr,
short mra, //0-расчет в потоке 1-нет
const char *grup_bu, //Группа бухгалтерского учёта
FILE *ff_prot,
GtkWidget *wpredok)
{
int             i;
double		sbsby,sizby;
double          bsby,izby,iz1by;
double		amort[4];
double		kof;
int		podr;
SQL_str         row;
SQLCURSOR curr;
char		strsql[512];
time_t		vrem;
int		kodotl;
class bsizw_data bal_st;
class poiinpdw_data rekin;

if(ff_prot != NULL)
 fprintf(ff_prot,"%s\n",__FUNCTION__);
int	vrabu=uosprovarw(grup_bu,1,wpredok); //Вариант расчета амотизации бухгалтерского учета
if(vrabu == -1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не определён вариант расчёта!\n");
  iceb_menu_soob("Не определён вариант расчёта!",wpredok);
  return(1);
 }

podr=0;


 
memset(&amort,'\0',sizeof(amort));
 

if(ff_prot != NULL)
  fprintf(ff_prot,"Расчёт амортизационных отчислений для бухгалтерского учёта\nin-%ld %d.%d\n",in,mr,gr);


if((i=poiinw(in,1,mr,gr,&podr,&kodotl,wpredok)) != 0)
 {
     
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %ld %s %d.%d ! %s %d\n",
  gettext("Инвентарный номер"),in,
  gettext("не числится по дате"),mr,gr,
  gettext("Подпрограмма вернула"),i);

  if(mra == 1)
   {
    sprintf(strsql,"%s %ld %s %d.%d ! %s %d",
    gettext("Инвентарный номер"),in,
    gettext("не числится по дате"),mr,gr,
    gettext("Подпрогамма вернула"),i);
    iceb_menu_soob(strsql,wpredok);   
   }

  return(1);
 }   


//if((i=poiinpdw(in,mr,gr,uos.shetu,uos.hzt,uos.hau,uos.hna,&uos.popkf,&uos.soso,uos.nomz,&m1,&g1,uos.hnaby,&uos.popkfby)) != 0)
if((i=poiinpdw(in,mr,gr,&rekin,wpredok)) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %ld %s %d.%d ! %s %d\n",
  gettext("Инвентарный номер"),in,
  gettext("не найдено настройки на дату"),mr,gr,
  gettext("Подпрограмма вернула"),i);

  if(mra == 1)
   {
    sprintf(strsql,"%s %ld %s %d.%d ! %s %d",
    gettext("Инвентарный номер"),in,
    gettext("не найдено настройки на дату"),mr,gr,
    gettext("Подпрограмма вернула"),i);
    iceb_menu_soob(strsql,wpredok);
   }

  return(1);
 }  

sbsby=sizby=bsby=izby=iz1by=0.;
//Должны знать начальную стоимость даже если не считается амортизация
//bsiz(in,podr,1,mr,gr,&sbs,&siz,&bs,&iz,&iz1,&sbsby,&sizby,&bsby,&izby,&iz1by,ff_prot);
bsizw(in,podr,1,1,gr+1,&bal_st,ff_prot,wpredok);


bsby=bal_st.sbsby+bal_st.bsby-uosgetlsw(in,1,mr,gr,1,wpredok);
izby=bal_st.sizby+bal_st.izby+bal_st.iz1by;

double balst=bsby;
if(rekin.soso == 1 ||  rekin.soso == 3)
 {

  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %ld %s.\n",
  gettext("Инвентарный номер"),in,
  gettext("не эксплуатируется"));

  if(mra == 1)
   {
    sprintf(strsql,"%s %ld %s.",
    gettext("Инвентарный номер"),in,
    gettext("не эксплуатируется"));
    iceb_menu_soob(strsql,wpredok);
   }
  goto vp;
 }



if(bsby <= izby)
 {

  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %ld %s. bsby=%.2f <= izby=%.2f\n",
  gettext("Инвентарный номер"),in,
  gettext("амортизирован полностью"),bsby,izby);

  if(mra == 1)
   {
    sprintf(strsql,"%s %ld %s. %.2f <= %.2f",
    gettext("Инвентарный номер"),in,
    gettext("амортизирован полностью"),bsby,izby);
    iceb_menu_soob(strsql,wpredok);
   }
  bsby=izby=0.;
 }

/*Читаем коэффициент*/
sprintf(strsql,"select kof from Uosgrup1 where kod='%s'",rekin.hnaby.ravno());
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {

  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %ld - %s %s !\n",
  gettext("Инвентарный номер"),in,
  gettext("не нашли кода группы"),rekin.hnaby.ravno());

  if(mra == 1)
   { 
    sprintf(strsql,"%s %ld - %s %s !\n",
    gettext("Инвентарный номер"),in,
    gettext("не нашли кода группы"),rekin.hnaby.ravno());
    iceb_menu_soob(strsql,wpredok);
   }

  return(1);
 }

kof=atof(row[0]);
/*
printw("bsby=%f izby=%f popkfby=%f kof=%f nemi=%f\n",bsby,izby,uos.popkfby,kof,nemi);
OSTANOV();    
*/

if(vrabu == 1)
  amort[1]=(bsby-izby)*rekin.popkfby*kof/100/12.;
if(vrabu == 0)
 {
  amort[1]=bsby*rekin.popkfby*kof/100/12.;
  if(amort[1] > bsby -izby)
     amort[1]=bsby -izby;
 }
amort[0]=iceb_u_okrug(amort[0],0.01);
if(mra == 1)
 {

  if(vrabu == 1)
    sprintf(strsql,"(%.2f-%.2f)*%.2f/100/12*%.2f=%.2f",
    bsby,izby,kof,rekin.popkfby,amort[1]);
  if(vrabu == 0)
    sprintf(strsql,"%.2f*%.2f/100/12*%.2f=%.2f",
    bsby,kof,rekin.popkfby,amort[1]);
  
  iceb_menu_soob(strsql,wpredok);
 }

if(izby + amort[0] > bsby)
 {

  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %ld %s.\n",
  gettext("Инвентарный номер"),in,
  gettext("остаточная амортизация"));

  if(mra == 1)
   {
    sprintf(strsql,"%s %ld %s.\n",
    gettext("Инвентарный номер"),in,
    gettext("остаточная амортизация"));

    iceb_menu_soob(strsql,wpredok);
   }
 }

vp:;

/*Удаляем старую запись если она есть*/
sprintf(strsql,"delete from Uosamor1 where innom=%ld and god=%d and \
mes=%d",in,gr,mr);

iceb_sql_zapis(strsql,1,0,wpredok);
/*********
printw("%s: %5.2f %s: %.2f\n",
gettext("Амортизация"),amort[1],
gettext(bsby);
*************/
if(ff_prot != NULL)
  fprintf(ff_prot,"%s: %5.2f %s: %.2f\n",
gettext("Амортизация"),amort[1],
gettext("Балансовая стоимость"),bsby);

if(balst != 0.) //Если начальная балансовая стоимость равна 0. амортизацию не записывать
 { 
  time(&vrem);

  sprintf(strsql,"insert into Uosamor1 \
  values (%ld,%d,%d,%d,'%s','%s',%.2f,%d,%ld)",
  in,mr,gr,podr,rekin.hzt.ravno(),rekin.hnaby.ravno(),amort[1],iceb_getuid(wpredok),vrem);

  iceb_sql_zapis(strsql,1,0,wpredok);
 }


return(0);
}





