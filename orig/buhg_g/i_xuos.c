/*$Id: i_xuos.c,v 1.48 2011-09-07 11:44:20 sasa Exp $*/
/*06.09.2010	17.10.2007	Белых А.И.	i_xuos.c
Учёт основных средств
*/
#define  DVERSIQ "06.09.2011"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include        <pwd.h>
#include        <unistd.h>
#include        <sys/stat.h>
#include <string.h>
#include        <locale.h>
#include        "buhg_g.h"

int uosnastw();
void smenakofw(int);
void i_xuos_rsd();
void i_xuos_pkin();
void i_xuos_ro();
void imp_uosdokw(GtkWidget *wpredok);

void l_uosspdok(short ds,short ms,short gs,GtkWidget *wpredok);
void uosprs();
void uosdvosw(int);
void uosvamotw();
void uosvamotbuw();
void uosrpw();
void uosvosiiw(int metka_oth);
void uosdpd();
int poigod_uos(GtkWidget *wpredok);
void uosinvedw();
void uosvosw(int metka_oth);  /*0-налоговый учет 1-бухгалтерский*/
void uosrbsw();
void uos_spw(int metka_r); /*0-по счетам списания 1-по счетам получения*/
void poiuosdokw(GtkWidget *wpredok);

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
iceb_kod_podsystem=ICEB_PS_UOS;
int		kom=0;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);

if(iceb_nastsys() != 0)
 iceb_exit(1);

if(uosnastw() != 0)
 iceb_exit(1);

if(iceb_perzap(1,NULL))
 iceb_exit(1);

nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(organ);
iceb_menu_mv_str2.new_plus(gettext("Учёт основных средств"));

class iceb_u_str kod("");
class iceb_u_str naim("");

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с документами"));//1
menu_str.plus(gettext("Просмотр карточки инвентарного номера"));//2
menu_str.plus(gettext("Просмотр списка инвентарных номеров"));//3
menu_str.plus(gettext("Ввод нормативно-справочной информации"));//4
menu_str.plus(gettext("Распечатка отчётов"));//5
menu_str.plus(gettext("Работа с файлами распечаток"));//6

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Учёт основных средств"),fioop.ravno(),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        if(iceb_nastsys() != 0)
         iceb_exit(1);
        if(uosnastw() != 0)
          iceb_exit(1);
       }
      break;

    case 1:
      i_xuos_rsd();
      break;

    case 2:
      i_xuos_pkin();
      break;

    case 3:
      l_uosin(0,&kod,&naim,NULL);
      break;

    case 4:
      i_xuos_vnsi(NULL);
      break;

    case 5:
      i_xuos_ro();
      break;
      
    case 6:
      iceb_l_rsfr(NULL);
      break;

  }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}

