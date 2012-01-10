/*$Id: i_xzp.c,v 1.135 2011-08-29 07:13:43 sasa Exp $*/
/*06.09.2010	21.06.2006	Белых А.И.	i_xzp.c
Расчёт зароботной платы
*/
#define  DVERSIQ "16.08.2011"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include        <unistd.h>
#include        <sys/stat.h>
#include <string.h>
#include        <locale.h>
#include        "buhg_g.h"


void i_xzp_ro(GtkWidget *wpredok);
void i_xzp_kr();
int zarnastw(void);
void   l_zarsaldo(GtkWidget *wpredok);
int vvod_podr(iceb_u_str *podr,GtkWidget *wpredok);
void l_otrvr(short,short,GtkWidget*);
void zar_ar(void);
void i_xzp_nu();
int vvod_nahud(int prn,class iceb_u_str *nahud,GtkWidget *wpredok);
void l_nahud(int prn,int kod_nah_ud,short mp,short gp,GtkWidget *wpredok);
void zar_krz(GtkWidget*);
void imp_zar_kart(GtkWidget *wpredok);
void imp_zar_kartsh(GtkWidget *wpredok);
int l_zar_dok(GtkWidget *wpredok);
void rasvedw(int,GtkWidget*);
void arz(GtkWidget*);
void zar_kateg(int,GtkWidget*);
void zar_otrsh(GtkWidget*);
void zar_spisr(GtkWidget*);
void zar_spdr(GtkWidget*);
void zar_sprav(int,GtkWidget*);
void zar_srinu(GtkWidget*);
void zar_srninu(GtkWidget*);
void zar_otrvr(GtkWidget*);
void zar_snu(GtkWidget*);
void zar_ppn(GtkWidget*);
void zar_dolgor(GtkWidget*);
void zar_dolgrab(GtkWidget*);
void zar_mg(GtkWidget*);
void zar_kart(GtkWidget*);
void zar_bpom(void);
void zar_alim(GtkWidget*);
void zar_vedpz(GtkWidget*);
void zar_rtabel(GtkWidget*);
void zar_f1df(GtkWidget*);
void l_f1df(GtkWidget*);
void zar_pensf(GtkWidget*);
void zar_soc_nah(GtkWidget*);
void zar_soc_vz(GtkWidget*);
void zar_spsoclgot(GtkWidget*);
void zar_bazno(GtkWidget*);
void zar_f1pv(GtkWidget*);
void zar_pi(GtkWidget*);
void i_xzp_ro_podr(GtkWidget *wpredok);
void zar_podr(int,GtkWidget*);
void zar_snahud(GtkWidget*);
void zar_rbh(GtkWidget*);
void poigod_zar(GtkWidget*);
void poizardokw(GtkWidget *wpredok);
void clearzarw();
void zarpensmw(GtkWidget *wpredok);
void saldozrw();
void zarpdw();
void zarbesvw(GtkWidget *wpredok);



extern SQL_baza	bd;
extern char		*organ;
extern char		*host ;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern char		*parol;
extern iceb_u_str shrift_rek_raz;
short           ddd,mmm,ggg; /*Месяц год*/
float dnei=0.,hasov=0.; //Количество рабочих дней и часов в определённом месяце. Читает программа redkalw.c перед началом расчёта начислений/удержаний
short mfnn=1;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
iceb_kod_podsystem=ICEB_PS_ZP;
int		kom=0;
short m,g;
char bros[1024];
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);

if(iceb_nastsys() != 0)
 iceb_exit(1);

if(zarnastw() != 0)
 iceb_exit(1);

if(iceb_perzap(1,NULL))
 iceb_exit(1);
nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(organ);
iceb_menu_mv_str2.new_plus(gettext("Расчёт заработной платы"));


iceb_u_poltekdat(&ddd,&mmm,&ggg);

if(ddd <= 14)
 iceb_u_dpm(&ddd,&mmm,&ggg,4);

sprintf(bros,"%d.%d",mmm,ggg);

