/*$Id: iceb_rab_kas.c,v 1.20 2011-02-21 07:36:07 sasa Exp $*/
/*26.12.2004	23.12.2004	Белых А.И.	iceb_rab_kas.c
Работа с кассовым регистратором
*/
#include <stdlib.h>
#include "iceb_libbuh.h"

int	settimmar(struct KASSA *kasr);
void	printdayshek(struct KASSA *kasr);
int	mariqtxt(struct KASSA *kasr, int nstr,const char *soob1);
extern int iceb_phzp(struct KASSA *kasr);

extern char *organ;

void    iceb_rab_kas(struct KASSA *kasr,int metka) //0-всё меню 1-без двух последних строчек
{
char       strsql[512];
iceb_u_str stroka;
int nomer=0;
double suma;

printf("xbug_nsi\n");


while(nomer >= 0)
 {

  iceb_u_str titl;
  iceb_u_str zagolovok;
  iceb_u_spisok punkt_m;

  titl.plus(gettext("Работа с кассовым регистратором"));

  stroka.new_plus("\n");
  memset(strsql,'\0',sizeof(strsql));
  strncpy(strsql,organ,40);
  stroka.plus_ps(strsql);
  stroka.plus_ps(gettext("Работа с кассовым регистратором"));

  zagolovok.plus(stroka.ravno());


  punkt_m.plus(gettext("Печать дневного отчетного чека."));//0
  punkt_m.plus(gettext("Очистить индикатор кассового регистратора."));//1
  punkt_m.plus(gettext("Печать отчетного чека за период."));//2
  if(metka == 0)
   {
    punkt_m.plus(gettext("Служебное внесение."));//3
    punkt_m.plus(gettext("Служебное изъятие."));//4
    punkt_m.plus(gettext("Выдача из кассы клиенту."));//5
    punkt_m.plus(gettext("Внесение в кассу клиентом."));//6
   }


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  

  switch (nomer)
   {
    case -1:
      return;
      
    case 0:
      printdayshek(kasr);
      break;
    case 1:
      mariqtxt(kasr,1,"");
      break;

    case 2:
      iceb_phzp(kasr);
      break;

    case 3:
      iceb_mariq_slvi(kasr,&suma,0);
      break;

    case 4:
      iceb_mariq_slvi(kasr,&suma,1);
      break;

    case 5:
     iceb_mariq_vs(0,kasr);
     break;

    case 6:
     iceb_mariq_vs(1,kasr);
     break;
   }
   
 }

}

/********************************************/
/*Установка времени в кассовом регистраторе*/
/********************************************/

int	settimmar(struct KASSA *kasr)
{
struct  tm      *bf;
time_t          tmm;
char		timev[20];
int		hour,min,sec;
char		bros[512];
char		strsql[512];

time(&tmm);
bf=localtime(&tmm);



sprintf(timev,"%02d:%02d",bf->tm_hour,bf->tm_min);

naz:;

if(iceb_menu_vvod1(gettext("Введите точное время"),timev,6,NULL) != 0)
 return(1);

iceb_u_polen(timev,bros,sizeof(bros),1,':');
hour=atoi(bros);
if(hour > 24 || hour < 0)
 {
  sprintf(strsql,"%s => %d",gettext("Неверно введен час"),hour);  
  iceb_menu_soob(strsql,NULL);
  goto naz;
 }

iceb_u_polen(timev,bros,sizeof(bros),2,':');
min=atoi(bros);
if(min > 60 || hour < 0)
 {
  sprintf(strsql,"%s => %d",gettext("Неверно введены минуты"),min);  
  iceb_menu_soob(strsql,NULL);

  goto naz;
 }
sec=0;


  sprintf(strsql,"FSTART: MARIA_TIME\n\
%s\n\
%s\n\
%d\n\
%d\n\
%d\n\
FEND:\n",
  kasr->prodavec.ravno(), kasr->parol, hour, min, sec);  
  if (iceb_cmd(kasr, strsql,NULL))
   return(1);
  else 
  {
   sprintf(strsql,"%s !",gettext("Время установлено"));  
   iceb_menu_soob(strsql,NULL);
  
   return(0);
  }
}



/**********************************/
/*Печать дневного чека*/
/**************************/

void	printdayshek(struct KASSA *kasr)
{
int	dayreg;
char	strsql[512];


if(iceb_menu_danet(gettext("Печать дневного отчетного чека."),2,NULL) == 2)
  return;

dayreg=0;
if(iceb_menu_danet(gettext("Обнулять дневные регистры"),2,NULL) == 2)
  dayreg=1;


sprintf(strsql,"FSTART: MARIA_DAY_REPORT\n\
%s\n\
%s\n\
%d\n\
FEND:\n",
kasr->prodavec.ravno(), kasr->parol,dayreg);  

iceb_cmd(kasr, strsql,NULL);


if(dayreg == 1)
   settimmar(kasr);

}
/********************************************************/
/*Распечатать сообщение на экане кассового регистратора*/
/********************************************************/
int	mariqtxt(struct KASSA *kasr, int nstr, const char *soob1)
{
char		strsql[512];

sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%s\n\
FEND:\n",
kasr->prodavec.ravno(), kasr->parol, nstr, soob1);  
//  printw("\n%s\n",strsql);

return(iceb_cmd(kasr, strsql,NULL));
}
