/*$Id: i_xbu.c,v 1.157 2011-08-29 07:13:43 sasa Exp $*/
/*04.12.2010	12.08.2003	Белых А.И.	i_xbu.c
Главная книга
*/
#define  DVERSIQ "31.01.2012"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <locale.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
#include "go.h"
//#include "../headers/go.h"
#include "pvbank.h"
//#include "../headers/pvbank.h"
#include "pvbanks.h"
//#include "../headers/pvbanks.h"
//ze


void    xbug_nsi(void);
void    xbug_rsp(void);
void	glmenu(int*);
int     buhnast_g(void);
void    l_saldo_sh(GtkWidget *wpredok);
void    l_saldo_shk(GtkWidget *wpredok);
void    vibshet(int);
void    poiprovw();
int     plc_gk(void);
void    xburo(void);
void    glkniw(void);
void    glkni_nbs_m(void);
void    rspvk_m(void);
int     go_m(class go_rr*);
int     go_ss_nbs(class go_rr*);
int     go_srv_nbs(class go_rr*);
int     go_srk(class go_rr*,int);
int     vibor_ss(void);
int     vibor_sr(void);
void    vibkontr(void);
int     pvbank_m(class pvbank_rr *rek_ras);
int     pvbanks_m(class pvbanks_rr *rek_ras);
int     pvbanks_r(class pvbanks_rr *rek_ras);
void    pvbank_r(class pvbank_rr *rek_ras);
void    clprov_m(void);
void    buhkomw(void);
void buhhahw(void);
void buhsdkrw(void);
void buhrpzw(void);
void buhrpznpw(void);
void buhgspw(void);
void buhvdzw(void);
void buhspkw(void);
void buhpsdw(void);
int gosssw_r(class go_rr *datark,GtkWidget *wpredok);
int gosrsw_r(class go_rr *datark,int metkasort,GtkWidget *wpredok);
int vibor_sosrt();
void saldogkw(GtkWidget*);

extern SQL_baza	bd;
extern char		*organ;
extern char		*host;
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
iceb_kod_podsystem=ICEB_PS_GK;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);
 
if(iceb_nastsys() != 0)
 iceb_exit(1);
if(buhnast_g() != 0)
 iceb_exit(1);
if(iceb_perzap(1,NULL))
 iceb_exit(1);

nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(organ);
iceb_menu_mv_str2.new_plus(gettext("Главная бухгалтерская книга"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с проводками"));
menu_str.plus(gettext("Ввод нормативно-справочной информации"));
menu_str.plus(gettext("Распечатать отчеты"));
menu_str.plus(gettext("Работа с файлами распечаток"));
menu_str.plus(gettext("Удалить проводки"));


while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Главная бухгалтерская книга"),fioop.ravno(),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        if(iceb_nastsys() != 0)
         iceb_exit(1);
        if(buhnast_g() != 0)
          iceb_exit(1);
       }
      break;

    case 1:
      xbug_rsp();
      break;

    case 2:
      xbug_nsi();
      break;

    case 3:
      xburo();
      break;

    case 4:
      iceb_l_rsfr(NULL);
      break;

    case 5:
      clprov_m();
      break;


   }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}

/****************************************/
/*Ввод нормативно-справочной информации*/
/****************************************/