if(iceb_menu_vvod1(gettext("Введите дату расчёта зарплаты (м.г)"),bros,8,NULL) != 0)
  iceb_exit(1);

if(iceb_u_rsdat1(&mmm,&ggg,bros) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата !"),NULL);
  iceb_exit(1);
 }

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с карточками"));
menu_str.plus(gettext("Ввод нормативно-справочной информации"));
menu_str.plus(gettext("Распечатка отчётов"));
menu_str.plus(gettext("Работа с файлами распечаток"));
menu_str.plus(gettext("Сменить текущюю дату"));
menu_str.plus(gettext("Удаление не нужных записей по зарплате"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Расчёт заработной платы"),fioop.ravno(),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        if(iceb_nastsys() != 0)
         iceb_exit(1);
        if(zarnastw() != 0)
          iceb_exit(1);
       }
      break;

    case 1:
      i_xzp_kr();
      break;

    case 2:
      v_nsi(NULL);
      break;

    case 3:
      i_xzp_ro(NULL);
      break;

    case 4:
      iceb_l_rsfr(NULL);
      break;
      
    case 5:

      sprintf(bros,"%d.%d",mmm,ggg);
      iceb_menu_vvod1(gettext("Введите дату расчёта зарплаты (м.г)"),bros,8,NULL);

      if(bros[0] != '\0')
       {
        if(iceb_u_rsdat1(&m,&g,bros) != 0)
          iceb_menu_soob(gettext("Неправильно введена дата !"),NULL);
        else 
         {
          mmm=m;
          ggg=g;
         } 
       }
      break;

    case 6:
      clearzarw();
      break;


  }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}


/*******************************/
/*Ввод нормативно-справочной информации*/
/*************************************/

void v_nsi(GtkWidget *wpredok)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));


zagolovok.plus(gettext("Ввод и корректировка НСИ"));

punkt_m.plus(gettext("Настройка программы"));//0
punkt_m.plus(gettext("Ввод данных для автоматизированного расчёта"));//1

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);



  iceb_u_str kod;
  iceb_u_str naim;
  
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      v_nsi_np(wpredok);
      break;

    case 1:
      v_nsi_rs(wpredok);
      break;


    case 2:
      break;


   }
   
 }

}
/*******************************************/
/*Работа с настроечными файлами*/
/*******************************************/

void i_xzp_nf(GtkWidget *wpredok)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Работа с файлами настройки"));

zagolovok.plus(gettext("Работа с файлами настройки"));

punkt_m.plus(gettext("Работа с файлом настройки"));//0
punkt_m.plus(gettext("Ввод и корректировка количества робочих дней и часов в месяце"));//1
punkt_m.plus(gettext("Настройка автоматического выполнения проводок"));//2
punkt_m.plus(gettext("Работа с перечнем праздничных и выходных дней"));//3
punkt_m.plus(gettext("Ввод и корректировка минимальной зарплати, прожиточного минимума"));//4
punkt_m.plus(gettext("Ввод и корректировка тарифных розрядов и коэффициентов для бюджетных организаций"));//5
punkt_m.plus(gettext("Настройка списка банков для перечисления средств на картсчета"));//6

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      if(iceb_f_redfil("zarnast.alx",0,wpredok) == 0)
       zarnastw();
      break;

    case 1:
      iceb_f_redfil("zarkalndr.alx",0,wpredok);
      break;


    case 2:
//      iceb_f_redfil("zarnpr.alx",0,wpredok);
      zarnvpw(wpredok);
      break;

    case 3:
      iceb_f_redfil("kalend.alx",0,wpredok);
      break;

    case 4:
      iceb_f_redfil("zar_mz_pm.alx",0,wpredok);
      break;

    case 5:
      iceb_f_redfil("zartarroz.alx",0,wpredok);
      
      zartarrozw(wpredok); /*читаем введённые тарифы*/
      break;

    case 6:
      iceb_f_redfil("zarbanks.alx",0,wpredok);
      break;
   }
   
 }

}

