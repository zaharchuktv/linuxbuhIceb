/* $Id: matnastw.c,v 1.14 2011-02-21 07:35:55 sasa Exp $ */
/*14.12.2010    14.04.1997      Белых А.И.     matnastw.c
Чтение настpойки для материального учета 
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <errno.h>
#include        <string.h>
#include <unistd.h>
#include	"buhg_g.h"

char            *mtsh=NULL;  /*Перечень материальных счетов*/
double		okrcn;  /*Округление цены*/
double		okrg1;  /*Округление 1*/
short           mfnn; /*Метка формирования номера накладной
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                       */
short		cnsnds; /*0-Цена материалла без ндс 1- с ндс*/
short		srtnk; /*0-не включена 1-включена сортировка записей в накладной*/
short		vtara; /*Код группы возвратная тара*/
char		*bnds1=NULL; /*0% НДС реализация на територии Украины.*/
char            *bnds2=NULL; /*0% НДС экспорт.*/
char            *bnds3=NULL; /*0% Освобождение от НДС статья 5.*/
short           obzap; /*0-не объединять записи 1-обединять*/
double		kursue;/*Курс условной единицы*/
char		*shrt=NULL;    /*Счета розничной торговли*/
short		konost;   /*Контроль остатка 0-включен 1-выключен*/
short		startgod; /*Стартовый год просмотров для материального учёта*/
short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		startgodb; /*Стартовый год просмотров главной книги*/
short		mborvd;    /*0-многопользовательская работа в документе разрешена 1- запрещена*/
char		*kodopsrvc=NULL; //Код операции расчета средневзвешенной цены
char		*kodopsp=NULL; //Коды операций сторнирования для приходов
char		*kodopsr=NULL; //Коды операций сторнирования для расходов
short metka_pros_mat_s_ost=0; //0-всё показывать 1-только материалы с остатками
extern SQL_baza bd;

