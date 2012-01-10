/* $Id: amortw.c,v 1.12 2011-03-28 06:55:45 sasa Exp $ */
/*28.09.2010    08.07.1996      Белых А.И.      amortw.c
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

void	amortbo(int,int,double,short,double,double,float,const char*,const char*,FILE *ff_prot,GtkWidget*);

extern float      nemi; /*Необлагаемый минимум*/
extern short metkabo;  //Если 1 то организация бюджетная
extern SQL_baza bd;

int amortw(int in,
short mr,short gr,
short mra, //0-расчет в потоке 1-нет
short kvrt,  //Квартал
const char *grupa,
FILE *ff_prot,
GtkWidget *wpredok)
{
int             i,i1;
double          bs=0.,iz=0.;
double		amort[4];
double		kof;
int		podr;
SQL_str         row;
SQLCURSOR curr;
char		strsql[1024];
time_t		vrem;
int		kodotl;
class bsizw_data bal_st;
class poiinpdw_data rekin;

printf("%s\n",__FUNCTION__);

if(ff_prot != NULL)
 {
  fprintf(ff_prot,"%s\n",__FUNCTION__);
 }
else
  printf("ff_prot=NULL\n");

podr=0;
if(metkabo == 1)
  mr=1;
  

memset(&amort,'\0',sizeof(amort));
 
if(kvrt != 0)
 {
  if(kvrt == 1)
    mr=1;
  if(kvrt == 2)
    mr=4;
  if(kvrt == 3)
    mr=7;
  if(kvrt == 4)
    mr=10;
 }
/********
if(mra == 0)
  printw("in-%d %d.%d\n",in,mr,gr);
***********/
if(ff_prot != NULL)
  fprintf(ff_prot,"Расчёт амортизации для налогового учёта.\nin-%d %d.%d\n",in,mr,gr);


if(metkabo == 1)
 {
  if((i=poiinw(in,1,1,gr+1,&podr,&kodotl,wpredok)) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s %d %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),in,
    gettext("не числится по дате"),1,gr+1,
    gettext("Подпрограмма вернула"),i);
    if(mra == 1)
     {
      fprintf(ff_prot,"%s %d %s %d.%d !\n %s %d",
      gettext("Инвентарный номер"),in,
      gettext("не числится по дате"),1,gr+1,
      gettext("Подпрограмма вернула"),i);
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }   
 }
else
 if((i=poiinw(in,1,mr,gr,&podr,&kodotl,wpredok)) != 0)
  {

   if(ff_prot != NULL)
     fprintf(ff_prot,"%s %d %s %d.%d ! %s %d.\n",
   gettext("Инвентарный номер"),in,
   gettext("не числится по дате"),mr,gr,
   gettext("Подпрограмма вернула"),i);

    if(mra == 1)
     {
      sprintf(strsql,"%s %d\n %s %d.%d !\n %s %d",
      gettext("Инвентарный номер"),in,
      gettext("не числится по дате"),mr,gr,
      gettext("Подпрограмма вернула"),i);
      iceb_menu_soob(strsql,wpredok);
     }
   return(1);
  }   


if(metkabo == 1)
 {
  if((i=poiinpdw(in,1,gr+1,&rekin,wpredok)) != 0)
   {

   if(ff_prot != NULL)
     fprintf(ff_prot,"%s %d %s %d.%d ! %s %d\n",
    gettext("Инвентарный номер"),in,
    gettext("не найдено настройки на дату"),1,gr+1,
    gettext("Подпрограмма вернула"),i);
    if(mra == 1)
     {
      sprintf(strsql,"%s %d\n%s %d.%d !\n %s %d",
      gettext("Инвентарный номер"),in,
      gettext("не найдено настройки на дату"),1,gr+1,
      gettext("Подпрограмма вернула"),i);
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }  
 }
else
if((i=poiinpdw(in,mr,gr,&rekin,wpredok)) != 0)
 {

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s %d.%d ! %s %d\n",
  gettext("Инвентарный номер"),in,
  gettext("не найдено настройки на дату"),mr,gr,
  gettext("Подпрогамма вернула"),i);

  if(mra == 1)
   {
    sprintf(strsql,"%s %d\n %s %d.%d !\n %s %d",
    gettext("Инвентарный номер"),in,
    gettext("не найдено настройки на дату"),mr,gr,
    gettext("Подпрогамма вернула"),i);
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }  

if(iceb_u_proverka(grupa,rekin.hna.ravno(),0,0) != 0 || uosprovgrw(rekin.hna.ravno(),wpredok) != 1)
 {

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s (%s)\n",
  gettext("Инвентарный номер в группе"),rekin.hna.ravno(),
  gettext("амортизация не считается"));

  if(mra == 1)
   {
    sprintf(strsql,"%s %s (%s)",
    gettext("Инвентарный номер в группе"),rekin.hna.ravno(),
    gettext("амортизация не считается"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }

if(rekin.soso == 1 || rekin.soso == 2  )
 {

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s.\n",
  gettext("Инвентарный номер"),in,
  gettext("не эксплуатируется"));

  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s.",
    gettext("Инвентарный номер"),in,
    gettext("не эксплуатируется"));
    iceb_menu_soob(strsql,wpredok);
   }

  goto vp;
 }

