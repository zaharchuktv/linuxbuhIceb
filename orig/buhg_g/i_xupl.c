/*$Id: i_xupl.c,v 1.22 2011-02-21 07:35:51 sasa Exp $*/
/*13.10.2010	29.02.2008	Белых А.И.	i_xupl.c
Подсистема "Учёт путевых листов"
*/

#define DVERSIQ "13.10.2010"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include        <pwd.h>
#include        <unistd.h>
#include        <sys/stat.h>
#include <string.h>
#include        <locale.h>
#include        "buhg_g.h"

int uplnastw();
void i_xupl_nsi(GtkWidget *wpredok);
void i_xupl_dok(GtkWidget *wpredok);
void l_uplspdok(short ds,short ms,short gs,GtkWidget *wpredok);
void i_xupl_ro(GtkWidget *wpredok);
void poiupldokw(GtkWidget *wpredok);
void upl_plc_r(GtkWidget *wpredok);
void upl_pvod();
void upl_pavt();
void upl_rpavt();
void upl_shvod();
void upl_shkg();
void upl_ovdt();
void saldouplw();

extern SQL_baza	bd;
extern char		*organ;
extern char		*host ;
const char *name_system={NAME_SYSTEM};
const char  *version={VERSION};
extern char		*parol;
extern iceb_u_str shrift_rek_raz;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
int		kom=0;
iceb_kod_podsystem=ICEB_PS_UPL;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);

if(iceb_nastsys() != 0)
 iceb_exit(1);

if(uplnastw() != 0)
 iceb_exit(1);

if(iceb_perzap(1,NULL))
 iceb_exit(1);
nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(organ);
iceb_menu_mv_str2.new_plus(gettext("Учёт путевых листов"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с документами"));
menu_str.plus(gettext("Ввод нормативно-справочной информации"));
menu_str.plus(gettext("Распечатка отчётов"));
menu_str.plus(gettext("Работа с файлами распечаток"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Учёт путевых листов"),fioop.ravno(),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        if(iceb_nastsys() != 0)
         iceb_exit(1);
        if(uplnastw() != 0)
          iceb_exit(1);
       }
      break;

    case 1:
      i_xupl_dok(NULL);
      break;


    case 2:
      i_xupl_nsi(NULL);
      break;

    case 3:
      i_xupl_ro(NULL);
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

/***********************/
/*Ввод нормативно-справочной информации*/
/**************************************/
void i_xupl_nsi(GtkWidget *wpredok)
{
iceb_u_str stroka;
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

stroka.plus(gettext("Ввод и корректировка НСИ"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Работа со списком подразделений"));//0
punkt_m.plus(gettext("Работа со списком марок топлива"));//1
punkt_m.plus(gettext("Работа со списком объектов учёта остатков топлива"));//2
punkt_m.plus(gettext("Работа со списком автомобилей"));//3
punkt_m.plus(gettext("Работа со стартовым сальдо"));//4
punkt_m.plus(gettext("Работа со списком групп объектов списания топлива"));//5
punkt_m.plus(gettext("Работа со списком объектов списания топлива"));//6
punkt_m.plus(gettext("Работа с файлом настройки"));//7
punkt_m.plus(gettext("Работа со справочником средних цен на топливо"));//8
punkt_m.plus(gettext("Выбор шрифта"));//9
punkt_m.plus(gettext("Перенос стартовых остатков топлива"));//10



while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);



  iceb_u_str kod("");
  iceb_u_str naim("");
  
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      l_uplpod(0,&kod,&naim,NULL);
      break;

    case 1:
      l_uplmt(0,&kod,&naim,NULL);
      break;

    case 2:
      l_uplout(0,&kod,&naim,0,NULL);
      break;

    case 3:
      l_uplavt(0,&kod,&naim,NULL);
      break;

    case 4:
      l_uplsal(NULL);
      break;

    case 5:
      l_uplgo(0,&kod,&naim,NULL);
      break;

    case 6:
      l_uplost(0,&kod,&naim,NULL);
      break;

    case 7:
      if(iceb_f_redfil("uplnast.alx",0,NULL) == 0)
       uplnastw();
      break;

    case 8:
      l_uplscnt(NULL);
      break;

    case 9:
      iceb_font_selection(NULL);
      break;

    case 10:
      saldouplw();
      break;


   }
   
 }
}
/***********************/
/*Работа в документами*/
/**************************************/
void i_xupl_dok(GtkWidget *wpredok)
{
char       strsql[512];
iceb_u_str stroka;
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

titl.plus(gettext("Работа с документами"));

stroka.plus(gettext("Работа с документами"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Ввод нового путевого листа"));//0
punkt_m.plus(gettext("Поиск введённого документа"));//1
punkt_m.plus(gettext("Просмотр всех введённых документов"));//2

sprintf(strsql,"%s %02d.%02d.%d%s",gettext("Просмотр документов начиная с"),
dt,mt,gt,
gettext("г."));
punkt_m.plus(strsql);//3

punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//4
punkt_m.plus(gettext("Проверка логической целосности базы данных"));//5


while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);



  iceb_u_str data_dok("");
  iceb_u_str kod_pod("");
  iceb_u_str nomdok("");
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      if(l_upldok_v(&data_dok,&kod_pod,&nomdok,NULL) == 0)
       l_upldok(data_dok.ravno(),nomdok.ravno(),kod_pod.ravno(),NULL);
      break;

    case 1:
      poiupldokw(NULL);
      break;
      
    case 2:
      l_uplspdok(0,0,0,NULL);
      break;


    case 3:
      l_uplspdok(dt,mt,gt,NULL);
      break;

    case 4:
//      iceb_f_redfil("uplblok.alx",0,NULL);
      iceb_l_blok(NULL);
      break;

    case 5:
      upl_plc_r(NULL);
      break;

   }
   
 }
}
/*********************/
/*Распечатка отчётов*/
/*********************/
void i_xupl_ro(GtkWidget *wpredok)
{
iceb_u_str stroka;
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

titl.plus(gettext("Распечатка отчётов"));

stroka.plus(gettext("Распечатка отчётов"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Распечатка реестра путевых листов по водителям"));//0
punkt_m.plus(gettext("Распечатка реестра путевых листов по автомобилям"));//1
punkt_m.plus(gettext("Распечатка оборотной ведомости движения топлива по автомобилям"));//2
punkt_m.plus(gettext("Распечатка ведомости списания топлива по водителям и счетам"));//3
punkt_m.plus(gettext("Распечатка ведомости списания топлива по счетам в киллограммах"));//4
punkt_m.plus(gettext("Распечатка оборотной ведомости движения топлива"));//5


while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);



  switch (nomer)
   {
    case -1:
      return;

    case 0:
      upl_pvod();
      break;

    case 1:
      upl_rpavt();
      break;

    case 2:
      upl_pavt();
      break;

    case 3:
      upl_shvod();
      break;

    case 4:
      upl_shkg();
      break;

    case 5:
      upl_ovdt();
      break;


   }
   
 }
}