void    xbug_nsi(void)
{
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

zagolovok.plus(gettext("Ввод и корректировка НСИ"));

punkt_m.plus(gettext("Ввод и корректировка плана счетов"));//0
punkt_m.plus(gettext("Ввод и корректировка списка контрагентов"));//1
punkt_m.plus(gettext("Ввод и корректировка списка контрагентов для счета"));//2
punkt_m.plus(gettext("Ввод и корректировка стартового сальдо по счетам"));//3
punkt_m.plus(gettext("Ввод и корректировка стартового сальдо по контрагентам"));//4
punkt_m.plus(gettext("Работа с файлом настройки"));//5
punkt_m.plus(gettext("Ввод и корректировка списка групп контрагентов"));//6
punkt_m.plus(gettext("Ввод и корректировка списка суббалансов"));//7
punkt_m.plus(gettext("Выбор шрифта"));//8
punkt_m.plus(gettext("Перенос сальдо"));//9
punkt_m.plus(gettext("Настройка списка банков для перечисления средств на картсчета"));//10


while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);



  class iceb_u_str kod("");
  class iceb_u_str naikod("");
  char shetsk[20];
  memset(shetsk,'\0',sizeof(shetsk));
  class iceb_u_str repl;
  OPSHET rekshet;
  
  switch (nomer)
   {
    case -1:
      return;
      
    case 0:
      iceb_l_plansh(0,&kod,&naikod,NULL);
      break;

    case 1:
          
      iceb_l_kontr(0,&kod,&naikod,NULL);
      break;

    case 2:

      repl.new_plus(gettext("Введите номер счета с развернутым сальдо"));
      if(iceb_menu_vvod1(repl.ravno(),shetsk,sizeof(shetsk),NULL) != 0)
        break;
      if(shetsk[0] == '\0')
       {
        if(iceb_l_plansh(1,&kod,&naikod,NULL) != 0)
         break;
        strncpy(shetsk,kod.ravno(),sizeof(shetsk));
        naikod.new_plus("");
       }        

      if(iceb_prsh1(shetsk,&rekshet,NULL) != 0)
        break;
      if(rekshet.saldo != 3)
       {
        iceb_u_str repl;
        repl.plus(gettext("Счет имеет свернутое сальдо !"));
        iceb_menu_soob(&repl,NULL);
        break;      
       }       
      iceb_l_kontrsh(0,shetsk,&kod,&naikod,NULL);
        
      break;

    case 3:
      l_saldo_sh(NULL);
      break;

    case 4:
      l_saldo_shk(NULL);
      break;

    case 5:
      if(iceb_f_redfil("nastrb.alx",0,NULL) == 0)
        buhnast_g();
      break;

    case 6:
      iceb_l_gkontr(0,&kod,&naikod,NULL);
      break;
    case 7:
      iceb_l_subbal(0,&kod,&naikod,NULL);
      break;

    case 8:
      iceb_font_selection(NULL);
      break;

    case 9:
      saldogkw(NULL);
      break;

    case 10:
      iceb_f_redfil("zarbanks.alx",0,NULL);
      break;
   }
   
 }

}
/****************************************/
/*Работа с проводками*/
/****************************************/

void    xbug_rsp(void)
{
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Работа с проводками"));

zagolovok.plus(gettext("Работа с проводками"));

punkt_m.plus(gettext("Ввод проводок"));//0
punkt_m.plus(gettext("Поиск проводок"));//1
punkt_m.plus(gettext("Проверка логической целосности базы данных"));//2
punkt_m.plus(gettext("Установка или снятие блокировки проводок"));//3
punkt_m.plus(gettext("Ввод проводок по группам контрагентов"));//4

while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      vibshet(0);
      break;

    case 1:
      poiprovw();
      break;

    case 2:
      plc_gk();
      break;

    case 3:
//      iceb_f_redfil("blok.alx",0,NULL);
      iceb_l_blok(NULL);
      break;

    case 4:
      vibshet(1);
      break;
   }
   
 }

}
/*******************/
/*выбрать вид отчета*/
/*********************/

int i_xbu_vibor_rr()
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;



titl.new_plus(gettext("Распечатать документы для перечисления на карт-счета"));

zagolovok.new_plus(gettext("Распечатать документы для перечисления на карт-счета"));

punkt_m.new_plus(gettext("Расчет с использованием проводок"));//0
punkt_m.plus(gettext("Расчет с использованием сальдо"));//1
int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

static class pvbank_rr rek_ras;
static class pvbanks_rr rek_rass;

switch (nomer)
 {
  case -1:
    return(1);

  case 0:
   if(pvbank_m(&rek_ras) == 0)
     pvbank_r(&rek_ras);
    break;
  case 1:
   if(pvbanks_m(&rek_rass) == 0)
     pvbanks_r(&rek_rass);
    break;
  }
return(0);
}
/**********************************/
/*Распечатать отчеты              */
/**********************************/

