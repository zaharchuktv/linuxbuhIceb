/*$Id: i_xdk.c,v 1.51 2011-06-07 08:52:26 sasa Exp $*/
/*06.09.2010	14.03.2006	Белых А.И.	i_xdk.c
Программа работы с платёжными документами и платёжными требованиями
*/
#define DVERSIQ "03.06.2011"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include        <pwd.h>
#include        <unistd.h>
#include        <sys/stat.h>
#include <string.h>
#include        <locale.h>
#include        "buhg_g.h"


void i_xdk_platdok(const char *tablica);
int nastdkw(void);
int vpld(iceb_u_str *datad,iceb_u_str *nomdok,const char *tablica,GtkWidget *wpredok);
void l_xdkdoks(short ds,short ms,short gs,const char *tablica,GtkWidget *wpredok);
void l_tippl(const char *tablica,int metka,GtkWidget *wpredok);
void xdksoznkom(char *imaf,GtkWidget *wpredok);
void poixdkdokw(const char *tablica,GtkWidget *wpredok);
void xdkgur(const char *tablica);
void xdksend(int metka,const char *tablica,GtkWidget *wpredok);
int poigod_xdk(const char *tablica,GtkWidget *wpredok);
void xdkudgod(void);
void dok_doverw(void);

extern SQL_baza	bd;
extern char		*organ;
extern char		*host;
const char *name_system={NAME_SYSTEM};
const char *version={VERSION};
extern char		*parol;
extern iceb_u_str shrift_rek_raz;
char *imafkomtmp=NULL;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
iceb_kod_podsystem=ICEB_PS_PD;
int		kom=0;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);
if(iceb_prr(NULL) != 0) /*проверяем разрешено ли оператору работать с подсистемой*/
 iceb_exit(0);

if(iceb_nastsys() != 0)
 iceb_exit(1);

if(nastdkw() != 0)
 iceb_exit(1);

if(iceb_perzap(1,NULL))
 iceb_exit(1);
nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(organ);
iceb_menu_mv_str2.new_plus(gettext("Платёжные документы"));

char bros[512];
sprintf(bros,"pl%d.tmp",getpid());
imafkomtmp=new char[strlen(bros)+1];
strcpy(imafkomtmp,bros);

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с платёжными поручениями"));
menu_str.plus(gettext("Работа с платёжными требованиями"));
menu_str.plus(gettext("Работа с файлом настройки"));
menu_str.plus(gettext("Удаление документов за год"));
menu_str.plus(gettext("Выписка доверенностей"));
menu_str.plus(gettext("Работа с файлами распечаток"));
menu_str.plus(gettext("Выбор шрифта"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Платёжные документы"),fioop.ravno(),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        if(iceb_nastsys() != 0)
         iceb_exit(1);
        if(nastdkw() != 0)
          iceb_exit(1);
       }
      break;
    case 1:
      i_xdk_platdok("Pltp");
      break;

    case 2:
      i_xdk_platdok("Pltt");
      break;
    
    case 3:
      if(iceb_f_redfil("nastdok.alx",0,NULL) == 0)
      nastdkw();
      break;

    case 4:
      xdkudgod();
      break;

    case 5:
      dok_doverw();
      break;

    case 6:
      iceb_l_rsfr(NULL);
      break;

    case 7:
      iceb_font_selection(NULL);
      break;

  }
 }
//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}

/*****************************************/
/*Работа с платёжными документами*/
/********************************/