//printw("in=%d podr=%d %d.%d\n",in,podr,mr,gr);
bs=iz=0.;

if(metkabo != 1)
  bsizw(in,podr,1,mr,gr,&bal_st,ff_prot,wpredok);
else
  bsizw(in,podr,1,1,gr+1,&bal_st,ff_prot,wpredok);

bs=bal_st.sbs+bal_st.bs-uosgetlsw(in,1,mr,gr,0,wpredok);
iz=bal_st.siz+bal_st.iz+bal_st.iz1;

if(ff_prot != NULL)
 fprintf(ff_prot,"bs=%f iz=%f\n",bs,iz);
 
if(bs <= iz)
 {

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s. bs=%.2f <= iz=%.2f\n",
  gettext("Инвентарный номер"),in,
  gettext("амортизирован полностью"),bs,iz);

  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s. %.2f <= %.2f",
    gettext("Инвентарный номер"),in,
    gettext("амортизирован полностью"),bs,iz);
    iceb_menu_soob(strsql,wpredok);  
   }
 }

/*Читаем коэффициент*/
sprintf(strsql,"select kof from Uosgrup where kod='%s'",rekin.hna.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {


  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %d - %s %s !\n",
  gettext("Инвентарный номер"),in,
  gettext("не нашли кода группы"),rekin.hna.ravno());

  if(mra == 1)
   {
    sprintf(strsql,"%s %d - %s %s !",
    gettext("Инвентарный номер"),in,
    gettext("не нашли кода группы"),rekin.hna.ravno());
   }
  return(1);
 }

kof=atof(row[0]);
if(ff_prot != NULL)
 fprintf(ff_prot,"Коэффициент=%f\n",kof);
 
/*
printw("bs=%f iz=%f popkf=%f kof=%f nemi=%f\n",bs,iz,uos.popkf,kof,nemi);
OSTANOV();    
*/

if(metkabo == 1)
 {
  amortbo(in,podr,kof,gr,bs,iz,rekin.popkf,rekin.hzt.ravno(),rekin.hna.ravno(),ff_prot,wpredok);
  return(0);
 }

if(uosprovarw(rekin.hna.ravno(),0,wpredok) == 1)
 {
  if(kvrt == 0)
   {
    if(ff_prot == NULL)
     fprintf(ff_prot,"Номер квартала равен нолю! Аварийное завершение расчёта!\n");
    return(1);
   }   
  if(iceb_u_SRAV(rekin.hna.ravno(),"1",0) == 0 && bs-iz <= 100*nemi)
   {
    amort[1]=bs-iz;
   }
  else
   {      
    
    amort[0]=(bs-iz)*rekin.popkf*3*kof/100/12.;

    if(ff_prot != NULL)
      fprintf(ff_prot,"%s=(%.2f-%.2f)*%.2f/100/12*3*%.2f=%.2f\n",
    gettext("Амортизация"),
    bs,iz,kof,rekin.popkf,amort[0]);
   
    if(mra == 1)
     {
      sprintf(strsql,"%s\n(%.2f-%.2f)*%.2f/100/12*3*%.2f=%.2f",
      gettext("Амортизация"),
      bs,iz,kof,rekin.popkf,amort[0]);
      iceb_menu_soob(strsql,wpredok);
     }

    amort[0]=iceb_u_okrug(amort[0],0.01);
    amort[1]=amort[0]/3;
    amort[1]=iceb_u_okrug(amort[1],0.01);
    amort[2]=amort[1];
    amort[3]=amort[0]-amort[1]-amort[2];


   }
 }
