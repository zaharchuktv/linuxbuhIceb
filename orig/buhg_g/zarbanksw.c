/*$Id: zarbanksw.c,v 1.7 2011-02-21 07:36:00 sasa Exp $*/
/*20.03.2010	09.12.2009	Белых А.И.	zarmbanksw.c
Получение списка банков
*/
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"

extern SQL_baza bd;

int zarbanksw(class iceb_u_str *kod00,int *kod_ban,GtkWidget *wpredok) /*Код банка в списке банков*/
{
class iceb_u_spisok naimb;
class iceb_u_spisok kodbi;
class iceb_u_str nmb("");
class iceb_u_str kodb("");
class iceb_u_spisok kodkontr;
class iceb_u_str kodk("");
class SQLCURSOR cur_alx;
class iceb_u_int kod_banka;
int kod_banka_int=0;
SQL_str row_alx;
int kolstr=0;
char strsql[512];
*kod_ban=0;

sprintf(strsql,"select str from Alx where fil='zarbanks.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(-1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob("Не найдены настройки zarbanks.alx",wpredok);
  return(-1);
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],&kodb,1,'|') != 0)
   continue;

  if(iceb_u_polen(row_alx[0],&nmb,2,'|') != 0)
   continue;

  iceb_u_polen(row_alx[0],&kodk,3,'|');
  if(kodk.getdlinna() <= 1)
   kodk.new_plus("00");

  iceb_u_polen(row_alx[0],&kod_banka_int,4,'|');
   
  naimb.plus(nmb.ravno());
  kodbi.plus(kodb.ravno());  
  kodkontr.plus(kodk.ravno());
  kod_banka.plus(kod_banka_int);
 }

if(naimb.kolih() == 1)
 {
  *kod_ban=kod_banka.ravno(0);
  kod00->new_plus(kodkontr.ravno(0));
  return(kodbi.ravno_atoi(0));
 } 

class iceb_u_spisok naim_kol;

naim_kol.plus(gettext("Код банка"));
naim_kol.plus(gettext("Наименование"));
iceb_u_str naim_menu;
naim_menu.plus(gettext("Выбор банка"));
int voz;  
if((voz=iceb_l_spisokm(&kodbi,&naimb,&naim_kol,&naim_menu,0,wpredok)) < 0)
  return(-1);

*kod_ban=kod_banka.ravno(voz);
kod00->new_plus(kodkontr.ravno(voz));
return(kodbi.ravno_atoi(voz));
}