/**********************************/
/*Ввод и корректировка нормативно-справочной информации*/
/*******************************************************/
void i_xuos_vnsi(GtkWidget *wpredok)
{
iceb_u_str stroka;
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

stroka.plus(gettext("Ввод и корректировка НСИ"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Ввод и корректировка приходных операций"));//0
punkt_m.plus(gettext("Ввод и корректировка расходных операций"));//1
punkt_m.plus(gettext("Ввод и корректировка списка подразделений"));//2
punkt_m.plus(gettext("Ввод и корректировка групп для налогового учёта"));//3
punkt_m.plus(gettext("Ввод и корректировка групп для бухгалтерского учёта"));//4
punkt_m.plus(gettext("Ввод и корректировка шифров аналитического учёта"));//5
punkt_m.plus(gettext("Ввод и корректировка шифров затрат"));//6
punkt_m.plus(gettext("Ввод и корректировка материально-ответственных"));//7
punkt_m.plus(gettext("Работа с файлом настройки"));//8
punkt_m.plus(gettext("Ввід і коригування списку контрагентів"));//9
punkt_m.plus(gettext("Смена поправочных коэффициентов для налогового учёта"));//10
punkt_m.plus(gettext("Смена поправочных коэффициентов для бухгалтерского учёта"));//11
punkt_m.plus(gettext("Ввод и корректировка списка форм оплаты"));//12
punkt_m.plus(gettext("Ввод и корректировка списка драгоценных металлов"));//13
punkt_m.plus(gettext("Настройка автоматического выполнения проводок"));//14
punkt_m.plus(gettext("Выбор шрифта"));//15

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
      l_uosopp(0,&kod,&naim,NULL);    
      break;


    case 1:
      l_uosopr(0,&kod,&naim,NULL);    
      break;


    case 2:
      l_uospodr(0,&kod,&naim,NULL);
      break;

    case 3:
      l_uosgrnu(0,&kod,&naim,NULL);
      break;

    case 4:
      l_uosgrbu(0,&kod,&naim,NULL);
      break;

    case 5:
      l_uoshau(0,&kod,&naim,NULL);
      break;

    case 6:
      l_uoshz(0,&kod,&naim,NULL);
      break;

    case 7:
      l_uosmo(0,&kod,&naim,NULL);
      break;

    case 8:
      if(iceb_f_redfil("uosnast.alx",0,NULL) == 0)
       uosnastw();
      break;

    case 9:
      iceb_l_kontr(0,&kod,&naim,NULL);
      break;

    case 10:
      smenakofw(0);
      break;

    case 11:
      smenakofw(1);
      break;

    case 12:
      iceb_l_forop(0,&kod,&naim,NULL);
      break;

    case 13:
      l_uosdm(0,&kod,&naim,NULL);
      break;

    case 14 :
      menuvnpw(3,0,NULL);
      break;

    case 15 :
      iceb_font_selection(NULL);
      break;

   }
   
 }

}
/*****************************/
/*Ввод нового документа*/
/**************************/

void i_xuos_vnd()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Ввод нового документа"));

zagolovok.plus(gettext("Ввод нового документа"));

punkt_m.plus(gettext("Ввод приходного документа"));//1
punkt_m.plus(gettext("Ввод расходного документа"));//0

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);



class iceb_u_str nomdok("");
class iceb_u_str datad("");
class iceb_u_str kod("");
class iceb_u_str naim("");

switch (nomer)
 {
  case -1:
    return;

  case 0:
    if(l_uosopp(1,&kod,&naim,NULL) == 0)
      if(uosvhd(&datad,&nomdok,&kod,1,NULL) == 0)
        l_uosdok(datad.ravno(),nomdok.ravno(),NULL);
       
    return;

  case 1:
    if(l_uosopr(1,&kod,&naim,NULL) == 0)
     if(uosvhd(&datad,&nomdok,&kod,2,NULL) == 0)
        l_uosdok(datad.ravno(),nomdok.ravno(),NULL);
    return;


 }
}

/*******************************/
/*Работа с документами*/
/*******************************/
void i_xuos_rsd()
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

punkt_m.plus(gettext("Ввод нового документа"));//0
punkt_m.plus(gettext("Поиск введённого документа"));//1
punkt_m.plus(gettext("Просмотр всех введённых документов"));//2

sprintf(strsql,"%s %02d.%02d.%d%s",gettext("Просмотр документов начиная с"),dt,mt,gt,gettext("г."));

punkt_m.plus(strsql);//3

punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//4
punkt_m.plus(gettext("Импорт документов"));//5
punkt_m.plus(gettext("Пропорциональное распределение сумм"));//6
//punkt_m.plus(gettext("Дооценка основных средств с нулевой остаточной стоимостью (бух.учёт)"));//7

while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      i_xuos_vnd();
      break;

    case 1:
      poiuosdokw(NULL);
      break;
      
    case 2:
      l_uosspdok(0,0,0,NULL);
      break;

    case 3:
      l_uosspdok(dt,mt,gt,NULL);
      break;
    
    case 4:
//      iceb_f_redfil("bluos.alx",0,NULL);
      iceb_l_blok(NULL);
      break;
    case 5:
      imp_uosdokw(NULL);
      break;
    case 6:
      uosprs();
      break;
   }
 }
}

/**********************/
/*Просмотр карточки инвентарного номера*/
/****************************************/

