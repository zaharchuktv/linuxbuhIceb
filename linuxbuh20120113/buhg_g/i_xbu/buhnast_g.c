/*$Id: buhnast_g.c,v 1.18 2011-02-21 07:35:51 sasa Exp $ */
/*10.11.2009    21.12.1993      Белых А.И.      buhnast_g.c
Чтение настройки для бугалтерии
Если вернули 0-файл прочитан
	     1-нет
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <errno.h>
#include <unistd.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

double		okrcn=0.;  /*Округление цены*/
double		okrg1=0.;  /*Округление 1*/
short		koolk; /*Корректор отступа от левого края*/
short		startgod=0; /*Стартовый год материального учёта*/
short		startgodb=0; /*Стартовый год главной книги*/
short		startgoduos=0; /*Стартовый год для Учёта основных средств*/
short		vplsh; /*0-двух порядковый план счетов 1-многпорядковый*/
char		*sbshet=NULL; //Список бюджетных счетов
extern SQL_baza bd;

int             buhnast_g()
{
char            st1[1024];
char		bros[1024];

vplsh=startgodb=koolk=0;

if(sbshet != NULL)
 {
  delete [] sbshet;
  sbshet=NULL;
 }
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='nastrb.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"nastrb.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  iceb_u_polen(row_alx[0],st1,sizeof(st1),1,'|');

  if(iceb_u_SRAV(st1,"Корректор отступа от левого края",0) == 0)
    {
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     koolk=(short)iceb_u_atof(bros);
     continue;
    }

  if(iceb_u_SRAV(st1,"Многопорядковый план счетов",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(iceb_u_SRAV(st1,"Вкл",1) == 0)
       vplsh=1;
     continue;
    }
  if(iceb_u_SRAV(st1,"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     startgodb=(short)iceb_u_atof(st1); 
     continue;
    }

  if(iceb_u_SRAV(st1,"Список бюджетных счетов",0) == 0)
   {
    memset(bros,'\0',sizeof(bros));
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    if(bros[0] == '\0')
      continue;
    sbshet=new char[strlen(bros)+1];
    strcpy(sbshet,bros);
   }

 }

startgod=0;

okrg1=okrcn=0.;
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

  if(iceb_u_SRAV(st1,"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     startgod=(short)iceb_u_atof(st1); 
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

 }
startgoduos=0;

sprintf(strsql,"select str from Alx where fil='uosnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uosnast.alx");
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
     startgoduos=(short)iceb_u_atof(st1); 
     continue;
    }
 }

return(0);
}