void i_xdk_platdok(const char *tablica)
{
char strsql[512];
iceb_u_str stroka;
int nomer=0;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
  titl.plus(gettext("Работа с платёжными поручениями"));
if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
  titl.plus(gettext("Работа с платёжными требованиями"));


if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
  stroka.plus(gettext("Работа с платёжными поручениями"));
if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
  stroka.plus(gettext("Работа с платёжными требованиями"));

zagolovok.plus(stroka.ravno());

if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
 {
  punkt_m.plus(gettext("Ввод нового платёжного поручения"));//0
  punkt_m.plus(gettext("Ввод с использованием типового платёжного поручения"));//1
  punkt_m.plus(gettext("Поиск платёжного поручения"));//2
  punkt_m.plus(gettext("Просмотр журнала платёжных поручений"));//3

  sprintf(strsql,"%s %d.%d.%d",gettext("Просмотр журнала платёжных поручений за"),dt,mt,gt);
  punkt_m.plus(strsql); //4
  
  punkt_m.plus(gettext("Распечатка журнала платёжных поручений"));//5

  punkt_m.plus(gettext("Ввод и корректировка контрагентов в файл"));//6

  punkt_m.plus(gettext("Ввод и корректировка контрагентов в базу"));//7

  punkt_m.plus(gettext("Ввод и корректировка операций"));//8
  punkt_m.plus(gettext("Ввод и корректировка типовых платёжных поручений"));//9

  punkt_m.plus(gettext("Переслать файл платёжек для передачи в банк"));//10

  punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//11

  punkt_m.plus(gettext("Распечатать список годов за которые введена информация"));//12

  punkt_m.plus(gettext("Настройка автоматического выполнения проводок"));//13 

  punkt_m.plus(gettext("Настройка списка банков для передачи документов"));//14

  punkt_m.plus(gettext("Записать файл платёжек (для импорта) на DOS дискету"));//15
  
 }

if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
 {
  punkt_m.plus(gettext("Ввод нового платёжного требования"));//0
  punkt_m.plus(gettext("Ввод с использованием типового платёжного требования"));//1
  punkt_m.plus(gettext("Поиск платёжного требования"));//2
  punkt_m.plus(gettext("Просмотр журнала платёжных требований"));//3

  sprintf(strsql,"%s %d.%d.%d",gettext("Просмотр журнала платёжных требований за"),dt,mt,gt);
  punkt_m.plus(strsql); //4
  
  punkt_m.plus(gettext("Распечатка журнала платёжных требований"));//5

  punkt_m.plus(gettext("Ввод и корректировка контрагентов в файл"));//6

  punkt_m.plus(gettext("Ввод и корректировка контрагентов в базу"));//7

  punkt_m.plus(gettext("Ввод и корректировка операций"));//8
  punkt_m.plus(gettext("Ввод и корректировка типовых платёжных требований"));//9

  punkt_m.plus(gettext("Переслать файл платёжек для передачи в банк"));//10

  punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//11

  punkt_m.plus(gettext("Распечатать список годов за которые введена информация"));//12

  punkt_m.plus(gettext("Настройка автоматического выполнения проводок"));//13 

  punkt_m.plus(gettext("Настройка списка банков для передачи документов"));//14

  punkt_m.plus(gettext("Записать файл платёжек (для импорта) на DOS дискету"));//15
  
 }
iceb_u_str datad;
iceb_u_str kod;
iceb_u_str naikod;
kod.plus("");
naikod.plus("");
datad.plus("");

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      break; //в конце удаляется промежуточный файл
    case 0:
      if(vpld(&datad,&kod,tablica,NULL) == 0)
       {
        xdkdok(tablica,NULL);
       }
      break;

    case 1:
      if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
        l_tippl("Ktnp",1,NULL);
      if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
        l_tippl("Ktnt",1,NULL);
      break;

    case 2:
      poixdkdokw(tablica,NULL);
      break;

    case 3:
      l_xdkdoks(0,0,0,tablica,NULL);
      break;

    case 4:
      l_xdkdoks(dt,mt,gt,tablica,NULL);
      break;

    case 5:
      xdkgur(tablica);
      break;

    case 6:
      iceb_f_redfil("platpor.alx",0,NULL);
      break;

    case 7:
      iceb_l_kontr(0,&kod,&naikod,NULL);
      break;

    case 8:
      l_xdkop(0,&kod,&naikod,tablica,NULL);
      break;

    case 9:
      if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
        l_tippl("Ktnp",0,NULL);
      if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
        l_tippl("Ktnt",0,NULL);
      break;

    case 10:
      xdksend(0,tablica,NULL);
      break;

    case 11:
      iceb_l_blok(NULL);
      break;

    case 12:
      poigod_xdk(tablica,NULL);
      break;
      
    case 13:
      if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
         iceb_f_redfil("avtprodok.alx",0,NULL);
      if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
         iceb_f_redfil("avtprodokt.alx",0,NULL);
      break;

    case 14 :
      iceb_f_redfil("banki.alx",0,NULL);
      break;

    case 15:
      xdksend(1,tablica,NULL);
      break;

   }
   
 }

unlink(imafkomtmp);

}