/*******************************/
/*Настройка программы*/
/***********************/

void v_nsi_np(GtkWidget *wpredok)
{

int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

zagolovok.plus(gettext("Ввод и корректировка НСИ"));

punkt_m.plus(gettext("Работа с перечнем групп подразделений"));//0
punkt_m.plus(gettext("Работа с перечнем подразделений"));//1
punkt_m.plus(gettext("Работа с перечнем категорий"));//2
punkt_m.plus(gettext("Работа с перечнем начислений"));//3
punkt_m.plus(gettext("Работа с перечнем удержаний"));//4
punkt_m.plus(gettext("Работа с перечнем званий"));//5
punkt_m.plus(gettext("Работа с перечнем видов табеля"));//6
punkt_m.plus(gettext("Работа с файлами настройки"));//7
punkt_m.plus(gettext("Работа со стартовым сальдо"));//8
punkt_m.plus(gettext("Работа с перечнем городов налоговых инспекций"));//9
punkt_m.plus(gettext("Работа с перечнем соц. отчислений"));//10
punkt_m.plus(gettext("Ввод и корректировка списка контрагентов"));//11
punkt_m.plus(gettext("Ввод и корректировка списка банков"));//12
punkt_m.plus(gettext("Выбор шрифта"));//13
punkt_m.plus(gettext("Перенос сальдо по работникам"));//14

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);



  iceb_u_str kod;
  iceb_u_str naim;
    
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      l_gruppod(0,&kod,&naim,wpredok);
      break;

    case 1:
      l_zarpodr(0,&kod,&naim,wpredok);
      break;

    case 2:
      l_zarkateg(0,&kod,&naim,wpredok);
      break;

    case 3:
      l_zarnah(0,&kod,&naim,wpredok);
      break;

    case 4:
      l_zarud(0,&kod,&naim,wpredok);
      break;


    case 5:
      l_zarzvan(0,&kod,&naim,wpredok);
      break;

    case 6:
      l_zarvidtab(0,&kod,&naim,wpredok);
      break;

    case 7:
      i_xzp_nf(NULL);
      break;


    case 8:
      l_zarsaldo(wpredok);
      break;

    case 9:
      l_zargni(0,&kod,&naim,wpredok);
      break;

    case 10:
      l_zarsocf(0,&kod,&naim,wpredok);
      break;

    case 11:
      iceb_l_kontr(0,&kod,&naim,wpredok);
      break;

    case 12:
      l_zarsb(0,&kod,&naim,wpredok);
      break;

    case 13:
      iceb_font_selection(NULL);
      break;

    case 14:
      saldozrw();
      break;

   }
   
 }
}
/***********************************************/
/*Ввод данных для автоматизированного расчёта*/
/**********************************************/
void v_nsi_rs(GtkWidget *wpredok)
{

int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));


zagolovok.plus(gettext("Ввод и корректировка НСИ"));