int             matnastw(void)
{
char		bros[1024];
char            st1[1024];
short           i;

//printw("\n%s\n",
//gettext("Чтение нормативно-справочной информации"));
 
mborvd=vplsh=konost=0;
kursue=0.;
obzap=0;
vtara=0;
srtnk=0;
metka_pros_mat_s_ost=0;

if(bnds1 != NULL)
 {
  delete [] bnds1;
  bnds1=NULL;
 }

if(bnds2 != NULL)
 {
  delete [] bnds2;
  bnds2=NULL;
 }

if(bnds3 != NULL)
 {
  delete [] bnds3;
  bnds3=NULL;
 }

if(shrt != NULL)
 {
  delete [] shrt;
  shrt=NULL;
 }
if(kodopsrvc != NULL)
 {
  delete [] kodopsrvc;
  kodopsrvc=NULL;
 }
if(mtsh != NULL)
 {
  delete [] mtsh;
  mtsh=NULL;
 }
if(kodopsp != NULL)
 {
  delete [] kodopsp;
  kodopsp=NULL;
 }
if(kodopsr != NULL)
 {
  delete [] kodopsp;
  kodopsp=NULL;
 }
  
startgod=0;
cnsnds=0;
okrg1=okrcn=0.;
mfnn=(-1);

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='matnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"matnast.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;


  iceb_u_polen(row_alx[0],st1,sizeof(st1),1,'|');

  if(iceb_u_SRAV(st1,"Контроль остатка на выписке",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(iceb_u_SRAV(st1,"Вкл",1) == 0)
        konost=0;
     else
        konost=1;
     
     continue;
    }
  if(iceb_u_SRAV(st1,"Показывать только материалы имеющие остаток",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(iceb_u_SRAV(st1,"Вкл",1) == 0)
        metka_pros_mat_s_ost=1;
     else
        metka_pros_mat_s_ost=0;
     
     continue;
    }
  if(iceb_u_SRAV(st1,"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     startgod=(short)iceb_u_atof(st1); 
     continue;
    }
  if(iceb_u_SRAV(st1,"Блокировка многопользовательской работы в документе",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(iceb_u_SRAV(st1,"Вкл",1) == 0)
        mborvd=1;
     else
        mborvd=0;
     
     continue;
    }
    
  if(iceb_u_SRAV(st1,"Счета розничной торговли",0) == 0)
    {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     shrt=new char[i];
     strcpy(shrt,bros);
     continue;
    }
  if(iceb_u_SRAV(st1,"Код операции расчета средневзвешенной цены",0) == 0)
    {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     kodopsrvc=new char[i];
     strcpy(kodopsrvc,bros);
     continue;
    }

  if(iceb_u_SRAV(st1,"0% НДС реализация на територии Украины",0) == 0)
    {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     bnds1=new char[i];
     strcpy(bnds1,bros);
     continue;
    }
  if(iceb_u_SRAV(st1,"0% НДС экспорт",0) == 0)
    {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     bnds2=new char[i];
     strcpy(bnds2,bros);
     continue;
    }
  if(iceb_u_SRAV(st1,"0% Освобождение от НДС статья 5",0) == 0)
    {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     bnds3=new char[i];
     strcpy(bnds3,bros);
     continue;
    }

  if(iceb_u_SRAV(st1,"Цена материалла с НДС",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(iceb_u_SRAV(st1,"Вкл",1) == 0)
       cnsnds=1;
     continue;
    }

  if(iceb_u_SRAV(st1,"Объединение одинаковых материалов в накладной",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(iceb_u_SRAV(st1,"Вкл",1) == 0)
       obzap=1;
     continue;
    }


  if(iceb_u_SRAV(st1,"Перечень материальных счетов",0) == 0)
    {
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     mtsh=new char[i];
     strcpy(mtsh,bros);
     continue;
    }
  if(iceb_u_SRAV(st1,"Коды операций сторнирования для приходов",0) == 0)
   {
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     kodopsp=new char[i];
     strcpy(kodopsp,bros);
     continue;
   }
  if(iceb_u_SRAV(st1,"Округление цены",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     okrcn=iceb_u_atof(st1);
     continue;
    }

  if(iceb_u_SRAV(st1,"Округление 1",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     okrg1=iceb_u_atof(st1);
     continue;
    }
  if(iceb_u_SRAV(st1,"Курс УЕ",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     kursue=iceb_u_atof(st1);
     continue;
    }
  if(iceb_u_SRAV(st1,"Код группы возвратная тара",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     vtara=(short)iceb_u_atof(st1);
     continue;
    }
  if(iceb_u_SRAV(st1,"Сортировка записей в накладной",0) == 0)
   {
    srtnk=0;
    iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
      
    if(iceb_u_SRAV(st1,"Вкл",1) == 0)
       srtnk=1;
     continue;
    
   }
  if(iceb_u_SRAV(st1,"Раздельная нумерация накладных",0) == 0)
    {
     mfnn=0;
     
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(st1[0] == '\0' || st1[0] == '\n')
      mfnn=(-1);
      
     if(iceb_u_SRAV(st1,"Вкл",1) == 0)
       mfnn=1;
     continue;
    }
 }

iceb_poldan("Многопорядковый план счетов",bros,"nastrb.alx",NULL);
if(iceb_u_SRAV(bros,"Вкл",1) == 0)
 vplsh=1;

memset(bros,'\0',sizeof(bros));
startgodb=0;
if(iceb_poldan("Стартовый год",bros,"nastrb.alx",NULL) == 0)
 {
  startgodb=(short)iceb_u_atof(bros);
 }
iceb_u_str reploh;

if(okrcn == 0.)
 {
  reploh.plus_ps("Не введено \"Округление цены\" !");
 }

if(okrg1 == 0.)
 {
  reploh.plus("Не введено \"Округление 1\" !");
 }

if(reploh.getdlinna() > 1)
 iceb_menu_soob(&reploh,NULL);
 
return(0);
}
