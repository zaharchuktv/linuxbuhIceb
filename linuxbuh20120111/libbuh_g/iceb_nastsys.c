/*$Id: iceb_nastsys.c,v 1.16 2011-02-21 07:36:07 sasa Exp $*/
/*09.12.2010	26.11.2004	Белых А.И.	iceb_nastsys.c
Чтение системных настроек
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "iceb_libbuh.h"

extern SQL_baza bd;

class iceb_u_str shrift_ravnohir; //Шрифт с одинаковой шириной всех букв
int kol_strok_na_liste=64;
int kol_strok_na_liste_l=47; //Количесво строк на стандартном листе в ориентации ландшафт
class iceb_u_str shrift_rek_raz;  //Шрифт которым будут отображаться реквизиты разработчика в главных меню подсистем
int timeout_progress_bar=500;
class iceb_u_str spis_print_a3; /*Список принтеров формата А3*/
extern class iceb_u_str kodirovka_iceb; /*определяется в iceb_start.c*/
extern int iceb_kod_podsystem;

int iceb_nastsys()
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
if(iceb_kod_podsystem != 0)
 if(iceb_prr(NULL) != 0)
  iceb_exit(0); /*Проверка можно ли работать оператору с этой подсистемой*/

shrift_ravnohir.new_plus("Monospase 12");
shrift_rek_raz.new_plus("");
spis_print_a3.new_plus("");

sprintf(strsql,"select str from Alx where fil='nastsys.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob("Не найдены настройки nastsys.alx",NULL);
  return(1);
 }



char stroka2[1024];
kol_strok_na_liste_l=47;
kol_strok_na_liste=64;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  
  iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),1,'|');

  if(iceb_u_SRAV(stroka2,"Шрифт с одинаковой шириной всех символов",0) == 0)
   {
    iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),2,'|');
    shrift_ravnohir.new_plus(stroka2);
    continue;
   }

  if(iceb_u_SRAV(stroka2,"Шрифт реквизитов разработчика",0) == 0)
   {
    iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),2,'|');
    shrift_rek_raz.new_plus(stroka2);
    continue;
   }

  if(iceb_u_SRAV(stroka2,"Количество строк на стандартном листе",0) == 0)
   {
    iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),2,'|');
    kol_strok_na_liste=atoi(stroka2);
    continue;
   }

  if(iceb_u_SRAV(stroka2,"Количество строк на стандартном листе в ориентации ландшафт",0) == 0)
    {
     iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),2,'|');
     kol_strok_na_liste_l=atoi(stroka2);
     continue;
    }

  if(iceb_u_SRAV(stroka2,"Таймаут в милесекундах для движения шкалы степени выполнения",0) == 0)
    {
     iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),2,'|');
     timeout_progress_bar=atoi(stroka2);
     continue;
    }
  if(iceb_u_SRAV(stroka2,"Список принтеров формата А3",0) == 0)
    {
     iceb_u_polen(row_alx[0],&spis_print_a3,2,'|');
     continue;
    }
  if(iceb_u_SRAV(stroka2,"Кодировка в которой работает программа",0) == 0)
   {
    iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),2,'|');
    if(stroka2[0] != '\0')
      iceb_u_polen(row_alx[0],&kodirovka_iceb,2,'|');
    continue;
   }

 }

if(kodirovka_iceb.getdlinna() <= 1)
 {
  kodirovka_iceb.new_plus("utf8");
 }

return(0);
 
}
