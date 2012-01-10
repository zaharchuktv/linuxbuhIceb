/*$Id: ukrnastw.c,v 1.8 2011-02-21 07:35:58 sasa Exp $*/
/*23.04.2010	22.10.2002	Белых А.И.	ukrnastw.c
Чтение настроечной информации для подсистемы "Учет командировочных расходов."
*/
#include        <errno.h>
#include <unistd.h>
#include	"buhg_g.h"


short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		startgod=0;
short		startgodb=0; /*Стартовый год просмотров главной книги*/
short           mfnn=0; //Метка формирования номера в этой подсистеме не задействована
double		okrcn=0.01;  /*Округление цены*/
double		okrg1=0.01;  /*Округление 1*/
extern SQL_baza bd;

int ukrnastw()
{
char		bros[1024];
char            st1[1024];

startgodb=startgod=0;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='ukrnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"ukrnast.alx");
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
 }

iceb_poldan("Многопорядковый план счетов",bros,"nastrb.alx",NULL);
if(iceb_u_SRAV(bros,"Вкл",1) == 0)
 vplsh=1;

memset(bros,'\0',sizeof(bros));
startgodb=0;
if(iceb_poldan("Стартовый год",bros,"nastrb.alx",NULL) == 0)
  startgodb=(short)iceb_u_atof(bros);

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
okrg1=0.01;
okrcn=0.01;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  iceb_u_polen(row_alx[0],st1,sizeof(st1),1,'|');

  if(iceb_u_SRAV(st1,"Округление 1",0) == 0)
   {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     okrg1=iceb_u_atof(st1);
     continue;
   }
/****************
  if(iceb_u_SRAV(st1,"Н.Д.С.",0) == 0)
   {
    iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
    nds1=iceb_u_atof(st1);
    nds1=iceb_u_okrug(nds1,0.01);
    continue;
   }
***************/
  if(iceb_u_SRAV(st1,"Округление цены",0) == 0)
   {
    iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
    okrcn=iceb_u_atof(st1);
    continue;
   }
 }
return(0); 
}
