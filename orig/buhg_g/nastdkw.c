/* $Id: nastdkw.c,v 1.9 2011-02-21 07:35:55 sasa Exp $ */
/*15.10.2010    10.12.1993      Белых А.И.      nastdkw.c
Читаем настройку  для платежных документов
Если вернули 0 файл прочитан
	     1-нет
*/
#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"

double          okrg1; /*Округление*/
short           kopk; /*0-копеек нет 1-копейки есть*/
short           shetg; /*1-счет для железной дороги 0-нет*/
double          smp0,smp1; /*Суммы за переводы внутри банка и вне банка*/
short		mpo;       /*0-искать реквизиты в файле 1-в базе данных*/
short           rzk;       /*0-преобразование числа с тире 1-со вставкой запятой*/
short           mvs;       /*1-с разбивкой суммы по счетам 0-без*/
short		kor; /*Корректор межстрочного растояния*/
short		ots; /*Отступ от левого края*/
short		kp; /*Количество копий платежки*/
short		startgodd; /*Стартовый год платежных документов*/
short		startgodb; /*Стартовый год просмотров главной книги*/
short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		kolstrdok; //Количество строк между документами
extern SQL_baza bd;

int             nastdkw(void)
{
char            st1[1024];
char		bros[1024];

mpo=rzk=mvs=kor=ots=0;

kp=1;
kolstrdok=10;
vplsh=startgodd=shetg=startgodb=0;
smp0=smp1=okrg1=0.;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='nastdok.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"nastdok.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  iceb_u_polen(row_alx[0],st1,sizeof(st1),1,'|');

  if(iceb_u_SRAV(st1,"Поиск организаций в базе",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(iceb_u_SRAV(st1,"Вкл",1) == 0)
       mpo=1;
     continue;
    }

  if(iceb_u_SRAV(st1,"Разделитель копеек",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(iceb_u_SRAV(st1,"Зап",1) == 0)
       rzk=1;
     continue;
    }

  if(iceb_u_SRAV(st1,"Разбивка суммы",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(iceb_u_SRAV(st1,"Вкл",1) == 0)
       mvs=1;
     continue;
    }
  if(iceb_u_SRAV(st1,"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     startgodd=(short)iceb_u_atof(st1); 
     continue;
    }

  if(iceb_u_SRAV(st1,"Счет для ЖД",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     shetg=0;
     if(iceb_u_SRAV(st1,"Да",1) == 0)
       shetg=1;
     continue;
    }
  if(iceb_u_SRAV(st1,"Копейки",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     kopk=0;
     if(iceb_u_SRAV(st1,"Есть",0) == 0)
       kopk=1;
     continue;
    }
  if(iceb_u_SRAV(st1,"Округление 1",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     okrg1=iceb_u_atof(st1);
     continue;
    }

  if(iceb_u_SRAV(st1,"Количество копий платежки",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     kp=(short)iceb_u_atof(st1);
     continue;
    }

  if(iceb_u_SRAV(st1,"Количество строк между документами",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     if(st1[0] != '\0')
       kolstrdok=(short)iceb_u_atof(st1);
     continue;
    }

  if(iceb_u_SRAV(st1,"Корректор межстрочного растояния",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     kor=(short)iceb_u_atof(st1);
     continue;
    }
  if(iceb_u_SRAV(st1,"Отступ от левого края",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     ots=(short)iceb_u_atof(st1);
     continue;
    }
  if(iceb_u_SRAV(st1,"Сумма платежа",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     smp0=iceb_u_atof(st1);
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'/');
     smp1=iceb_u_atof(st1);
     continue;
    }


 }

memset(bros,'\0',sizeof(bros));
vplsh=0;
iceb_poldan("Многопорядковый план счетов",bros,"nastrb.alx",NULL);
if(iceb_u_SRAV(bros,"Вкл",1) == 0)
 vplsh=1;

memset(bros,'\0',sizeof(bros));
startgodb=0;
if(iceb_poldan("Стартовый год",bros,"nastrb.alx",NULL) == 0)
 {
  startgodb=(short)iceb_u_atof(bros);
 }


if(kp == 0)
 kp=1;

return(0);
}
