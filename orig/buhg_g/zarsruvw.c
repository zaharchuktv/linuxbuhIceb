/*$Id: zarsruvw.c,v 1.11 2011-08-29 07:13:44 sasa Exp $*/
/*20.11.2009	22.11.2001	Белых А.И.	zarsruvw.c
Расчет средней зароботной платы социальной помощи
*/
#include <stdlib.h>
#include        <errno.h>
#include        <math.h>
#include        <time.h>
#include        "buhg_g.h"
#include <unistd.h>
#include "zar_sprav.h"

void zarsruv_end(double suma,FILE *ff,GtkWidget *wpredok);
void zarsruv_sh(const char *inn,const char *fio,FILE *ff,GtkWidget *wpredok);


extern short	*kodsocstrnv; //Коды не входящие в расчет соцстраха
extern short    *knvr;/*Коды начислений не входящие в расчет подоходного налога*/
extern char     *organ;
extern short    *kodmp;   /*Коды материальной помощи*/
extern short    kodpen;  /*Код пенсионных отчислений*/
extern short    kodsocstr;  /*Код отчисления на соц-страх*/
extern short    kodbzr;  /*Код отчисления на безработицу*/
extern class iceb_u_str kods_esv_all; /*Все коды единого социального взноса*/
extern class iceb_u_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/
extern SQL_baza bd;

void zarsruvw(class zar_sprav_rek *data,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
char		strsql[2048];
SQL_str		row,row1;
int		kolstr=0;
short		kolmes=0; //Количество месяцев в периоде
double		suma=0.;
short		kkk;
char		imaf[64],imafprot[64],imaf_bezr[64];
FILE		*ff,*ffprot,*ff_bezr;
char		kodtabotrv[312]; //Коды табеля отработанного времени
char		kodtabotp[312]; //Коды табеля отпускных
short		knah;
short		dr,mr,gr;
char		fambuh[512];
short		prn=0;
double		matpomb=0.;
class iceb_u_str fio("");

SQLCURSOR curr;

short dn=1,mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,data->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->datak.ravno());

kolmes=iceb_u_period(1,mn,gn,1,mk,gk,1);
if(kolmes == 0)
 {
  sprintf(strsql,"%s %d.%d  %d.%d",gettext("Количество месяцев в периоде равно нолю !"),mn,gn,mk,gk);
  iceb_menu_soob(strsql,wpredok);
  return;
 }
char inn[50];
sprintf(strsql,"select fio,inn from Kartb where tabn=%d",data->tabnom.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),data->tabnom.ravno_atoi());
  iceb_menu_soob(strsql,wpredok);
  return;
 }
strncpy(inn,row[1],sizeof(inn)-1);
fio.new_plus(row[0]);

sprintf(strsql,"%s %d.%d %s %d.%d %s %s\n",
gettext("Период с"),mn,gn,
gettext("до"),mk,gk,data->tabnom.ravno(),data->fio.ravno());

iceb_printw(iceb_u_toutf(strsql),buffer,view);

sprintf(strsql,"%s:%d\n",gettext("Количество месяцев"),kolmes);

iceb_printw(iceb_u_toutf(strsql),buffer,view);