void i_xuos_pkin()
{

class iceb_u_str innom;
if(uosgetinn(&innom,1,NULL) != 0)
 return;
uoskart2(innom.ravno_atoi(),0,0,0,NULL);



}
/*****************/
/*Выбор расчёта*/
/****************/
void i_xuos_vr()
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Расчёт движения основных средств по счетам получения"));//1
punkt_m.plus(gettext("Расчёт движения основных средств по счетам списания"));//0


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
switch(nomer)
 {
  case 0:
    uos_spw(0); /*0-по счетам списания 1-по счетам получения*/
    break;
  case 1:
    uos_spw(1); /*0-по счетам списания 1-по счетам получения*/
    break;
 }
}
/********************************/
/*распечатка ведомости движения*/
/*******************************/
void i_xuos_vd()
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Распечатка ведомости движения основных средств по подразделениям"));//0
punkt_m.plus(gettext("Распечатка ведомости движения основных средств по мат. ответственным"));//1


int nomer=0;
while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch(nomer)
   {
    case 0:
      uosdvosw(0);
      break;

    case 1:
      uosdvosw(1);
      break;
   }
 }
}
/********************************/
/*распечатка ведомости аморт-отчислений*/
/*******************************/
void i_xuos_ao()
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Распечатка ведомости аморт-отчислений для налогового учёта"));//2
punkt_m.plus(gettext("Распечатка ведомости аморт-отчислений для бух. учёта"));//3


int nomer=0;
while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch(nomer)
   {
    case 0:
      uosvamotw();
      break;

    case 1:
      uosvamotbuw();
      break;

   }
 }
}
/********************************/
/*распечатка ведомости износа и остаточной стоимости*/
/*******************************/
void i_xuos_viios()
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Распечатать ведомость износа и остаточной стоимости (налоговый учёт)"));//5
punkt_m.plus(gettext("Распечатать ведомость износа и остаточной стоимости (бух. учёт)"));//6


int nomer=0;
while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch(nomer)
   {
    case 0:
      uosvosiiw(0);
      break;

    case 1:
      uosvosiiw(1);
      break;

   }
 }
}
/********************************/
/*распечатка ведомости остаточной стоимости*/
/*******************************/
void i_xuos_vos()
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Распечатать ведомость остаточной стоимости (налоговый учёт)"));//10
punkt_m.plus(gettext("Распечатать ведомость остаточной стоимости (бух. учёт)"));//11


int nomer=0;
while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch(nomer)
   {
    case 0:
      uosvosw(0);
      break;

    case 1:
      uosvosw(1);
      break;
   }
 }
}

/*******************/
/*Распечатка отчётов*/
/*********************/

void i_xuos_ro()
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка отчётов"));


zagolovok.plus(gettext("Распечатка отчётов"));


punkt_m.plus(gettext("Распечатка ведомости движения основных средств"));//0
punkt_m.plus(gettext("Распечатка ведомости аморт-отчислений"));//2
punkt_m.plus(gettext("Распечатать реестр проводок по видам операций"));//4
punkt_m.plus(gettext("Распечатать ведомость износа и остаточной стоимости (налоговый учёт)"));//5
punkt_m.plus(gettext("Распечатать движение по документам"));//7
punkt_m.plus(gettext("Распечатать список лет за которые введена информация"));//8
punkt_m.plus(gettext("Распечатать инвентаризационные ведомости"));//9
punkt_m.plus(gettext("Распечатать ведомость остаточной стоимости"));//10
punkt_m.plus(gettext("Расчёт балансовой стоимости по мат.ответственным и счетам учёта"));//12
punkt_m.plus(gettext("Расчёт движения основных средств по счетам списания/получения"));//13


int nomer=0;

while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      i_xuos_vd();
      break;


    case 1:
      i_xuos_ao();
      break;

    case 2:
      uosrpw();
      break;

    case 3:
      i_xuos_viios();
      break;

    case 4:
      uosdpd();
      break;

    case 5:
      poigod_uos(NULL);
      break;

    case 6:
      uosinvedw();
      break;

    case 7:
      i_xuos_vos();
      break;

    case 8:
      uosrbsw();
      break;
    case 9:
      i_xuos_vr();
      break;
   }
 }
}


