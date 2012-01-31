/*$Id: vregp.c,v 1.10 2011-02-21 07:35:58 sasa Exp $*/
/*26.03.2008	05.01.2004	Белых А.И.	vregp.c
Выбор режима просмотра
*/
#include <unistd.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "prov_poi.h"


int vregp_m(GtkWidget *wpredok);

extern char *name_system;
//extern prov_poi_data rekv_m_poiprov; //Реквизиты меню для поиска проводок

void vregp(GtkWidget *wpredok)
{
class prov_poi_data rekv_m_poiprov;

int ktoz=0;
time_t vremn=0;

int nomer=0;

if((nomer=vregp_m(wpredok)) < 0)
 return;

iceb_u_str repl;
repl.plus(gettext("Только проводки сделанные вами ?"));
int metka;
metka=iceb_menu_danet(&repl,2,wpredok);

if(metka == 1)
 ktoz=iceb_getuid(wpredok);
 
time(&vremn);

if(nomer == 0)
  vremn-=2*60;
if(nomer == 1)
  vremn-=10*60;
if(nomer == 2)
  vremn-=60*60;

if(nomer == 3)
 {
  time_t tmm;
  struct tm *bft;
  time(&tmm);
  bft=localtime(&tmm);
    
  struct tm bf;
  memset(&bf,'\0',sizeof(bf));
  bf.tm_mday=bft->tm_mday;
  bf.tm_mon=bft->tm_mon;
  bf.tm_year=bft->tm_year;
//  bf.tm_hour=(int)0;
//  bf.tm_min=0;
//  bf.tm_sec=0;

  vremn=mktime(&bf);
 }
char strsql[100];
if(ktoz != 0)
 {
  sprintf(strsql,"select * from Prov where vrem >= %ld \
  and ktoi=%d order by vrem asc",vremn,ktoz);
 }    
else
 sprintf(strsql,"select * from Prov where vrem >= %ld \
  order by vrem asc",vremn);

rekv_m_poiprov.zapros.new_plus(strsql);

l_prov(&rekv_m_poiprov,wpredok);

}

/*****************************/
/*Меню выбора*/
/****************************/
int vregp_m(GtkWidget *wpredok)
{


iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Поиск"));


zagolovok.plus(gettext("Выберите период, за который хотите увидеть проводки"));
zagolovok.ps_plus(gettext("Поиск будет выполняться по времени записи проводок"));


punkt_m.plus(gettext("За последних две минуты"));
punkt_m.plus(gettext("За последних десять минут"));
punkt_m.plus(gettext("За последний час"));
punkt_m.plus(gettext("За текущий день"));


int nomer=0;
return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL));



}