punkt_m.plus(gettext("Ввести данные для удержания кредита"));//0
punkt_m.plus(gettext("Ввести данные для удержания алиментов"));//1
punkt_m.plus(gettext("Ввести перечень должностных окладов"));//2
punkt_m.plus(gettext("Ввести данные для автоматизированного расчёта начислений"));//3
punkt_m.plus(gettext("Ввести данные для начисление за звание"));//4
punkt_m.plus(gettext("Ввести перечень инвалидов"));//5
punkt_m.plus(gettext("Ввести перечень работников, работающих по договорам"));//6
punkt_m.plus(gettext("Ввести процент выполнения плана по подразделениям"));//7
punkt_m.plus(gettext("Ввести данные для расчёта индексации"));//8
punkt_m.plus(gettext("Ввести перечень пенсионеров"));//9
punkt_m.plus(gettext("Ввод и корректировка льгот для расчёта подоходного налога"));//10
punkt_m.plus(gettext("Ввод и корректировка персональных процентов отчисления в пенс.фонд"));//11

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      iceb_f_redfil("zarkredit.alx",0,wpredok);
      break;
    case 1:
      iceb_f_redfil("zaralim.alx",0,wpredok);
      break;
    case 2:
      iceb_f_redfil("zardolhn.alx",0,wpredok);
      break;
    case 3:
      iceb_f_redfil("zarproc.alx",0,wpredok);
      break;
    case 4:
      iceb_f_redfil("zarzvanie.alx",0,wpredok);
      break;
    case 5:
      iceb_f_redfil("zarinv.alx",0,wpredok);
      break;
    case 6:
      iceb_f_redfil("zardog.alx",0,wpredok);
      break;
    case 7:
      iceb_f_redfil("zarpodpr.alx",0,wpredok);
      break;
    case 8:
      iceb_f_redfil("zarindex.alx",0,wpredok);
      break;
    case 9:
      iceb_f_redfil("zarpens.alx",0,wpredok);
      break;
    case 10:
      iceb_f_redfil("zarlgot.alx",0,wpredok);
      break;

    case 11:
      iceb_f_redfil("zarsppo.alx",0,wpredok);
      break;
   }
   
 }
}
/*****************************/
/*Работа с документами*/
/****************************/
void i_xzp_rsd(GtkWidget *wpredok)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Работа с документами"));


//zagolovok.plus(organ,40);
zagolovok.plus(gettext("Работа с документами"));

punkt_m.plus(gettext("Ввод нового документа"));//0
punkt_m.plus(gettext("Поиск документа по номеру"));//1
punkt_m.plus(gettext("Просмотр всех введённых документов"));//2

int nomer=0;

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);


  class iceb_u_str data_dok("");
  class iceb_u_str nomdok("");
  short prn=0;
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      if((prn=l_zar_dok_prn(wpredok)) < 1)
        break;       

      if(l_zar_dok_hap(&data_dok,&nomdok,prn,wpredok) == 0)
         l_zar_dok_zap(prn,data_dok.ravno(),nomdok.ravno(),wpredok);
        

      break;

    case 1:
      poizardokw(wpredok);
      break;

    case 2:
      l_zar_dok(wpredok);
      break;

   }
 }
 
}

/*********************************/
/*Работа с карточками работников*/
/*********************************/

void i_xzp_kr()
{
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Работа с карточками"));


zagolovok.plus(gettext("Работа с карточками"));

punkt_m.plus(gettext("Поиск по табельному номеру"));//0
punkt_m.plus(gettext("Просмотр работников отдельного подразделения"));//1
punkt_m.plus(gettext("Просмотр всех работников"));//2
punkt_m.plus(gettext("Ввод нового работника"));//3
punkt_m.plus(gettext("Ввод отработанного времени"));//4
punkt_m.plus(gettext("Автоматизированный расчёт зарплат по всем карточкам"));//5
punkt_m.plus(gettext("Ввод и корректировка начислений или удержаний"));//6
punkt_m.plus(gettext("Контроль расчёта зарплаты"));//7
punkt_m.plus(gettext("Установка/снятие блокировки дат"));//8
punkt_m.plus(gettext("Импорт карточек из файла"));//9
punkt_m.plus(gettext("Импорт карт-счетов из файла"));//10
punkt_m.plus(gettext("Работа с документами"));//11
punkt_m.plus(gettext("Автоматическая разноска заработной платы"));//12


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  class iceb_u_str tabnom("");
  class iceb_u_str fio("");
  class iceb_u_str podr("");
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      if(vvod_tabnom(&tabnom,NULL) != 0)
       break;
      vkartz(&tabnom,NULL);
      break;

    case 1:
      if(vvod_podr(&podr,NULL) != 0)
       break;
      l_sptbn(0,&tabnom,&fio,podr.ravno_atoi(),NULL);
      break;

    case 2:
      l_sptbn(0,&tabnom,&fio,0,NULL);
      break;

    case 3:
      vkartz(&tabnom,NULL);
      break;

    case 4:
      l_otrvr(mmm,ggg,NULL);
      break;

    case 5:
      zar_ar();
      break;

    case 6:
      i_xzp_nu();
      break;

    case 7:
      zar_krz(NULL);
      break;

    case 8:
      iceb_f_redfil("zarblok.alx",0,NULL);
      break;

    case 9:
      imp_zar_kart(NULL);
      break;

    case 10:
      imp_zar_kartsh(NULL);
      break;

    case 11:
      i_xzp_rsd(NULL);
      break;

    case 12:
      arz(NULL);
      break;
   }
   
 }
}
/*************************/
/*Начисления/удержания*/
/***********************/
void i_xzp_nu()
{


iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Ввод и корректировка начисления"));//0
punkt_m.plus(gettext("Ввод и корректировка удержания"));//1


int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,NULL);
if(nomer < 0)
 return;
 
