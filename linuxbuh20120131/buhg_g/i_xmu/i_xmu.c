/*$Id: i_xmu.c,v 1.168 2011-09-05 08:18:27 sasa Exp $*/
/*14.10.2010	03.05.2004	Белых А.И.	i_xmu.c
Материольный учёт
*/
#define DVERSIQ "31.01.2012"
#include        <stdlib.h>
#include        <sys/stat.h>
#include        <pwd.h>
#include        <unistd.h>
#include        <locale.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze


void    xmu_nsi(void);
void    rabdok(void);
void    rasoth(void);
int    sklkar_m(iceb_u_str*,iceb_u_str*,GtkWidget*);
void  l_doksmat(short dp,short mp,short gp,GtkWidget *wpredok);
void  vvodnd(void);
void rasoskr_oiv();
void poimatdokw(GtkWidget *wpredok);
void impmatdokw(void);
void sproznw(void);
void plc_mu(void);
void ras_dtmc(void);
void muinvw(void);
void dvtmcf1w(void);
void dvtmcf2w(void);
void dvtmcf3w(void);
void zagotkl(void);
void rasspdokw(void);
void rppvow(void);
void rasndsw(void);
void prognozw(void);
void rozn_oth(int,int,const char*);
void poigod_mu(GtkWidget*);
void doocsumw(void);
void kriostw(void);
void matuddokw(void);
void opssw(void);
void l_spec(GtkWidget*);
void spis_p_kontrw(void);
void ost_skl_shetw(GtkWidget *wpredok);
void mudmcw();
void mumow();
void saldomuw(GtkWidget*);
int l_nalog(int,GtkWidget*);

extern SQL_baza	bd;
extern char		*organ;
extern char		*host;
extern char		*parol;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern iceb_u_str shrift_rek_raz;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
iceb_kod_podsystem=ICEB_PS_MU;
int		kom=0;
iceb_u_str skl;
iceb_u_str n_kar;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);

//printf("Уровень вхождения циклов=%d\n",gtk_main_level());
if(iceb_nastsys() != 0)
 iceb_exit(1); 
if(matnastw() != 0)
 iceb_exit(1);
if(iceb_perzap(1,NULL))
 iceb_exit(1);
nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(organ);
iceb_menu_mv_str2.new_plus(gettext("Материальный учёт"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с товарно-материальными документами"));
menu_str.plus(gettext("Ввод нормативно-справочной информации"));
menu_str.plus(gettext("Распечатать отчеты"));
menu_str.plus(gettext("Работа с файлами распечаток"));
menu_str.plus(gettext("Просмотр карточки материалла"));
menu_str.plus(gettext("Удаление документов за год"));



while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Материальный учёт"),fioop.ravno(),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        if(iceb_nastsys() != 0)
         iceb_exit(1);
        if(matnastw() != 0)
          iceb_exit(1);
       }
      break;


    case 1:
      rabdok();
      break;

    case 2:
      xmu_nsi();
      break;

    case 3:
      rasoth();
      break;

    case 4:
      iceb_l_rsfr(NULL);
      break;



    case 5:
      if(sklkar_m(&skl,&n_kar,NULL) != 0)
        break;
      l_zkartmat(skl.ravno_atoi(),n_kar.ravno_atoi(),NULL);
        
      break;

    case 6:
      matuddokw();
      break;

   }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}

/***********************/
/*Работа с документами*/
/***********************/

void rabdok(void)
{
char       strsql[512];
iceb_u_str stroka;
int nomer=0;

short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Работа с товарно-материальными документами"));

//memset(strsql,'\0',sizeof(strsql));
//iceb_u_strncpy(strsql,organ,40);
//stroka.new_plus(strsql);
stroka.plus(gettext("Работа с товарно-материальными документами"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Ввод нового документа"));//0
punkt_m.plus(gettext("Поиск введённого документа"));//1
punkt_m.plus(gettext("Просмотр всех введённых документов"));//2

sprintf(strsql,"%s %02d.%02d.%d%s",gettext("Просмотр документов начиная с"),
ddd,mmm,ggg,
gettext("г."));

punkt_m.plus(strsql);//3

punkt_m.plus(gettext("Импорт документов"));//4
punkt_m.plus(gettext("Списание розничной торговли"));//5
punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//6
punkt_m.plus(gettext("Проверка логической целосности базы данных"));//7  
while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      vvodnd();
      break;

    case 1:
      poimatdokw(NULL);
      break;

    case 2:
      l_doksmat(0,0,0,NULL);
      break;

    case 3:
      l_doksmat(ddd,mmm,ggg,NULL);
      break;

    case 4:
      impmatdokw();
      break;

    case 5:
      sproznw();
      break;

    case 6:
//      iceb_f_redfil("blokmak.alx",0,NULL);
      iceb_l_blok(NULL);
      break;

    case 7:
      plc_mu();
      break;


   }
   
 }

}


