/*$Id: rnnovdw.c,v 1.6 2011-02-21 07:35:57 sasa Exp $*/
/*11.09.2010	13.12.2009	Белых А.И.	rnnovdw.c
Чтение настроек для определения вида документа
*/
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
#include "rnnovdw.h"

extern SQL_baza bd;

int rnnovdw(int metka,/*1-чтение настроек для приходных 2-для расходных*/
class rnnovd_rek *nvd,
GtkWidget *wpredok)
{
int metka_pods=0; /*1-материальный учёт 2 учёт услуг 3 учёт основных средств 4 учёт командировочных расходов*/
class iceb_u_str imaf;
class iceb_u_str viddok("");
class iceb_u_str nastr("");

if(metka == 1)
 imaf.plus("rnnovdp.alx");
if(metka == 2)
 imaf.plus("rnnovdr.alx");

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf.ravno());
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_strstrm(row_alx[0],"Материальный учёт") == 1)
   {
    metka_pods=1;
    continue;
   } 

  if(iceb_u_strstrm(row_alx[0],"Учёт услуг") == 1)
   {
    metka_pods=2;
    continue;
   } 

  if(iceb_u_strstrm(row_alx[0],"Учёт основных средств") == 1)
   {
    metka_pods=3;
    continue;
   } 

  if(iceb_u_strstrm(row_alx[0],"Учёт командировочных расходов") == 1)
   {
    metka_pods=4;
    continue;
   } 
  if(metka_pods == 0)
   continue;

  if(iceb_u_polen(row_alx[0],&viddok,1,'|') != 0)
   continue;  

  if(iceb_u_polen(row_alx[0],&nastr,2,'|') != 0)
   continue;  

  if(metka_pods == 1)
   {
    nvd->muvd.plus(viddok.ravno());
    nvd->muso.plus(nastr.ravno());
   }
  if(metka_pods == 2)
   {
    nvd->uuvd.plus(viddok.ravno());
    nvd->uuso.plus(nastr.ravno());
   }
  if(metka_pods == 3)
   {
    nvd->uosvd.plus(viddok.ravno());
    nvd->uosso.plus(nastr.ravno());
   }
  if(metka_pods == 4)
   {
    nvd->ukrvd.plus(viddok.ravno());
    nvd->ukrsr.plus(nastr.ravno());
   }
 }


return(0);
}