class iceb_u_str nahud(0);
if(vvod_nahud(++nomer,&nahud,NULL) != 0)
 return;


l_nahud(nomer,nahud.ravno_atoi(),mmm,ggg,NULL);


}
/*******************************************/
/*Распечатать свод по катгориям*/
/*******************************************/

void i_xzp_ro_kat(GtkWidget *wpredok)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатать свод по категориям"));

zagolovok.plus(gettext("Распечатать свод по категориям"));

punkt_m.plus(gettext("Распечатать свод начислений и удержаний по категориям"));//0
punkt_m.plus(gettext("Распечатать свод начилений/счёт и удержаний/счёт по категориям"));//1
punkt_m.plus(gettext("Распечатать свод отработанного времени по категориям"));//2

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      zar_kateg(0,NULL);
      break;

    case 1:
      zar_kateg(1,NULL);
      break;

    case 2:
      zar_otrsh(NULL);
      break;




   }
   
 }

}

/*******************************************/
/*Распечатать своды по категориям*/
/*******************************************/

void i_xzp_ro_ved(GtkWidget *wpredok)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатать ведомость заработной платы"));

zagolovok.plus(gettext("Распечатать ведомость заработной платы"));

punkt_m.plus(gettext("Распечатать в порядке увеличения табельных номеров"));//0
punkt_m.plus(gettext("Распечатать в алфавитном порядке"));//1
punkt_m.plus(gettext("Автоматическая разноска заработной платы"));//2

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      rasvedw(0,NULL);
      break;

    case 1:
      rasvedw(1,NULL);
      break;

    case 2:
      arz(NULL);
      break;




   }
   
 }

}
/*******************************************/
/*Распечатать список работников*/
/*******************************************/

void i_xzp_ro_spisr(GtkWidget *wpredok)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка списока работников"));
zagolovok.plus(gettext("Распечатка списока работников"));

punkt_m.plus(gettext("Распечатать список работников"));//0
punkt_m.plus(gettext("Распечатать список всех работающих с датой рождения"));//1

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      zar_spisr(NULL);
      break;

    case 1:
      zar_spdr(NULL);
      break;

   }
   
 }

}
/*******************************************/
/*Распечатать справки по зарплате*/
/*******************************************/

void i_xzp_ro_sprav(GtkWidget *wpredok)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатать справки о зарплате"));
zagolovok.plus(gettext("Распечатать справки о зарплате"));

punkt_m.plus(gettext("Распечатать справку о доходах"));//0
punkt_m.plus(gettext("Распечатать расчёт средней зарплаты для начисления больничного"));//1
punkt_m.plus(gettext("Распечатать расчёт средней зарплаты для социальной помощи"));//2
punkt_m.plus(gettext("Распечатать справку о доходах в счёт месяцев начисления"));//3
punkt_m.plus(gettext("Распечатать расчёт отпускных"));//4

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      zar_sprav(1,NULL);
      break;

    case 1:
      zar_sprav(2,NULL);
      break;

    case 2:
      zar_sprav(3,NULL);
      break;

    case 3:
      zar_sprav(4,NULL);
      break;

    case 4:
      zar_sprav(6,NULL);
      break;


   }
   
 }

}