/****************************/
/*Ввод нормативно-справочной информации*/
/***************************************/
void    xmu_nsi(void)
{
iceb_u_str stroka;
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

stroka.plus(gettext("Ввод и корректировка НСИ"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Ввод и корректировка списка групп товаров и материалов"));//0
punkt_m.plus(gettext("Ввод и корректировка списка единиц измерения"));//1
punkt_m.plus(gettext("Ввод и корректировка списка товаров и материалов"));//2
punkt_m.plus(gettext("Ввод и корректировка списка операций приходов"));//3
punkt_m.plus(gettext("Ввод и корректировка списка операций расходов"));//4
punkt_m.plus(gettext("Ввод и корректировка списка складов"));//5
punkt_m.plus(gettext("Ввод и корректировка списка форм оплаты"));//6
punkt_m.plus(gettext("Работа с файлом настройки"));//7
punkt_m.plus(gettext("Ввод и корректировка списка контрагентов"));//8
punkt_m.plus(gettext("Ввод и корректировка плана счетов"));//9
punkt_m.plus(gettext("Работа со списком корректировок на сумму документа"));//10
punkt_m.plus(gettext("Настройка автоматического выполнения проводок"));//11
punkt_m.plus(gettext("Ввод и корректировка спецификаций на изделия"));//12
punkt_m.plus(gettext("Ввод и корректировка концовки счёта"));//13
punkt_m.plus(gettext("Ввод и корректировка концовки накладной"));//14
punkt_m.plus(gettext("Выбор шрифта"));//15
punkt_m.plus(gettext("Перенос стартовых остатков"));//16
punkt_m.plus(gettext("Ввод и корректировка НДС"));//17

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);



  iceb_u_str kod;
  iceb_u_str naikod;
  
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      iceb_l_grupmat(0,&kod,&naikod,NULL);
      break;

    case 1:
      iceb_l_ei(0,&kod,&naikod,NULL);
      break;

    case 2:
      l_mater(0,&kod,&naikod,0,0,NULL);
      break;

    case 3:
      iceb_l_opmup(0,&kod,&naikod,NULL);
      break;

    case 4:
      iceb_l_opmur(0,&kod,&naikod,NULL);
      break;

    case 5:
      iceb_l_sklad(0,&kod,&naikod,NULL);
      break;

    case 6:
      iceb_l_forop(0,&kod,&naikod,NULL);
      break;

    case 7:
      if(iceb_f_redfil("matnast.alx",0,NULL) == 0)
        matnastw();
      break;

    case 8:
      iceb_l_kontr(0,&kod,&naikod,NULL);
      break;

    case 9:
      iceb_l_plansh(0,&kod,&naikod,NULL);
      break;

    case 10 :
      iceb_f_redfil("makkor.alx",0,NULL);
      break;

    case 11 :
      menuvnpw(1,0,NULL);
      break;

    case 12 :
      l_spec(NULL);
      break;

    case 13:
      iceb_f_redfil("matshetend.alx",0,NULL);
      break;

    case 14:
      iceb_f_redfil("matnakend.alx",0,NULL);
      break;

    case 15:
      iceb_font_selection(NULL);
      break;

    case 16:
      saldomuw(NULL);
      break;

    case 17:
      l_nalog(0,NULL);
      break;

   }
   
 }

}
/*****************************/
/*Распечатка отчётов*/
/*****************************/