memset(fambuh,'\0',sizeof(fambuh));
iceb_poldan("Табельный номер бухгалтера",fambuh,"zarnast.alx",wpredok);
if(fambuh[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",fambuh);
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
   strncpy(fambuh,row[0],sizeof(fambuh)-1);
 }

class iceb_u_double nash; //Всего начислено
class iceb_u_double vrashet; //Всего берется в расчет
class iceb_u_double pens; //Всего пенсионных отчислений
class iceb_u_double bezr; //Всего по безработице
class iceb_u_double netrud; //Всего по нетрудоспособности
class iceb_u_int    kolrd; //Количество отработанных дней

nash.make_class(kolmes);
vrashet.make_class(kolmes);
pens.make_class(kolmes);
bezr.make_class(kolmes);
netrud.make_class(kolmes);
kolrd.make_class(kolmes);

iceb_poldan("Коды видов табеля отработанного времени",kodtabotrv,"zarsrbol.alx",wpredok);
iceb_poldan("Коды видов табеля отпускных",kodtabotp,"zarsrbol.alx",wpredok);

sprintf(imaf,"szu%s_%d.lst",data->tabnom.ravno(),getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

sprintf(imaf_bezr,"bezr%s_%d.lst",data->tabnom.ravno(),getpid());
if((ff_bezr = fopen(imaf_bezr,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_bezr,"",errno,wpredok);
  return;
 }


zarsruv_sh(inn,fio.ravno(),ff_bezr,wpredok);

sprintf(imafprot,"szup%s_%d.lst",data->tabnom.ravno(),getpid());
if((ffprot = fopen(imafprot,"w")) == NULL)
 {
  iceb_er_op_fil(imafprot,"",errno,wpredok);
  return;
 }
short dk=30;
iceb_u_dpm(&dk,&mk,&gk,5);
iceb_u_zagolov(gettext("Расчет средней зарплаты для социальной помощи"),1,mn,gn,dk,mk,gk,organ,ffprot);
fprintf(ffprot,"%s %s\n",data->tabnom.ravno(),data->fio.ravno());
fprintf(ffprot,"%s:%d\n",gettext("Количество месяцев"),kolmes);

SQLCURSOR cur;
class zar_read_tn1h nastr;

dr=1; mr=mn; gr=gn;
for(kkk=0; kkk < kolmes; kkk++)
 {
  fprintf(ffprot,"\n%d.%d\n",mr,gr);
  sprintf(strsql,"select prn,knah,suma from Zarp where tabn=%s and \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and suma <> 0.",
  data->tabnom.ravno(),gr,mr,gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }
  
  if(kolstr != 0)
   {
    //чтение настроек
    zar_read_tn1w(1,mr,gr,&nastr,NULL,wpredok);
    
    while(cur.read_cursor(&row) != 0)
     {
      prn=atoi(row[0]);
      knah=atoi(row[1]);
      suma=atof(row[2]);    
      if(prn == 1)
        sprintf(strsql,"select naik from Nash where kod=%d",knah);
      if(prn == 2)
        sprintf(strsql,"select naik from Uder where kod=%d",knah);
      sql_readkey(&bd,strsql,&row1,&curr);
      
      fprintf(ffprot,"%2d %-*s %8.2f\n",knah,iceb_u_kolbait(40,row1[0]),row1[0],suma);
      if(prn == 1)
       {
        nash.plus(suma,kkk);    
        
        if(provkodw(knvr,knah) >= 0)
         {
          fprintf(ffprot,"%s !\n",gettext("Не вошло в расчет"));      
          continue;    
         }
        if(iceb_u_sravmydat(dn,mn,gn,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
         {
          if(provkodw(kodsocstrnv,knah) >= 0)
           {
            fprintf(ffprot,"%s !\n",gettext("Не вошло в расчет"));      
            continue;    
           }
         }
        else
         {
          if(iceb_u_proverka(knvr_esv_r.ravno(),knah,0,1) == 0)
           {
            fprintf(ffprot,"%s !\n",gettext("Не вошло в расчет"));      
            continue;    
           }
         }
        vrashet.plus(suma,kkk);    
       }
      if(prn == 2)
       {
        if(knah == kodpen) //Код пенсионных отчислений
          pens.plus(suma,kkk);
        if(knah == kodsocstr) //Код отчисления на соц-страх
          netrud.plus(suma,kkk);

        if(iceb_u_sravmydat(dn,mn,gn,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
         {
          if(knah == kodbzr) //Код отчисления на безработицу
            bezr.plus(suma,kkk);
         }
        else
         {
          if(iceb_u_proverka(kods_esv_all.ravno(),knah,0,1) == 0)
            bezr.plus(suma,kkk);
           
         }
       }
     }
    double suma_blag_pom,suma_blag_pom_b;
    vrashet.plus(matpom1w(data->tabnom.ravno_atoi(),mr,gr,kodmp,ffprot,1,&matpomb,0,&suma_blag_pom,&suma_blag_pom_b,wpredok)*-1,kkk);
    if(vrashet.ravno(kkk) > nastr.max_sum_for_soc)
     {
      fprintf(ffprot,"%s %.2f\n",gettext("Больше"),nastr.max_sum_for_soc);
      vrashet.new_plus(nastr.max_sum_for_soc,kkk);
     }
    fprintf(ffprot,"%33s:%8.2f\n",gettext("Итого"),vrashet.ravno(kkk));
   }  
  //Узнаем количество отработанных дней
  sprintf(strsql,"select kodt,dnei,kdnei from Ztab where tabn=%s and god=%d \
and mes=%d",data->tabnom.ravno(),gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   { 
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }
  while(cur.read_cursor(&row) != 0)
   {
    sprintf(strsql,"select naik from Tabel where kod=%s",row[0]);
    sql_readkey(&bd,strsql,&row1,&curr);
    fprintf(ffprot,"%-2s %-*s %2s %2s\n",row[0],iceb_u_kolbait(30,row1[0]),row1[0],row[1],row[2]);
    if(iceb_u_proverka(kodtabotrv,row[0],0,1) == 0)
      kolrd.plus(atoi(row[2]),kkk);
    if(iceb_u_proverka(kodtabotp,row[0],0,1) == 0)
      kolrd.plus(atoi(row[2]),kkk);
   }  
  iceb_u_dpm(&dr,&mr,&gr,3);
 }
//Распечатываем результат

fprintf(ff,"\
                   ДОВІДКА\n\
          про середню заробітну плату (дохід)\n\
видана особі %s\n\
яка працює (працювала) %s\n\
ідентифікаційний номер %-10s  Номер страхового свідоцтва____________\n",
data->fio.ravno(),organ,inn);

if(iceb_u_sravmydat(dn,mn,gn,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
 {
  fprintf(ff,"\
------------------------------------------------------------------------\n\
 Дата  |Кількість|Нарахована|Сплачені|Cтрахові внески,сплачені особою  |\n\
       |  днів   |зарплата  |внески  |Пенсійний|Безробіття|Непра-тність|\n\
------------------------------------------------------------------------\n");

  sprintf(strsql,"\
------------------------------------------------------------------------\n\
 Дата  |Кількість|Нарахована|Сплачені|Cтрахові внески,сплачені особою  |\n\
       |  днів   |зарплата  |внески  |Пенсійний|Безробіття|Непра-тність|\n\
------------------------------------------------------------------------\n");
 }
else
 {
  fprintf(ff,"\
-------------------------------------------------\n\
 Дата  |Кількість|Нарахована|Сплачені|Единий соц|\n\
       |  днів   |зарплата  |внески  | внесок   |\n\
-------------------------------------------------\n");

  sprintf(strsql,"\
-------------------------------------------------\n\
 Дата  |Кількість|Нарахована|Сплачені|Единий соц|\n\
       |  днів   |зарплата  |внески  | внесок   |\n\
-------------------------------------------------\n");
 }


iceb_printw(iceb_u_toutf(strsql),buffer,view);



int iotr=0;
double itog[5];
memset(&itog,'\0',sizeof(itog));
dr=1; mr=mn; gr=gn;
int nomer_str_bez=0;
char naim_mes[128];
for(kkk=0; kkk < kolmes; kkk++)
 {
  /*читаем значение max_sum_for_soc действующее в этом месяце*/
  class zar_read_tn1h nastr;
  zar_read_tn1w(1,mr,gr,&nastr,NULL,wpredok);


  if(iceb_u_sravmydat(dn,mn,gn,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
   {
    sprintf(strsql,"%02d.%d %-9d %10.2f %8.2f %9.2f %10.2f %12.2f\n",
    mr,gr,kolrd.ravno(kkk),nash.ravno(kkk),vrashet.ravno(kkk),pens.ravno(kkk)*-1,bezr.ravno(kkk)*-1,
    netrud.ravno(kkk)*-1);

    fprintf(ff,"%02d.%d %-9d %10.2f %8.2f %9.2f %10.2f %12.2f\n",
    mr,gr,kolrd.ravno(kkk),nash.ravno(kkk),vrashet.ravno(kkk),pens.ravno(kkk)*-1,bezr.ravno(kkk)*-1,
    netrud.ravno(kkk)*-1);
   }  
  else
   {
    sprintf(strsql,"%02d.%d %-9d %10.2f %8.2f %10.2f\n",
    mr,gr,kolrd.ravno(kkk),nash.ravno(kkk),vrashet.ravno(kkk),bezr.ravno(kkk)*-1);

    fprintf(ff,"%02d.%d %-9d %10.2f %8.2f %10.2f\n",
    mr,gr,kolrd.ravno(kkk),nash.ravno(kkk),vrashet.ravno(kkk),bezr.ravno(kkk)*-1);

   }
  iceb_printw(iceb_u_toutf(strsql),buffer,view);

  memset(naim_mes,'\0',sizeof(naim_mes));
  iceb_mesc(mr,0,naim_mes);

  fprintf(ff_bezr,"|%02d|%02d.%04d%15s|%-*s|%14d|%13.2f|%13.2f|%16.2f|\n",
  ++nomer_str_bez,
  mr,
  gr,
  "",
  iceb_u_kolbait(21,naim_mes),
  naim_mes,
  kolrd.ravno(kkk),nash.ravno(kkk),vrashet.ravno(kkk),bezr.ravno(kkk)*-1);

  iotr+=kolrd.ravno(kkk);
  itog[0]+=nash.ravno(kkk);
  itog[1]+=vrashet.ravno(kkk);
  itog[2]+=pens.ravno(kkk)*-1;
  itog[3]+=bezr.ravno(kkk)*-1;
  itog[4]+=netrud.ravno(kkk)*-1;
  

  iceb_u_dpm(&dr,&mr,&gr,3);
 }

fprintf(ff_bezr,"\
-------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_bezr,"%*s|%14d|%13.2f|%13.2f|%16.2f|\n",
iceb_u_kolbait(48,gettext("Итого")),gettext("Итого"),
iotr,itog[0],itog[1],itog[3]);

fprintf(ff_bezr,"\
-------------------------------------------------------------------------------------------------------------\n");
double ddd=0.;
if(iotr > 0)
 ddd=itog[0]/iotr;

zarsruv_end(ddd,ff_bezr,wpredok);

if(iceb_u_sravmydat(dn,mn,gn,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
 {
  sprintf(strsql,"\
------------------------------------------------------------------------\n");
  iceb_printw(iceb_u_toutf(strsql),buffer,view);

  sprintf(strsql,"%-*.*s %-9d %10.2f %8.2f %9.2f %10.2f %12.2f\n",
  iceb_u_kolbait(7,gettext("Итого")),iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),
  iotr,
  itog[0],itog[1],itog[2],itog[3],itog[4]);

  iceb_printw(iceb_u_toutf(strsql),buffer,view);

  fprintf(ff,"\
------------------------------------------------------------------------\n");

  fprintf(ff,"%-*.*s %-9d %10.2f %8.2f %9.2f %10.2f %12.2f\n",
  iceb_u_kolbait(7,gettext("Итого")),iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),
  iotr,
  itog[0],itog[1],itog[2],itog[3],itog[4]);

  fprintf(ff,"\
------------------------------------------------------------------------\n");
 }
else
 {
  sprintf(strsql,"\
-------------------------------------------------\n");

  iceb_printw(iceb_u_toutf(strsql),buffer,view);

  sprintf(strsql,"%-*.*s %-9d %10.2f %8.2f %10.2f\n",
  iceb_u_kolbait(7,gettext("Итого")),iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),
  iotr,
  itog[0],itog[1],itog[3]);

  iceb_printw(iceb_u_toutf(strsql),buffer,view);

  fprintf(ff,"\
-------------------------------------------------\n");

  fprintf(ff,"%-*.*s %-9d %10.2f %8.2f %10.2f\n",
  iceb_u_kolbait(7,gettext("Итого")),iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),
  iotr,
  itog[0],itog[1],itog[3]);

  fprintf(ff,"\
-------------------------------------------------\n");
 }

if(iotr != 0)
 sprintf(strsql,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
 itog[1],iotr,itog[1]/iotr);
else
 sprintf(strsql,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
 itog[1],iotr,0.);

iceb_printw(iceb_u_toutf(strsql),buffer,view);



if(iotr != 0)
 fprintf(ff,"Середньоденна заробітна плата (дохід): %.2f/%d=%.2f\n",
 itog[1],iotr,itog[1]/iotr);
else
 fprintf(ff,"Середньоденна заробітна плата (дохід): %.2f/%d=%.2f\n",
 itog[1],iotr,0.);

fprintf(ff,"\
Примітка. Дата звільнення з роботи за сумісництвом________________\n\
(заповнюеться у разі видання довідки  для признячення страхових\n\
виплат, пов'язаних з безробіттям).\n\n\
Головний бухгалтер ________________________ %s\n\
(або фізична особа)\n\
 М.П.\n\
Телефон виконавця____________\n",fambuh); 

iceb_podpis(ff,wpredok);
iceb_podpis(ffprot,wpredok);
iceb_podpis(ff_bezr,wpredok);

fclose(ff);
fclose(ffprot);
fclose(ff_bezr);

data->imaf.plus(imaf);
data->naimf.plus(gettext("Расчет средней зарплаты для социальной помощи"));

data->imaf.plus(imaf_bezr);
data->naimf.plus(gettext("Справка в фонд занятости"));

data->imaf.plus(imafprot);
data->naimf.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < data->imaf.kolih(); nom++)
 iceb_ustpeh(data->imaf.ravno(nom),3,wpredok);

}
/*******************************************/
/*Заполнение шапки справки в фонд занятости*/
/*******************************************/
void zarsruv_sh(const char *inn,const char *fio,FILE *ff,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
short dt,mt,gt;
int nomer_str=0;
char stroka[1024];
char bros[512];

iceb_u_poltekdat(&dt,&mt,&gt);

sprintf(strsql,"select str from Alx where fil='zarsvfz_start.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarsvfz_start.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  nomer_str++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch(nomer_str)
   {
    case 1:
     sprintf(bros,"%02d.%02d.%d р.",dt,mt,gt);
     iceb_u_bstab(stroka,bros,1,14,1);
     break;

    case 5:
     iceb_u_bstab(stroka,fio,10,110,1);
     break;

    case 6:
     iceb_u_bstab(stroka,organ,25,110,1);
     break;
    case 7:
     iceb_u_bstab(stroka,inn,23,37,1);
     break;
   }

  fprintf(ff,"%s",stroka);

 }

}
/*******************/
/*Заполение концовки*/
/********************/
void zarsruv_end(double suma,FILE *ff,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
int nomer_str=0;
char stroka[1024];
char suma_prop[100];
double cel=0.,drob=0.;
class iceb_fioruk_rk rekruk;

memset(suma_prop,'\0',sizeof(suma_prop));
iceb_u_preobr(suma,suma_prop,0);

drob=modf(suma,&cel);

sprintf(strsql,"select str from Alx where fil='zarsvfz_end.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarsvfz_end.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch(nomer_str)
   {
    case 4:
     iceb_u_bstab(stroka,(int)cel,1,12,1);
     iceb_u_bstab(stroka,(int)drob*100,18,21,1);
     break;

    case 5:
     iceb_u_bstab(stroka,suma_prop,1,109,1);
     break;

    case 21:
     iceb_fioruk(1,&rekruk,wpredok);      
     iceb_u_bstab(stroka,rekruk.fio.ravno(),45,109,1);
     break;

    case 24:
     iceb_fioruk(2,&rekruk,wpredok);      
     iceb_u_bstab(stroka,rekruk.fio.ravno(),45,109,1);
     break;

    case 26:
     iceb_u_bstab(stroka,rekruk.telef.ravno(),48,109,1);
     break;

   }

  fprintf(ff,"%s",stroka);

 }



}
