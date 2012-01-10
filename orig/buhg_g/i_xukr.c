/*$Id: i_xukr.c,v 1.30 2011-07-06 08:12:30 sasa Exp $*/
/*06.09.2010	18.02.2008	Белых А.И.	i_xukr.c
Подсистема "Учёт командировочных расходов"
*/

#define DVERSIQ "09.11.2011"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include        <pwd.h>
#include        <unistd.h>
#include        <sys/stat.h>
#include <string.h>
#include        <locale.h>
#include        "buhg_g.h"


int ukrnastw();
void i_xukr_nsi(GtkWidget *wpredok);
void poiukrdokw(GtkWidget *wpredok);
void i_xukr_rd();
void i_xukr_ro();
void ukrrpw();
void ukrkontr();
void ukrprik();
void ukrshkz();
void ukrspdokw();
void ukrspdok1w();

extern SQL_baza	bd;
extern char		*organ;
extern char		*host ;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern char		*parol;
extern iceb_u_str shrift_rek_raz;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
int		kom=0;
iceb_kod_podsystem=ICEB_PS_UKR;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);

if(iceb_nastsys() != 0)
 iceb_exit(1);

if(ukrnastw() != 0)
 iceb_exit(1);

if(iceb_perzap(1,NULL))
 iceb_exit(1);

nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(organ);
iceb_menu_mv_str2.new_plus(gettext("Учёт командировочных расходов"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с документами"));
menu_str.plus(gettext("Ввод нормативно-справочной информации"));
menu_str.plus(gettext("Распечатка отчётов"));
menu_str.plus(gettext("Работа с файлами распечаток"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Учёт командировочных расходов"),fioop.ravno(),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        if(iceb_nastsys() != 0)
         iceb_exit(1);
        if(ukrnastw() != 0)
          iceb_exit(1);
       }
      break;

    case 1:
      i_xukr_rd();
      break;


    case 2:
      i_xukr_nsi(NULL);
      break;

    case 3:
      i_xukr_ro();
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

/**********************************/
/*Ввод нормативно-справочной информации*/
/**************************************/
void i_xukr_nsi(GtkWidget *wpredok)
{
char       strsql[512];
iceb_u_str stroka;
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

memset(strsql,'\0',sizeof(strsql));
stroka.plus(gettext("Ввод и корректировка НСИ"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Ввод и корректировка групп затрат на командировки"));//0
punkt_m.plus(gettext("Ввод и корректировка списка единиц измерения"));//1
punkt_m.plus(gettext("Ввод и корректировка командировочных расходов"));//2
punkt_m.plus(gettext("Ввод и корректировка видов командировочных расходов"));//3
punkt_m.plus(gettext("Ввод и корректировка списка контрагентов"));//4
punkt_m.plus(gettext("Работа с файлом настройки"));//5
punkt_m.plus(gettext("Настройка автоматического выполнения проводок"));//6
punkt_m.plus(gettext("Выбор шрифта"));//7

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
      l_ukrgrup(0,&kod,&naim,NULL);
      break;

    case 1:
      iceb_l_ei(0,&kod,&naim,NULL);
      break;

    case 2:
      l_ukrzat(0,&kod,&naim,NULL);
      break;

    case 3:
      l_ukrvk(0,&kod,&naim,NULL);
      break;

    case 4:
      iceb_l_kontr(0,&kod,&naim,NULL);
      break;

    case 5:
      if(iceb_f_redfil("ukrnast.alx",0,NULL) == 0)
       ukrnastw();
      break;

    case 6:
      iceb_f_redfil("avtprukr.alx",0,NULL);
      break;

    case 7:
      iceb_font_selection(NULL);
      break;



   }
   
 }

}
/****************************/
/*Работа с документами*/
/**************************/

void i_xukr_rd()
{
char strsql[512];
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Работа с документами"));


zagolovok.plus(gettext("Работа с документами"));


punkt_m.plus(gettext("Ввод нового документа"));//0
punkt_m.plus(gettext("Поиск введённого документа"));//1
punkt_m.plus(gettext("Просмотр введённых документов"));//2

sprintf(strsql,"%s %02d.%02d.%d%s",gettext("Просмотр документов начиная с"),
dt,mt,gt,
gettext("г."));
punkt_m.plus(strsql);//3

punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//4

int nomer=0;
while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  class iceb_u_str datadok("");
  class iceb_u_str nomdok("");
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      if(ukrvhd(&datadok,&nomdok,NULL) == 0)
        l_ukrdok(datadok.ravno(),nomdok.ravno(),NULL);
        
      break;

    case 1:
      poiukrdokw(NULL);
      break;
      
    case 2:
      l_ukrspdok(0,0,0,NULL);      
      break;

    case 3:
      l_ukrspdok(dt,mt,gt,NULL);      
      break;

    case 4:
//      iceb_f_redfil("ukrblok.alx",0,NULL);
      iceb_l_blok(NULL);
      break;
   }
 }
}
/****************************/
/*Отчёты*/
/**************************/

void i_xukr_ro()
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка отчётов"));


zagolovok.plus(gettext("Распечака отчётов"));


punkt_m.plus(gettext("Распечатать реестр проводок по видам командировок"));//0
punkt_m.plus(gettext("Распечатать ведомость по контрагентам"));//1
punkt_m.plus(gettext("Распечатать приказы на командировки"));//2
punkt_m.plus(gettext("Распечатать свод по счетам-кодам расходов"));//3
punkt_m.plus(gettext("Распечатать список документов на командировки"));//4
punkt_m.plus(gettext("Распечатать список документов по датам приказов"));//5

int nomer=0;
while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      ukrrpw();
      break;

    case 1:
      ukrkontr();
      break;

    case 2:
      ukrprik();
      break;

    case 3:
      ukrshkz();
      break;

    case 4:
      ukrspdokw();
      break;

    case 5:
      ukrspdok1w();
      break;
   }
 }
}