void rasoth()
{

iceb_u_str stroka;
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка отчётов"));

stroka.plus(gettext("Распечатка отчётов"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Распечатать остатки на карточках"));//0
punkt_m.plus(gettext("Распечатать движение товарно-материальных ценностей"));//1
punkt_m.plus(gettext("Распечатать заготовку для отчёта"));//2
punkt_m.plus(gettext("Распечатать список документов"));//3
punkt_m.plus(gettext("Распечатать реестр проводок по видам операций"));//4
punkt_m.plus(gettext("Распечатать распределение НДС по видам операций"));//5
punkt_m.plus(gettext("Распечатать прогноз реализации товаров"));//6
punkt_m.plus(gettext("Распечатать розничную реализацию по торговим местам"));//7
punkt_m.plus(gettext("Распечатать свод розничной реализации"));//8
punkt_m.plus(gettext("Распечатать список годов, за которые введена информация"));//9
punkt_m.plus(gettext("Распечатать суммы выполненных автоматических дооценок"));//10
punkt_m.plus(gettext("Распечатать материалы имеющие критический остаток"));//11

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;
    case 0:
      rasoskr_oiv();
      break;

    case 1:
      ras_dtmc();
      break;

    case 2:
      zagotkl();
      break;

    case 3:
      rasspdokw();
      break;

    case 4:
      rppvow();
      break;

    case 5:
      rasndsw();
      break;

    case 6:
      prognozw();
      break;

    case 7:
      rozn_oth(0,0,"");
      break;

    case 8:
      rozn_oth(1,0,"");
      break;


    case 9:
      poigod_mu(NULL);
      break;

    case 10:
      doocsumw();
      break;

    case 11:
      kriostw();
      break;


   }
   
 }

}
/*****************************/
/*Ввод нового документа*/
/**************************/

void vvodnd()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Ввод нового документа"));

//zagolovok.plus(organ,40);
zagolovok.plus(gettext("Ввод нового документа"));

punkt_m.plus(gettext("Ввод расходного документа"));//0
punkt_m.plus(gettext("Ввод приходного документа"));//1

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);



iceb_u_str kod;
iceb_u_str naikod;
iceb_u_str nomdok;
iceb_u_str sklad;
iceb_u_str datad;
switch (nomer)
 {
  case -1:
    return;

  case 0:
    datad.new_plus("");
    sklad.new_plus("");
    nomdok.new_plus("");
    if(iceb_l_opmur(1,&kod,&naikod,NULL) == 0)
      if(vdndw(2,&datad,&sklad,&nomdok,&kod,NULL) == 0)
           l_dokmat(&datad,1,&sklad,&nomdok,NULL);

    return;

  case 1:
    datad.new_plus("");
    sklad.new_plus("");
    nomdok.new_plus("");
    if(iceb_l_opmup(1,&kod,&naikod,NULL) == 0)
      if(vdndw(1,&datad,&sklad,&nomdok,&kod,NULL) == 0)
           l_dokmat(&datad,1,&sklad,&nomdok,NULL);

    return;


 }
}
/*****************************/
/*Распечатать остатки на картосках*/
/**************************/

void rasoskr_oiv()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Распечатка отчётов"));

//zagolovok.plus(organ,40);
zagolovok.plus(gettext("Распечатка отчётов"));

punkt_m.plus(gettext("Распечатать остатки на карточках"));//0
punkt_m.plus(gettext("Распечатать инвентаризационные ведомости"));//1
punkt_m.plus(gettext("Расчёт остатков по складам и счетам учёта"));//2

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

switch (nomer)
 {
  case -1:
    return;

  case 0:
    rasoskrw(NULL);
    return;

  case 1:
    muinvw();
    return;

  case 2:
    ost_skl_shetw(NULL);
    return;
 }
}
/******************************/
/*Меню выбора отчёта*/
/************************/
void ras_dtmc()
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка отчётов"));

iceb_u_str stroka;
//stroka.new_plus(organ);
stroka.plus(gettext("Распечатать движение товарно-материальных ценностей"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Распечатать движение товарно-материальных ценностей (форма 1)"));//0
punkt_m.plus(gettext("Распечатать движение товарно-материальных ценностей (форма 2)"));//1
punkt_m.plus(gettext("Распечатать движение товарно-материальных ценностей (форма 3)"));//2
punkt_m.plus(gettext("Распечатать движение товарно-материальных ценностей по счетам списания"));//3
punkt_m.plus(gettext("Расчёт списания/получения материаллов по контрагентам"));//4
punkt_m.plus(gettext("Расчёт движения по кодам материаллов"));//5
punkt_m.plus(gettext("Материальный отчёт"));//6

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;
    case 0:
      dvtmcf1w();
      break;
    case 1:
      dvtmcf2w();
      break;
    case 2:
      dvtmcf3w();
      break;
    case 3:
      opssw();
      break;
    case 4:
      spis_p_kontrw();
      break;
    case 5:
      mudmcw();
      break;
    case 6:
      mumow();
      break;
   }
   
 }

}
