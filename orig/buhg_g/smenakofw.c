/*$Id: smenakofw.c,v 1.7 2011-02-21 07:35:57 sasa Exp $*/
/*19.10.2007	19.10.2007	Белых А.И.	smenakofw.c
Программа смены поправочных коэффициентов в карточках основных средств
для налогового и бухгалтерского учёта
*/
#include <stdlib.h>
#include <unistd.h>
#include "buhg_g.h"
#include "smenakofw.h"

int   smenakofw_m(class smenakofw_data *rek_ras,GtkWidget *wpredok);
extern SQL_baza bd;

void smenakofw(int metka_uh) //0-налоговый учёт 1-бухгалтерский
{
static class smenakofw_data data;
data.metka_uh=metka_uh;


if(smenakofw_m(&data,NULL) != 0)
 return;

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,NULL);
short mn,gn;
iceb_u_rsdat1(&mn,&gn,data.datak.ravno());

char strsql[512];
int kolstr=0;
SQL_str row;

sprintf(strsql,"select innom,naim from Uosin");
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),NULL);
  return;
 }

float kolstr1=0.;
time_t vrem=time(NULL);

float popkf,popkfby;

int in;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
//  kolstr1++;
//  strzag(LINES-1,0,kolstr,kolstr1);
  in=atol(row[0]);
  class poiinpdw_data rekin;
  if(poiinpdw(in,mn,gn,&rekin,NULL) != 0)
    continue;  

  if(data.metka_uh == 0)
   if(iceb_u_SRAV(data.kod_gr.ravno(),rekin.hna.ravno(),0) != 0)
    continue;

  if(data.metka_uh == 1)
   if(iceb_u_SRAV(data.kod_gr.ravno(),rekin.hnaby.ravno(),0) != 0)
    continue;

//  printw("%ld %s\n",row[0],row[1]);    

  if(data.metka_uh == 0)
   {
    popkfby=rekin.popkfby;
    popkf=data.kof.ravno_atof();
   }
  else
   {
    popkfby=data.kof.ravno_atof();
    popkf=rekin.popkf;    
   }
 
  sprintf(strsql,"replace into Uosinp \
values (%d,%d,%d,'%s','%s','%s','%s',%.6g,%d,'%s',%d,%ld,'%s',%.10g)",
  in,mn,gn,rekin.shetu.ravno(),rekin.hzt.ravno(),rekin.hau.ravno(),rekin.hna.ravno(),popkf,rekin.soso,
  rekin.nomz.ravno(),iceb_getuid(NULL),vrem,rekin.hnaby.ravno(),popkfby);
 
  iceb_sql_zapis(strsql,1,0,NULL);
      
 }

}







