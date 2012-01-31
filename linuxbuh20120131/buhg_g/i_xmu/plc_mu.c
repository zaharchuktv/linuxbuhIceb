/*$Id: plc_mu.c,v 1.8 2011-02-21 07:35:55 sasa Exp $*/
/*18.11.2004	18.11.2004	Белых А.И.	plc_mu.c
Проверка логической целосности базы
*/
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

int plc_mu_r(int metka,iceb_u_str*,GtkWidget *wpredok);


void plc_mu()
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Проверка логичной целосности базы"));//0
punkt_m.plus(gettext("Проверка логической целосности базы и удаление нелогичных записей"));//1


int nomer=0;
if((nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL)) == -1)
 return;

iceb_u_str imaf_p;

if(plc_mu_r(nomer,&imaf_p,NULL) != 0)
 {
  iceb_u_spisok imaf_s;
  iceb_u_spisok naim_s;
  
  imaf_s.plus(imaf_p.ravno());
  naim_s.plus(gettext("Протокол ошибок"));
  
  iceb_rabfil(&imaf_s,&naim_s,"",0,NULL);
  
 }


}
