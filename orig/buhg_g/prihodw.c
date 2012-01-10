/* $Id: prihodw.c,v 1.11 2011-02-21 07:35:56 sasa Exp $ */
/*18.02.2010    20.01.1993      Белых А.И.      prihodw.c
Ввод приходов в уже существующие карточки материалов
Возвращяем 0 если не записали
	   1 записали
*/
#include	"buhg_g.h"

int prihod_m(class iceb_u_str *kolih,class iceb_u_str *shet_pol,GtkWidget *wpredok);

extern SQL_baza  bd;

int prihodw(short dd,short md,short gd,int skl,const char *nomd,
int km, //Код материалла
int nk, //Номер карточки
double cena,const char *ei,int mnds,double ndsg,
const char *nomz, //Номер заказа
GtkWidget *wpredok)
{
/***************
iceb_u_str zapros;
zapros.plus(gettext("Введите количество"));



iceb_u_str kolih;
     
if(iceb_menu_vvod1(&zapros,&kolih,20,wpredok) != 0)
  return(0);
******************/
class iceb_u_str kolih("");
class iceb_u_str shet_pol("");

naz:;
if(prihod_m(&kolih,&shet_pol,wpredok) != 0)
 return(0);

if(kolih.ravno_atof() == 0.)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не введено количество"));
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);
  goto naz;
 }

if(kolih.ravno_atof() < 0.)
 {
  short dt,mt,gt;
  class ostatok ostvkar;
  iceb_u_poltekdat(&dt,&mt,&gt);
  ostkarw(1,1,gt,dt,mt,gt,skl,nk,&ostvkar);
  if(ostvkar.ostg[3]+kolih.ravno_atof() < -0.0001)
   {
    char strsql[512];    
    sprintf(strsql,"%s!",gettext("Отрицательный остаток"));
    iceb_menu_soob(strsql,wpredok);
    return(0);
   }  
 }

zapvdokw(dd,md,gd,skl,nk,km,nomd,kolih.ravno_atof(),cena,ei,ndsg,mnds,0,1,0,shet_pol.ravno(),nomz,wpredok);

return(1);
}