/*******************************************/
/*Распечатать отчёты по работникам*/
/*******************************************/

void i_xzp_ro_porab(GtkWidget *wpredok)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатать отчёты по работникам"));
zagolovok.plus(gettext("Распечатать отчёты по работникам"));

punkt_m.plus(gettext("Распечатать список работников"));//0
punkt_m.plus(gettext("Распечатать справки о зарплате"));//1
punkt_m.plus(gettext("Распечатать список работников, которые имеют начисление или удержание "));//2
punkt_m.plus(gettext("Распечатать список работников, которые не имеют начисление или удержание "));//3
punkt_m.plus(gettext("Распечатать свод отработанного времени по работникам"));//4
punkt_m.plus(gettext("Распечатать свод начислений и удержаний по работникам"));//5
punkt_m.plus(gettext("Распечатать перерасчёт подоходного налога"));//6
punkt_m.plus(gettext("Распечатать долги предприятия перед работниками"));//7
punkt_m.plus(gettext("Распечатать долги работников за месяц"));//8
punkt_m.plus(gettext("Распечатать распределение зарплаты мужчины/женщины"));//9
punkt_m.plus(gettext("Распечатать карточки начислений/удержаний по работникам"));//10
punkt_m.plus(gettext("Распечатать работников, которым начислена благотворительная помощь"));//11

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      i_xzp_ro_spisr(wpredok);
      break;

    case 1:
      i_xzp_ro_sprav(wpredok);
      break;

    case 2:
      zar_srinu(wpredok);
      break;

    case 3:
      zar_srninu(wpredok);
      break;

    case 4:
      zar_otrvr(wpredok);
      break;

    case 5:
      zar_snu(wpredok);
      break;

    case 6:
      zar_ppn(wpredok);
      zarnastw(); //Читаем текущие настройки
      break;

    case 7:
      zar_dolgor(wpredok);
      break;

    case 8:
      zar_dolgrab(wpredok);
      break;

    case 9:
      zar_mg(wpredok);
      break;

    case 10:
      zar_kart(wpredok);
      break;

    case 11:
      zar_bpom();
      break;

   }
   
 }

}
/*******************************************/
/*Распечатать форму 1дф*/
/*******************************************/

void i_xzp_ro_1df(GtkWidget *wpredok)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатать форму 1дф"));
zagolovok.plus(gettext("Распечатать форму 1дф"));

punkt_m.plus(gettext("Расчитать подоходный на выплаченную зарплату"));//0
punkt_m.plus(gettext("Распечатать форму 1дф"));//1
punkt_m.plus(gettext("Работа с архивом документов формы 1ДФ"));//2

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

  switch (nomer)
   {
    case -1:
      return;
    case 0:
      zarpdw();
      break;

    case 1:
      zar_f1df(wpredok);
      break;

    case 2:
      l_f1df(wpredok);
      break;

   }
   
 }

}
/*******************************************/
/*Отчёты в пенсионный фонд*/
/*******************************************/

void i_xzp_ro_pens(GtkWidget *wpredok)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Отчёты в пенсионный фонд"));
zagolovok.plus(gettext("Отчёты в пенсионный фонд"));

punkt_m.plus(gettext("Расчёт единого социального взноса"));//1
punkt_m.plus(gettext("Расчёт годового отчёта для пенсионного персонифицированного учёта"));//0

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

  switch (nomer)
   {
    case -1:
      return;


    case 0:
      zarpensmw(wpredok);
      break;

    case 1:
      zar_pensf(wpredok);
      break;

   }
   
 }

}

/*******************************************/
/*Распечатать отчисления в соц. фонды*/
/*******************************************/

void i_xzp_ro_socot(GtkWidget *wpredok)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатать свод отчислений в гос. фонды"));
zagolovok.plus(gettext("Распечатать свод отчислений в гос. фонды"));

