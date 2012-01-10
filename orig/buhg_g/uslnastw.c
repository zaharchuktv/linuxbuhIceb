/*$Id: uslnastw.c,v 1.10 2011-02-21 07:35:58 sasa Exp $*/
/*23.04.2010	11.02.2000	Белых А.И.	uslnast.c
Чтение настроек для услуг
Если вернули 0 файл прочитан
	     1 файл не прочитан
*/
#include        <errno.h>
#include        <stdlib.h>
#include <unistd.h>
#include	"buhg_g.h"

short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		startgodus=0; /*Стартовый год просмотров услуг*/
short		startgodb=0; /*Стартовый год Главной книги*/
short		startgod; //Стартовый год просмотров в материальном учёте

char		*bndsu1; /*0% НДС реализация на територии Украины.*/
char            *bndsu2; /*0% НДС экспорт.*/
char            *bndsu3; /*0% Освобождение от НДС статья 5.*/
//double          nds1; /*Н.Д.С.*/
double		okrcn;  /*Округление цены*/
double		okrg1;  /*Округление 1*/
short		cnsnds; /*0-Цена материалла без ндс 1- с ндс*/
short		cnsndsu; /*0-Цена услуг без ндс 1- с ндс*/
short           mfnn; /*Метка формирования номера накладной
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                       */
double		kursue=0.;/*Курс условной единицы*/
extern SQL_baza bd;

int		uslnastw()
{
char		st1[1024];
char		bros[1024];
int		i;
short		voz;


startgod=0;
cnsnds=cnsndsu=0;
okrg1=okrcn=0.;
vplsh=startgodus=0;
mfnn=(-1);
kursue=0.;
if(bndsu1 != NULL)
 {
  delete [] bndsu1;
  bndsu1=NULL;
 }

if(bndsu2 != NULL)
 {
  delete [] bndsu2;
  bndsu2=NULL;
 }

if(bndsu3 != NULL)
 {
  delete [] bndsu3;
  bndsu3=NULL;
 }

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"uslnast.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  iceb_u_polen(row_alx[0],st1,sizeof(st1),1,'|');
  if(iceb_u_SRAV(st1,"Раздельная нумерация документов",0) == 0)
    {
     mfnn=0;
     
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(st1[0] == '\0' || st1[0] == '\n')
      mfnn=(-1);
      
     if(iceb_u_SRAV(st1,"Вкл",1) == 0)
       mfnn=1;
     continue;
    }

  if(iceb_u_SRAV(st1,"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     startgodus=atoi(st1); 
     continue;
    }
  if(iceb_u_SRAV(st1,"0% НДС реализация на територии Украины",0) == 0)
    {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if((bndsu1=new char[i]) == NULL)
       printf("Не могу выделить пямять для bndsu1 !\n");
     strcpy(bndsu1,bros);
     continue;
    }
  if(iceb_u_SRAV(st1,"0% НДС экспорт",0) == 0)
    {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if((bndsu2=new char[i]) == NULL)
       printf("Не могу выделить пямять для bndsu2 !\n");
     strcpy(bndsu2,bros);
     continue;
    }
  if(iceb_u_SRAV(st1,"0% Освобождение от НДС статья 5",0) == 0)
    {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     if((bndsu3=new char[i]) == NULL)
       printf("Не могу выделить пямять для bndsu3 !\n");
     strcpy(bndsu3,bros);
     continue;
    }

  if(iceb_u_SRAV(st1,"Цена услуг с НДС",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(iceb_u_SRAV(st1,"Вкл",1) == 0)
       cnsndsu=1;
     continue;
    }

 }
const char *imaf_alx1={"matnast.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx1);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx1);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  iceb_u_polen(row_alx[0],st1,sizeof(st1),1,'|');
  if(iceb_u_SRAV(st1,"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     startgod=atoi(st1); 
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
  if(iceb_u_SRAV(st1,"Цена материалла с НДС",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(iceb_u_SRAV(st1,"Вкл",1) == 0)
       cnsnds=1;
     continue;
    }
  if(iceb_u_SRAV(st1,"Курс УЕ",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     kursue=iceb_u_atof(st1);
     continue;
    }
 }
vplsh=0;
iceb_poldan("Многопорядковый план счетов",st1,"nastrb.alx",NULL);
if(iceb_u_SRAV(st1,"Вкл",1) == 0)
 vplsh=1;

memset(st1,'\0',sizeof(st1));
startgodb=0;
if(iceb_poldan("Стартовый год",st1,"nastrb.alx",NULL) == 0)
 {
  startgodb=atoi(st1);
 }

voz=0;
iceb_u_str repl;

if(okrcn == 0.)
 {
  voz++;
//  printw(gettext("Не введено \"Округление цены\" !\n"));
  repl.plus_ps("Не введено \"Округление цены\" !");
  
 }

if(okrg1 == 0.)
 {
  voz++;
//  printw(gettext("Не введено \"Округление 1\" !\n"));
  repl.plus_ps("Не введено \"Округление 1\" !");
 }

if(voz != 0)
 iceb_menu_soob(&repl,NULL);
 
return(0);

}