void xburo(void)
{
static class go_rr  rek_ras;
int metkasort=0;

int nomer=0,nomer1=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатать отчеты"));

zagolovok.plus(gettext("Распечатать отчеты"));

punkt_m.plus(gettext("Расчет оборотного баланса и главной книги"));//0
punkt_m.plus(gettext("Расчет журнала-ордера по счету"));//1
punkt_m.plus(gettext("Расчет сальдо по всем контрагентам"));//2
punkt_m.plus(gettext("Расчет сальдо по контрагенту"));//3
punkt_m.plus(gettext("Распечатать список лет за которые введена информация"));//4
punkt_m.plus(gettext("Расчет оборотного баланса по забалансовым счетам"));//5
punkt_m.plus(gettext("Распечатать документы для перечисления на карт-счета"));//6
punkt_m.plus(gettext("Распечатать отчёт по комментариям"));//7
punkt_m.plus(gettext("Расчёт шахматки"));//8
punkt_m.plus(gettext("Расчёт по заданным спискам дебетовых и кредитовых счетов"));//9
punkt_m.plus(gettext("Расчёт распределения административных затрат на доходы"));//10
punkt_m.plus(gettext("Расчёт видов затрат по элементам"));//11
punkt_m.plus(gettext("Расчёт по спискам групп контрагентов"));//12
punkt_m.plus(gettext("Расчёт валовых доходов и затрат"));//13
punkt_m.plus(gettext("Расчёт акта сверки по группе контрагента"));//14
punkt_m.plus(gettext("Проверка согласованности данных в подсистемах"));//15

while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      glkniw();
      break;
        
      break;

    case 1:
      if(go_m(&rek_ras) != 0)
       break;
      if(rek_ras.saldo != 3)  //Свернутое сальдо
       {
        if(rek_ras.stat == 0)
         {
          if(gosssw_r(&rek_ras,NULL) == 0)
            iceb_rabfil(&rek_ras.imaf,&rek_ras.naimf,"",0,NULL);

         }
        if(rek_ras.stat == 1)
          go_ss_nbs(&rek_ras);

       }
      else  // Развернутое сальдо
       {
        if(rek_ras.stat == 0)
         {
          nomer1=vibor_sr();
          if(nomer1 == 0)
           {
            if((metkasort=vibor_sosrt()) >= 0)
              if(gosrsw_r(&rek_ras,metkasort,NULL) == 0)
               iceb_rabfil(&rek_ras.imaf,&rek_ras.naimf,"",0,NULL);
           }
          if(nomer1 == 1)
            if((metkasort=vibor_sosrt()) >= 0)
              if(go_srk(&rek_ras,metkasort) == 0)
                   iceb_rabfil(&rek_ras.imaf,&rek_ras.naimf,"",0,NULL);

         }
        if(rek_ras.stat == 1)
            go_srv_nbs(&rek_ras);
       }
                    
      break;

    case 2:
      rspvk_m();
      break;

    case 3:
      iceb_salorok("",NULL);
      break;

    case 4:
      rbd(1);
      break;

    case 5:
      glkni_nbs_m();
      break;

    case 6:
      i_xbu_vibor_rr();
      break;

    case 7:
      buhkomw();
      break;
  
    case 8:
      buhhahw();
      break;

    case 9:
      buhsdkrw();
      break;

    case 10:
      buhrpznpw();
      break;
  
    case 11:
      buhrpzw();
      break;

    case 12:
      buhgspw();
      break;

    case 13:
      buhvdzw();
      break;

    case 14:
      buhspkw();
      break;

    case 15:
      buhpsdw();
      break;
   }
   
 }
}
/*******************/
/*выбрать вид отчета*/
/*********************/

int vibor_ss()
{
char bros[512];

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

sprintf(bros,"%s 1",gettext("Расчет журнала-ордера форма"));
punkt_m.plus(bros);
sprintf(bros,"%s 2",gettext("Расчет журнала-ордера форма"));
punkt_m.plus(bros);

int nomer=0;
return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL));

}

/*******************/
/*выбрать вид отчета*/
/*********************/

int vibor_sr()
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


//zagolovok.plus(organ,40);
zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Расчет журнала-ордера и оборотного баланса по счету"));//0
punkt_m.plus(gettext("Распечатка проводок по контрагентам"));//1


int nomer=0;
return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL));

}
/*******************/
/*выбрать вид сортировки*/
/*********************/

int vibor_sosrt()
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


//zagolovok.plus(organ,40);
zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Без сортировки"));//0
punkt_m.plus(gettext("Сортировка по коду контрагента в символическом виде"));//1
punkt_m.plus(gettext("Сортировка по коду контрагента в цифровом виде"));//2
punkt_m.plus(gettext("Сортировка по наименованию контрагента"));//3
punkt_m.plus(gettext("Сортировка по группе контрагента"));//4


int nomer=0;
return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL));

}

