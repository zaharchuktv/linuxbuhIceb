/*$Id: i_xrnn.c,v 1.31 2011-08-29 07:13:43 sasa Exp $*/
/*07.12.2010	14.04.2008	Белых А.И.	i_xrnn.c
Реестр налоговых накладных
*/

#define DVERSIQ "18.08.2011"

#include        "buhg_g.h"

int ree_nastw();
void l_xrnnp(short dnp,short mnp,short gnp,GtkWidget *wpredok);
void l_xrnnv(short dnp,short mnp,short gnp,GtkWidget *wpredok);
void xrnn_rasp();
void xrnn_uzzp(GtkWidget*);
void i_xrnn_rsd();
void i_xrnn_nsi();

extern SQL_baza	bd;
extern char		*organ;
extern char		*host ;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern char		*parol;
extern iceb_u_str shrift_rek_raz;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern short dp,mp,gp; //Дата начала просмотра реестров
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
int		kom=0;
iceb_kod_podsystem=ICEB_PS_RNN;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);

if(iceb_nastsys() != 0)
 iceb_exit(1);

if(ree_nastw() != 0)
 iceb_exit(1);

if(iceb_perzap(1,NULL))
 iceb_exit(1);

nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(organ);
iceb_menu_mv_str2.new_plus(gettext("Материальный учёт"));

iceb_menu_mv_str1.new_plus(organ);
iceb_menu_mv_str2.new_plus(gettext("Реестр налоговых накладных"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с рееcтрами"));
menu_str.plus(gettext("Распечатать реестр налоговых накладных"));
menu_str.plus(gettext("Ввод нормативно-справочной информации"));
menu_str.plus(gettext("Работа с файлами распечаток"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Реестр налоговых накладных"),fioop.ravno(),nom_op,&menu_str,"white");
  class iceb_u_str kod("");
  class iceb_u_str naim("");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        if(iceb_nastsys() != 0)
         iceb_exit(1);
        if(ree_nastw() != 0)
          iceb_exit(1);
       }
      break;
    case 1:
      i_xrnn_rsd();
      break;

    case 2:
      xrnn_rasp();
      break;

    case 3:
      i_xrnn_nsi();
      break;

    case 4:
      iceb_l_rsfr(NULL);
      break;
  }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}
/********************************************************/
/*Ввод и корректировка нормативно-справочной информации*/
/********************************************************/
void i_xrnn_nsi()
{
int nomer=0;
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

zagolovok.plus(gettext("Ввод и корректировка НСИ"));

punkt_m.plus(gettext("Ввод и корректировка списка групп для приходных накладных"));
punkt_m.plus(gettext("Ввод и корректировка списка групп для расходных накладных"));
punkt_m.plus(gettext("Настройка определения вида приходных накладных"));
punkt_m.plus(gettext("Настройка определения вида расходных накладных"));
punkt_m.plus(gettext("Работа с файлом настройки"));
punkt_m.plus(gettext("Настройка импорта полученных накладных"));
punkt_m.plus(gettext("Выбор шрифта"));

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  class iceb_u_str kod("");
  class iceb_u_str naim("");  

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      l_xrnngpn(0,&kod,&naim,NULL);
      break;

    case 1:
      l_xrnngvn(0,&kod,&naim,NULL);
      break;

    case 2:
      iceb_f_redfil("rnnovdp.alx",0,NULL);
      break;

    case 3:
      iceb_f_redfil("rnnovdr.alx",0,NULL);
      break;

    case 4:
      if(iceb_f_redfil("rnn_nast.alx",0,NULL) == 0)
        ree_nastw();
      break;

    case 5:
      iceb_f_redfil("rnn_nimp1.alx",0,NULL);
      break;

    case 6:
      iceb_font_selection(NULL);
      break;
   }
 }   
}
/*******************************/
/*Работа с реестрами*/
/*******************************/
void i_xrnn_rsd()
{
char bros[512];
int nomer=0;
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;
short dt,mt,gt;

iceb_u_poltekdat(&dt,&mt,&gt);
iceb_u_dpm(&dt,&mt,&gt,4); /*Уменьшить на месяц*/
titl.plus(gettext("Работа с рееcтрами"));


zagolovok.plus(gettext("Работа с рееcтрами"));

punkt_m.plus(gettext("Реестр полученных налоговых накладных"));

sprintf(bros,"%s %02d.%04d %s",gettext("Реестр полученных налоговых накладных"),mt,gt,gettext("г."));
punkt_m.plus(bros);

punkt_m.plus(gettext("Реестр выданных налоговых накладных"));

sprintf(bros,"%s %02d.%04d %s",gettext("Реестр выданных налоговых накладных"),mt,gt,gettext("г."));
punkt_m.plus(bros);

punkt_m.plus(gettext("Установить дату начала просмотра реестров"));
punkt_m.plus(gettext("Устанвливать/снимать блокировку дат"));
punkt_m.plus(gettext("Удалить записи за период"));

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  class iceb_u_str kod("");

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      l_xrnnp(dp,mp,gp,NULL);
      break;

    case 1:
      l_xrnnp(1,mt,gt,NULL);
      break;

    case 2:
      l_xrnnv(dp,mp,gp,NULL);
      break;

    case 3:
      l_xrnnv(1,mt,gt,NULL);
      break;

    case 4:
      if(dp != 0)
       {
        char bros[20];
        sprintf(bros,"%d.%d.%d",dp,mp,gp);
        kod.new_plus(bros);
       }
      if(iceb_menu_vvod1(gettext("Введите дату начала просмотра (д.м.г)"),&kod,11,NULL) == 0)
       {
        short d=0,m=0,g=0;
        if(iceb_u_rsdat(&d,&m,&g,kod.ravno(),1) != 0)
         {
          iceb_menu_soob(gettext("Неправильно ввели дату!"),NULL);
         }
        else
         {
          dp=d;
          mp=m;
          gp=g;
         }
       }
      break;


    case 5:
//      iceb_f_redfil("rnn_blok.alx",0,NULL);
      iceb_l_blok(NULL);
      break;

    case 6:
      xrnn_uzzp(NULL);
      break;
   }
 }


}