punkt_m.plus(gettext("Распечатать отчёты в налоговую"));//0
punkt_m.plus(gettext("Распечатать справки для Пенсионного фонда Украины"));//1
punkt_m.plus(gettext("Распечатать расчёт соц. отчислений на фонд оплаты труда"));//2
punkt_m.plus(gettext("Распечатать расчёт отчислений на выданную зарплату"));//3
punkt_m.plus(gettext("Распечатать справку имеющих льготи на соц. отчисления"));//4
punkt_m.plus(gettext("Распечатать базы налогообложения для соц. фондов"));//5
punkt_m.plus(gettext("Расчёт статистической формы 1ПВ"));//6
punkt_m.plus(gettext("Распечатать отчёт по пенсионерам и инвалидам"));//7
punkt_m.plus(gettext("Распечатать базы налогообложения для единого социального взноса"));//8




while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      i_xzp_ro_1df(wpredok);
      break;

    case 1:
      i_xzp_ro_pens(wpredok);
      break;

    case 2:
      zar_soc_nah(wpredok);
      break;

    case 3:
      zar_soc_vz(wpredok);
      break;

    case 4:
      zar_spsoclgot(wpredok);
      break;

    case 5:
      zar_bazno(wpredok);
      break;

    case 6:
      zar_f1pv(wpredok);
      break;

    case 7:
      zar_pi(wpredok);
      break;

    case 8:
      zarbesvw(wpredok);
      break;


   }
   
 }

}


/*******************************************/
/*Распечатать отчёты по подразделениям*/
/*******************************************/

void i_xzp_ro_podr(GtkWidget *wpredok)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатать отчёты по подразделениям"));
zagolovok.plus(gettext("Распечатать отчёты по подразделениям"));

punkt_m.plus(gettext("Распечатать отчёты по подразделениям (форма 1)"));
punkt_m.plus(gettext("Распечатать отчёты по подразделениям (форма 2)"));

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      zar_podr(0,wpredok);
      break;

    case 1:
      zar_podr(1,wpredok);
      break;

   }
   
 }

}

/*********************/
/*Распечатка отчётов*/
/*********************/

void i_xzp_ro(GtkWidget *wpredok)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка отчётов"));
zagolovok.plus(gettext("Распечатка отчётов"));

punkt_m.plus(gettext("Распечатать ведомость заработной платы"));//0
punkt_m.plus(gettext("Распечатать свод по категориям"));//1
punkt_m.plus(gettext("Распечатать отчёты по работникам"));//2
punkt_m.plus(gettext("Распечатать реестр почтовых переводов на алименты"));//3
punkt_m.plus(gettext("Распечатать ведомость производственных затрат"));//4
punkt_m.plus(gettext("Распечатать заготовку для заполнения табеля"));//5
punkt_m.plus(gettext("Распечатать свод отчислений в гос. фонды"));//6
punkt_m.plus(gettext("Распечатать отчёты по подразделениям"));//7
punkt_m.plus(gettext("Распечатать свод начислений и удержаний"));//8
punkt_m.plus(gettext("Расчёт частей зарплатных счетов"));//9
punkt_m.plus(gettext("Распечатать список годов за которые введена информация"));//10

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);



  switch (nomer)
   {
    case -1:
      return;

    case 0:
      i_xzp_ro_ved(wpredok);
      break;

    case 1:
      i_xzp_ro_kat(wpredok);
      break;


    case 2:
      i_xzp_ro_porab(wpredok);
      break;

    case 3:
      zar_alim(wpredok);
      break;

    case 4:
      zar_vedpz(wpredok);
      break;

    case 5:
      zar_rtabel(wpredok);
      break;

    case 6:
      i_xzp_ro_socot(wpredok);
      break;

    case 7:
      i_xzp_ro_podr(wpredok);
      break;

    case 8:
      zar_snahud(wpredok);
      break;

    case 9:
      zar_rbh(wpredok);
      break;

    case 10:
      poigod_zar(wpredok);
      break;
   }
   
 }

}