else  /*Так как было раньше*/
 {

  if(kvrt != 0 )
   {
    return(1); 
   }
  amort[1]=bs*rekin.popkf*kof/100/12.;
  if(amort[1] > bs-iz)
    amort[1]=bs-iz;
  amort[0]=iceb_u_okrug(amort[0],0.01);
  if(mra == 1)
   {
    sprintf(strsql,"%s\n%.2f*%.2f/100/12.*%.2f=%.2f",
    gettext("Амортизация"),
    bs,kof,rekin.popkf,amort[1]);

    iceb_menu_soob(strsql,wpredok);
   }
 }

if(mra == 1 && bs == 0.)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"%d-%s\n",in,
    gettext("Балансовая стоимость равна нолю !"));

  sprintf(strsql,"%d-%s",in,gettext("Балансовая стоимость равна нолю !"));
  iceb_menu_soob(strsql,wpredok);
 }

 
if(iz + amort[0] > bs)
 {

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s.\n",
  gettext("Инвентарный номер"),in,
  gettext("остаточная амортизация"));
  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s",
    gettext("Инвентарный номер"),in,
    gettext("остаточная амортизация"));
    iceb_menu_soob(strsql,wpredok);
   }
 }

vp:;

/*Удаляем старую запись если она есть*/
sprintf(strsql,"delete from Uosamor where innom=%d and god=%d and mes=%d",in,gr,mr);
iceb_sql_zapis(strsql,1,0,wpredok);
 
if(kvrt != 0)
 for(i=mr+1 ; i < mr+3; i++)
  {
   sprintf(strsql,"delete from Uosamor where innom=%d and god=%d and  mes=%d",in,gr,i);
   iceb_sql_zapis(strsql,1,0,wpredok);
  }

if(ff_prot != NULL)
 fprintf(ff_prot,"%s: %5.2f %s: %.2f\n",
gettext("Амортизация"),amort[1],
gettext("Балансовая стоимость"),bs);
time(&vrem);

sprintf(strsql,"insert into Uosamor \
values (%d,%d,%d,%d,'%s','%s',%.2f,%d,%ld)",
in,mr,gr,podr,rekin.hzt.ravno(),rekin.hna.ravno(),amort[1],iceb_getuid(wpredok),vrem);

iceb_sql_zapis(strsql,1,0,wpredok);

i1=2;
if(kvrt != 0)
 for(i=mr+1 ; i < mr+3; i++)
  {
   sprintf(strsql,"insert into Uosamor \
values (%d,%d,%d,%d,'%s','%s',%.2f,%d,%ld)",
   in,i,gr,podr,rekin.hzt.ravno(),rekin.hna.ravno(),amort[i1++],iceb_getuid(wpredok),vrem);

   iceb_sql_zapis(strsql,1,0,wpredok);
  }

return(0);
}
/*************************/
/*Амортизация для бюджетных организаций*/
/***************************************/

void amortbo(int in,
int podr,
double kof,
short gr,
double bs,
double iz,
float popkf,
const char *hzt,
const char *hna,
FILE *ff_prot,
GtkWidget *wpredok)
{
time_t  vrem;
char	strsql[512];
if(ff_prot != NULL)

fprintf(ff_prot,"%s\n",__FUNCTION__);

double amort=bs*popkf/100*kof;
if(amort > bs-iz)
  amort=bs-iz;

amort=iceb_u_okrug(amort,1.); //Округляем до гривны

/*Удаляем старую запись если она есть*/
sprintf(strsql,"delete from Uosamor where innom=%d and god=%d",in,gr);
iceb_sql_zapis(strsql,1,0,wpredok);

time(&vrem);

sprintf(strsql,"insert into Uosamor \
values (%d,%d,%d,%d,'%s','%s',%.2f,%d,%ld)",
in,12,gr,podr,hzt,hna,amort,iceb_getuid(wpredok),vrem);

if(ff_prot != NULL)
 fprintf(ff_prot,"%s\n",strsql);
 
iceb_sql_zapis(strsql,1,0,wpredok);

}

