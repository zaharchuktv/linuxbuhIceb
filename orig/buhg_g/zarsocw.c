/* $Id: zarsocw.c,v 1.21 2011-05-06 08:37:47 sasa Exp $ */
/*28.04.2011	03.04.2000	Белых А.И.	zarsocw.c
Вычисление необходимых соц отчислений на фонд зарплаты и запись их в таблицу
*/
#include <stdlib.h>
#include        <pwd.h>
#include        <errno.h>
#include        <time.h>
#include        "buhg_g.h"
#include <unistd.h>
void zarsocw_old(short mr,short gr,int tabn,FILE *ff_prot,GtkWidget*);
void zarsocw_esv(short mr,short gr,int tabn,FILE *ff_prot,GtkWidget*);

extern double   okrg; /*Округление*/
extern short	kodpenf; /*Код пенсионного фонда*/
extern short    kodpen;  /*Код пенсионных отчислений*/

extern short	kodbezf;  /*Код фонда занятости*/
extern short    kodbzr;  /*Код отчисления на безработицу*/
extern float procent_fb_dog; /*Прочент начисления на фонд зарплаты для фонда безработицы для работающий по договору*/

extern short	kodsoc;   /*Код фонда социального страхования*/
extern short    kodsocstr;  /*Код отчисления на соц-страх*/
extern short	kodfsons;   /*Код фонда страхования от несчасного случая*/

extern short	kodsoci;   /*Код социального страхования инвалидов*/
extern char	*shetb; /*Бюджетные счета*/
extern short    *kodmp;   /*Коды материальной помощи*/
extern char     *p_shet_inv; //счет для начисления пенсионных отчислений на фонд зарплаты для инвалидов (если для них нежен другой счет)
extern float     p_tarif_inv; //тариф (процент) отчисления в пенсионный фонд инвалидами
extern short    *kodbl;  /*Код начисления больничного*/
extern SQL_baza bd;

extern int kodf_esv;
extern int kodf_esv_bol;
extern int kodf_esv_inv;
extern int kodf_esv_dog;
extern int kodf_esv_vs;

extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/
extern int kod_esv_vs;
extern class iceb_u_str kod_zv_gr;

void zarsocw(short mr,short gr,int tabn,FILE *ff_prot,GtkWidget *wpredok)
{

if(iceb_u_sravmydat(1,mr,gr,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
 zarsocw_old(mr,gr,tabn,ff_prot,wpredok);
else
 zarsocw_esv(mr,gr,tabn,ff_prot,wpredok);

}
/************************************************/
void zarsocw_esv(short mr,short gr,int tabn,FILE *ff_prot,GtkWidget *wpredok)
{
char strsql[512];
SQLCURSOR cur;
int kolstr=0;
SQL_str row;
int knah=0;
double suma=0.;
float dproc_sr=0.;
float dproc_bl=0.;
float dproc_in=0.;
float dproc_dg=0.;
float dproc_vs=0.;

float proc_vr=0.; /*процент взятый в расчёт*/
int kodf_esv_vr=0;
double suma_bol=0.;
double suma_bolb=0.;
double suma_vr=0.;
double suma_vrb=0.;

class iceb_u_str knvr_vr("");
class iceb_u_str shet_vr("");
class iceb_u_str shetbu_vr(""); /*взятый в расчёт*/

class iceb_u_str knvr("");
class iceb_u_str shet("");
class iceb_u_str shetbu("");

class iceb_u_str knvr_bol("");
class iceb_u_str shet_bol("");
class iceb_u_str shetbu_bol("");

class iceb_u_str knvr_inv("");
class iceb_u_str shet_inv("");
class iceb_u_str shetbu_inv("");

class iceb_u_str knvr_dog("");
class iceb_u_str shet_dog("");
class iceb_u_str shetbu_dog("");

class iceb_u_str knvr_vs("");
class iceb_u_str shet_vs("");
class iceb_u_str shetbu_vs("");

if(ff_prot != NULL)
 fprintf(ff_prot,"Расчёт начислений единого социального взноса-%d.%d Табельный номер:%d\n",mr,gr,tabn);

//Проверяем есть ли удержание
sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv);
if(iceb_sql_readkey(strsql,wpredok) <=  0)
 {  
  sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
  datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_bol);
  if(iceb_sql_readkey(strsql,wpredok) <=  0)
   {
    sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
    datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_inv);
    if(iceb_sql_readkey(strsql,wpredok) <=  0)
    {
     sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
     datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_dog);
     if(iceb_sql_readkey(strsql,wpredok) <=  0)
     {
      sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
      datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_vs);
      if(iceb_sql_readkey(strsql,wpredok) <=  0)
       {
        if(ff_prot != NULL)
           fprintf(ff_prot,"У работника нет кода удержания социального взноса в списке его удержаний\n");
        return;
       }
     }
    }     
  }
 }

if(kodf_esv == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код фонда единого социального взноса равен нолю!\n");
  return; 
 }
kodf_esv_vr=kodf_esv;

if(kodf_esv_bol == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код фонда единого социального взноса для больничных равен нолю!\n");
  return; 
 }



/*Читаем коды не входящие в расчёт и счёта*/
sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены код фонда %d\n",kodf_esv);
  return;
 }

shet.new_plus(row[0]);
knvr.new_plus(row[1]);
shetbu.new_plus(row[2]);

if(ff_prot != NULL)
 fprintf(ff_prot,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
 kodf_esv,knvr.ravno(),shetbu.ravno());
 
/*Читаем коды не входящие в расчёт и счёта*/
sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv_bol);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены код фонда %d\n",kodf_esv_bol);
  return;
 }

shet_bol.new_plus(row[0]);
knvr_bol.new_plus(row[1]);
shetbu_bol.new_plus(row[2]);
if(ff_prot != NULL)
 fprintf(ff_prot,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
 kodf_esv_bol,knvr_bol.ravno(),shetbu_bol.ravno());

/*Читаем коды не входящие в расчёт и счёта*/
sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv_inv);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены код фонда %d\n",kodf_esv_inv);
  return;
 }

shet_inv.new_plus(row[0]);
knvr_inv.new_plus(row[1]);
shetbu_inv.new_plus(row[2]);
if(ff_prot != NULL)
 fprintf(ff_prot,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
 kodf_esv_inv,knvr_inv.ravno(),shetbu_inv.ravno());

/*Читаем коды не входящие в расчёт и счёта*/
sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv_dog);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены код фонда %d\n",kodf_esv_dog);
  return;
 }

shet_dog.new_plus(row[0]);
knvr_dog.new_plus(row[1]);
shetbu_dog.new_plus(row[2]);
if(ff_prot != NULL)
 fprintf(ff_prot,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
 kodf_esv_dog,knvr_dog.ravno(),shetbu_dog.ravno());

/*Читаем коды не входящие в расчёт и счёта*/
if(kod_esv_vs != 0)
 {
  sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv_vs);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не найдены код фонда %d\n",kodf_esv_vs);
    return;
   }

  shet_vs.new_plus(row[0]);
  knvr_vs.new_plus(row[1]);
  shetbu_vs.new_plus(row[2]);
  if(ff_prot != NULL)
   fprintf(ff_prot,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
   kodf_esv_vs,knvr_vs.ravno(),shetbu_vs.ravno());
 }
 

/*Читаем действующие на дату расчёта проценты*/
sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv,mr,gr);
  return;
 }
dproc_sr=atof(row[0]);
/*Читаем действующие на дату расчёта проценты*/
sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv_bol,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv_bol,mr,gr);
  return;
 }
dproc_bl=atof(row[0]);



if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\
Процент с работника:%.2f\n\
Процент с больничного:%.2f\n",
dproc_sr,
dproc_bl);

  if(shetb != NULL)
    fprintf(ff_prot,"\
Бюджетные счета:%s\n",
   shetb);
 }

proc_vr=dproc_sr;
knvr_vr.new_plus(knvr.ravno());
shet_vr.new_plus(shet.ravno());
shetbu_vr.new_plus(shetbu.ravno());


int metka_inv=zarprtnw(mr,gr,tabn,"zarinv.alx",ff_prot,wpredok);


if(metka_inv == 1)
 {
   if(ff_prot != NULL)
     fprintf(ff_prot,"Находится в списке инвалидов процент %.2f\n",proc_vr);
  if(kodf_esv_inv == 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Код фонда единого социального взноса для инвалидов равен нолю!\n");
    return; 
   }
  /*Читаем действующие на дату расчёта проценты*/
  sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv_inv,gr,mr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv_inv,mr,gr);
    return;
   }
  dproc_in=atof(row[0]);

  kodf_esv_vr=kodf_esv_inv;
  proc_vr=dproc_in;
  knvr_vr.new_plus(knvr_inv.ravno());
  shet_vr.new_plus(shet_inv.ravno());
  shetbu_vr.new_plus(shetbu_inv.ravno());
 }

int metka_dog=zarprtnw(mr,gr,tabn,"zardog.alx",ff_prot,wpredok); //Метка работника работающего по договору
if(metka_dog == 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Находится в списке работающих по договорам подряда\n");
  if(kodf_esv_dog == 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Код фонда единого социального взноса для договоров равен нолю!\n");
    return; 
   }
  /*Читаем действующие на дату расчёта проценты*/
  sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv_dog,gr,mr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv_dog,mr,gr);
    return;
   }

  dproc_dg=atof(row[0]);

  kodf_esv_vr=kodf_esv_dog;
  proc_vr=dproc_dg;
  knvr_vr.new_plus(knvr_dog.ravno());
  shet_vr.new_plus(shet_dog.ravno());
  shetbu_vr.new_plus(shetbu_dog.ravno());
 }

sprintf(strsql,"select zvan from Zarn where tabn=%d and god=%d and mes=%d",tabn,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
if(atoi(row[0]) != 0)
 if(iceb_u_proverka(kod_zv_gr.ravno(),row[0],0,0) != 0)
  {

   if(ff_prot != NULL)
    fprintf(ff_prot,"Военный %s != %s\n",kod_zv_gr.ravno(),row[0]);
   if(kodf_esv_vs == 0)
    {
     if(ff_prot != NULL)
      fprintf(ff_prot,"Код фонда единого социального взноса для военных равен нолю!\n");
     return; 
    }

    /*Читаем действующие на дату расчёта проценты*/
   sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv_vs,gr,mr);
   if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
    {
     if(ff_prot != NULL)
      fprintf(ff_prot,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv_dog,mr,gr);
     return;
    }

   dproc_vs=atof(row[0]);
   kodf_esv_vr=kodf_esv_vs;
   proc_vr=dproc_vs;
   knvr_vr.new_plus(knvr_vs.ravno());
   shet_vr.new_plus(shet_vs.ravno());
   shetbu_vr.new_plus(shetbu_vs.ravno());
  }

if(ff_prot != NULL)
 fprintf(ff_prot,"Берём в расчёт:\n\
Код фонда:%d\n\
Процент:%.2f\n\
Коды не входящие в расчёт:%s\n\
Счёт хозрасчётный:%s\n\
Счёт бюджетный:%s\n",
kodf_esv_vr,
proc_vr,
knvr_vr.ravno(),
shet_vr.ravno(),
shetbu_vr.ravno());

/*Блокируем таблицу*/
sprintf(strsql,"LOCK TABLES Zarsocz WRITE,Zarsoc WRITE,Zarp WRITE,Alx READ,icebuser READ");
class iceb_lock_tables flag(strsql);
  


/*Удаляем все записи если они есть*/
sprintf(strsql,"delete from Zarsocz where datz='%d-%d-01' and tabn=%d",gr,mr,tabn);

iceb_sql_zapis(strsql,1,0,wpredok);


sprintf(strsql,"select knah,suma,shet,godn,mesn from Zarp where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and tabn=%d and \
prn='1' and suma <> 0. order by knah asc",gr,mr,gr,mr,tabn);


/*
printw("\n%s\n",strsql);
refresh();
*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено ни одного начисления!");
  return;

 }

while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);

  knah=atoi(row[0]);

  suma=atof(row[1]);

  if(provkodw(kodbl,knah) >= 0 )
   {
    if(iceb_u_proverka(knvr_bol.ravno(),row[0],0,1) == 0)
     continue;
    if(iceb_u_proverka(shetb,row[2],0,1) == 0)
     suma_bolb+=suma;
    else
     suma_bol+=suma;
   }
  else
   {
    if(iceb_u_proverka(knvr_vr.ravno(),row[0],0,1) == 0)
     continue;
    if(iceb_u_proverka(shetb,row[2],0,1) == 0)
      suma_vrb+=suma;
    else
      suma_vr+=suma;
   }
 }




if(ff_prot != NULL)
 fprintf(ff_prot,"suma_bol=%.2f suma_vr=%.2f suma_bolb=%.2f suma_vrb=%.2f proc_vr=%.2f\n",suma_bol,suma_vr,suma_bolb,suma_vrb,proc_vr);
 
if(suma_bol != 0. || suma_bolb != 0.)
 {
  double sumao=suma_bol*dproc_bl/100.;
  double suma_b=suma_bolb*dproc_bl/100.;
  sumao=iceb_u_okrug(sumao,okrg);
  suma_b=iceb_u_okrug(suma_b,okrg);

  class iceb_u_str shet_zap(shet_bol.ravno());

  if(shetbu_bol.getdlinna() > 1)
   shet_zap.plus(",",shetbu_bol.ravno());
     
  sprintf(strsql,"insert into Zarsocz (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb) \
values ('%d-%d-01',%d,%d,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
  gr,mr,tabn,kodf_esv_bol,shet_zap.ravno(),suma_bol+suma_bolb,sumao+suma_b,dproc_bl,iceb_getuid(wpredok),time(NULL),suma_b,suma_bolb);
  iceb_sql_zapis(strsql,1,0,wpredok);
 }

class zar_read_tn1h nastr;
zar_read_tn1w(1,mr,gr,&nastr,ff_prot,wpredok);

if(suma_vr != 0. || suma_vrb != 0.)
 {
  if(suma_vr >  nastr.max_sum_for_soc) //Максимальная сумма с которой начисляются соц.отчисления
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Cумма %.2f > %.2f Берём в расчёт %.2f\n",suma_vr, nastr.max_sum_for_soc, nastr.max_sum_for_soc);

    suma_vr= nastr.max_sum_for_soc;

   }
  if(suma_vrb >  nastr.max_sum_for_soc) //Максимальная сумма с которой начисляются соц.отчисления
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Cумма %.2f > %.2f Берём в расчёт %.2f\n",suma_vrb, nastr.max_sum_for_soc, nastr.max_sum_for_soc);

    suma_vrb= nastr.max_sum_for_soc;

   }


  double sumao=suma_vr*proc_vr/100.;
  double suma_b=suma_vrb*proc_vr/100.;
  sumao=iceb_u_okrug(sumao,okrg);
  suma_b=iceb_u_okrug(suma_b,okrg);
  class iceb_u_str shet_zap(shet_vr.ravno());

  if(shetbu_vr.getdlinna() > 1)
   shet_zap.plus(",",shetbu_vr.ravno());
  
  sprintf(strsql,"insert into Zarsocz (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb) \
values ('%d-%d-01',%d,%d,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
  gr,mr,tabn,kodf_esv_vr,shet_zap.ravno(),suma_vr+suma_vrb,sumao+suma_b,proc_vr,iceb_getuid(wpredok),time(NULL),suma_b,suma_vrb);
  iceb_sql_zapis(strsql,1,0,wpredok);
 }
}

/*********************************************************/

void zarsocw_old(short mr,short gr,int tabn,FILE *ff_prot,GtkWidget *wpredok)
{
float		tarif,tarif1;
float		procsoc=0.;  /*Процент отчисления на соц. страхование*/
float		procsons=0.;  /*Процент страхования от несчасного случая*/
float		procbezf=0.;  /*Процент отчисления в фонд занятости*/
char   		kodnvpen[512]; /*Коды не входящие в расчет пенсионного отчисления*/
char		kodnvbez[512]; /*Коды не входящие в расчет отчисления в фонд занятости*/
char 		kodnvsoc[512]; /*Коды начислений не входящие в расчет для соц. страхования*/
char		kodnvsons[512]; /*Коды начислений не входящие в расчет для соц. страхования от несчасного случая*/
char		shetpen[40];
char		shetbez[40];
char		shetsoc[40];
char		shetsons[40]; //Счет страхования от несчасного случая
SQL_str         row;
char		strsql[512];
long		kolstr;
double		suma;
double		sumapen,sumapenb;
double		sumasoc,sumasocb;
double		sumasoci,sumasocib;
double		sumasons,sumasonsb;
double		sumabez,sumabezb;
short		metka=0;
time_t		vrem;
double		bb,bbb;
short		knah;
char		bros[512];
double		nmatpom=0.;  //для пенсионного 
double suma_boln_ntm=0.; //сумма больничных не текущего месяца

SQLCURSOR curr;
if(ff_prot != NULL)
 fprintf(ff_prot,"%s-Расчёт отчислений в соц. фонды-%d.%d Табельный номер:%d\n",__FUNCTION__,mr,gr,tabn);
//refresh();

/*Блокируем таблицу*/
sprintf(strsql,"LOCK TABLES Zarsocz WRITE,Zarsoc WRITE,Zarp WRITE,Alx READ,icebuser READ,Blok READ");
class iceb_lock_tables lock(strsql);
  

SQLCURSOR cur;

/*Удаляем все записи если они есть*/
sprintf(strsql,"delete from Zarsocz where datz='%d-%d-01' and \
tabn=%d",gr,mr,tabn);

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_NO_SUCH_TABLE)  //Нет такой таблицы
    return;
  if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Доступ только на чтение
    return;

  sprintf(bros,"zarsoc-%s",gettext("Ошибка удаления записи !"));
  iceb_msql_error(&bd,bros,strsql,wpredok);

 }


sprintf(strsql,"select knah,suma,shet,godn,mesn from Zarp where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and tabn=%d and \
prn='1' and suma <> 0. order by knah asc",gr,mr,gr,mr,tabn);


/*
printw("\n%s\n",strsql);
refresh();
*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr == 0)
  return;


//printw("\nkodmp=%d\n",kodmp[1]);
//nmatpom=matpom1(tabn,mr,gr,kodmp,gettext("Матеpиальная помощь"),NULL,1,&matpomb,1);

//printw("\nnmatpom=%.2f\n",nmatpom);
//OSTANOV();

memset(shetpen,'\0',sizeof(shetpen));
memset(kodnvpen,'\0',sizeof(kodnvpen));
tarif1=0.;
sprintf(strsql,"select proc,kodn,shet from Zarsoc where kod=%d",kodpenf);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
 {  
  tarif1=atof(row[0]);
  strncpy(kodnvpen,row[1],sizeof(kodnvpen)-1);
  strncpy(shetpen,row[2],sizeof(shetpen)-1);
 }

 
memset(shetbez,'\0',sizeof(shetbez));
memset(kodnvbez,'\0',sizeof(kodnvbez));
procbezf=0.;
sprintf(strsql,"select proc,kodn,shet from Zarsoc where kod=%d",kodbezf);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
 {
  procbezf=atof(row[0]);
  strncpy(kodnvbez,row[1],sizeof(kodnvbez)-1);
  strncpy(shetbez,row[2],sizeof(shetbez)-1);
 }

memset(shetsoc,'\0',sizeof(shetsoc));
memset(kodnvsoc,'\0',sizeof(kodnvsoc));
procsoc=0.;
sprintf(strsql,"select proc,kodn,shet from Zarsoc where kod=%d",kodsoc);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
 {
  procsoc=atof(row[0]);
  strncpy(kodnvsoc,row[1],sizeof(kodnvsoc)-1);
  strncpy(shetsoc,row[2],sizeof(shetsoc)-1);
 }


memset(shetsons,'\0',sizeof(shetsons));
memset(kodnvsons,'\0',sizeof(kodnvsons));
procsons=0.;
sprintf(strsql,"select proc,kodn,shet from Zarsoc where kod=%d",kodfsons);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
 {
  procsons=atof(row[0]);
  strncpy(kodnvsons,row[1],sizeof(kodnvsons)-1);
  strncpy(shetsons,row[2],sizeof(shetsons)-1);
 }


sumapen=sumasoc=sumasoci=sumabez=0.;
sumapenb=sumasocb=sumasocib=sumabezb=0.;
sumasons=sumasonsb=0.;

metka=zarprtnw(mr,gr,tabn,"zarinv.alx",ff_prot,wpredok); /*для инвалидов 4%*/
if(metka == 1 && ff_prot != NULL)
 fprintf(ff_prot,"Находится в списке инвалидов\n");
/************
iceb_imafn("zarpens.alx",&imaf_nast);
int metkapens=zarprtnw(mr,gr,tabn,imaf_nast.ravno(),ff_prot,wpredok);
if(metkapens == 1 && ff_prot != NULL)
 fprintf(ff_prot,"Находится в списке пенсионеров\n");
*************/
int metka_dog=zarprtnw(mr,gr,tabn,"zardog.alx",ff_prot,wpredok); /*Метка работника работающего по договору*/
if(metka_dog == 1 && ff_prot != NULL)
 fprintf(ff_prot,"Находится в списке работающих по договорам подряда\n");

while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
   
  knah=atoi(row[0]);

  suma=atof(row[1]);

//  double budget=0.;
//  double suma_s_nds=knnfndsw(1,knah,"",suma,&budget,NULL);
//  rudnfvw(&suma,suma_s_nds,NULL);
/*************
  if(provkodw(knnf,knah) >= 0) //увеличиваем сумму для неденежных форм начисления зарплаты
    rudnfvw(&suma,suma,NULL);
*************/  
  /*Проверяем на коды не входящие в расчет пенсионного*/
  if(iceb_u_proverka(kodnvpen,row[0],0,1) != 0)
   {
    sumapen+=suma;
    if(iceb_u_proverka(shetb,row[2],0,1) == 0)
      sumapenb+=suma;
   }
  //Проверяем на больничные не текущего месяца
  if(provkodw(kodbl,knah) >= 0 && (gr != atoi(row[3]) || mr != atoi(row[4])) )
   {
    suma_boln_ntm+=suma;
   }
  
  /*Проверяем на коды не входящие в расчет соц. страх*/
  if(iceb_u_proverka(kodnvsoc,row[0],0,1) != 0)
   {
    sumasoc+=suma;
    if(iceb_u_proverka(shetb,row[2],0,1) == 0)
      sumasocb+=suma;

   }

  /*Проверяем на коды не входящие в расчет безработицы*/

  if(iceb_u_proverka(kodnvbez,row[0],0,1) != 0)
   {
    sumabez+=suma;
    if(iceb_u_proverka(shetb,row[2],0,1) == 0)
      sumabezb+=suma;
   }

  /*
  printw("shetbezb=%s row[2]=%s sumabez=%.2f sumabezb=%.2f\n",
  shetbezb,row[2],sumabez,sumabezb);
  OSTANOV();
  */

  /*Проверяем на коды не входящие в расчет соц. страх от несчасного случая*/
  if(iceb_u_proverka(kodnvsons,row[0],0,1) != 0)
   {
    sumasons+=suma;
    if(iceb_u_proverka(shetb,row[2],0,1) == 0)
      sumasonsb+=suma;

   }


 } 

time(&vrem);
class zar_read_tn1h nastr;
zar_read_tn1w(1,mr,gr,&nastr,ff_prot,wpredok);

/*Делаем записи в таблицу отчислений в социальные фонды*/
if(sumapen != 0. && kodpenf != 0)
 {
  sumapen-=nmatpom;
  char shetz[30];
    
  //Проверяем если ли удержание в пенсионный фонд
  sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",
  gr,mr,gr,mr,tabn,kodpen);
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   {  
    sprintf(strsql,"select kod from Zarsoc where kod=%d",kodpenf);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     {  
      if(sumapen-suma_boln_ntm > nastr.max_sum_for_soc && nastr.max_sum_for_soc != 0)
       sumapen=nastr.max_sum_for_soc;

      memset(shetz,'\0',sizeof(shetz));     
      strcpy(shetz,shetpen);
      if(metka != 1)
       {
        tarif=tarif1;
       }
      else
       {
        if(p_shet_inv != NULL)
        if(p_shet_inv[0] != '\0')
         {
          memset(shetz,'\0',sizeof(shetz));     
          strcpy(shetz,p_shet_inv);
         }
        tarif=p_tarif_inv;
       }        
      bb=sumapen*tarif/100.;
      bbb=sumapenb*tarif/100.;
    /*
      printw("\nmetka=%d tarif1=%f trif2=%f bb=%f sumapen=%f\n",
      metka,tarif1,p_tarif_inv,bb,sumapen);
      OSTANOV(); 
    */

      bb=iceb_u_okrug(bb,okrg);
      bbb=iceb_u_okrug(bbb,okrg);
            
      sprintf(strsql,"insert into Zarsocz (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb)  \
values ('%d-%d-01',%d,%d,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
      gr,mr,tabn,kodpenf,shetz,sumapen,bb,tarif,iceb_getuid(wpredok),vrem,bbb,sumapenb);
      if(sql_zap(&bd,strsql) != 0)
       {
        sprintf(bros,"zarsoc-%s",gettext("Ошибка записи !"));
        iceb_msql_error(&bd,bros,strsql,wpredok);
       }
     }
   }
  else
   if(ff_prot != NULL)
    fprintf(ff_prot,"Нет отчисления в пенсионный фонд в удержаниях работника\n");
 }

if(sumasoc != 0. && kodsoc != 0)
 {
  sumasoc-=nmatpom;
//  printw("sumasoc=%f kodsoc=%d kodsocstr=%d\n",sumasoc,kodsoc,kodsocstr);
//  OSTANOV();
  //Проверяем если ли удержание на соцстрах
  sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",
  gr,mr,gr,mr,tabn,kodsocstr);
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   {  
    sprintf(strsql,"select kod from Zarsoc where kod=%d",kodsoc);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     {  
      if(sumasoc > nastr.max_sum_for_soc && nastr.max_sum_for_soc != 0)
       sumasoc=nastr.max_sum_for_soc;
      bb=sumasoc*procsoc/100.;
      bb=iceb_u_okrug(bb,okrg);

      bbb=sumasocb*procsoc/100.;
      bbb=iceb_u_okrug(bbb,okrg);
    /*
      printw("\nsumasoc=%f procsoc=%f bb=%f\n",
      sumasoc,procsoc,bb);
      OSTANOV();
    */  
      sprintf(strsql,"insert into Zarsocz (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb)  \
values ('%d-%d-01',%d,%d,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
      gr,mr,tabn,kodsoc,shetsoc,sumasoc,bb,procsoc,iceb_getuid(wpredok),vrem,bbb,sumasocb);
      if(sql_zap(&bd,strsql) != 0)
       {
        sprintf(bros,"zarsoc-%s",gettext("Ошибка записи !"));
        iceb_msql_error(&bd,bros,strsql,wpredok);
       }
     }
   }
  else
   if(ff_prot != NULL)
    fprintf(ff_prot,"Нет отчисления в фонд соц. страха удержаниях работника\n");
 }
//Фонд страхованія от несчасного случая
if(sumasons != 0. && kodfsons != 0)
 {
  sumasons-=nmatpom;
  //Начисления с человека по этому фонду нет поэтому проверяем чтобы код фонда был в списке фондов

  sprintf(strsql,"select kod from Zarsoc where kod=%d",kodfsons);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   {  
    
    if(sumasons > nastr.max_sum_for_soc && nastr.max_sum_for_soc != 0)
       sumasons=nastr.max_sum_for_soc;
    bb=sumasons*procsons/100.;
    bb=iceb_u_okrug(bb,okrg);

    bbb=sumasonsb*procsons/100.;
    bbb=iceb_u_okrug(bbb,okrg);
  /*
    printw("\nsumasoc=%f procsoc=%f bb=%f\n",
    sumasoc,procsoc,bb);
    OSTANOV();
  */  
    sprintf(strsql,"insert into Zarsocz (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb)  \
values ('%d-%d-01',%d,%d,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
    gr,mr,tabn,kodfsons,shetsons,sumasons,bb,procsons,iceb_getuid(wpredok),vrem,bbb,sumasonsb);
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zarsoc-%s",gettext("Ошибка записи !"));
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }
 }


if(sumabez != 0. && kodbezf != 0)
 {
  sumabez-=nmatpom;

  
  //Проверяем если ли удержание на безработицу
  if(metka_dog == 0)
   {
    sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",
    gr,mr,gr,mr,tabn,kodbzr);
    if(iceb_sql_readkey(strsql,&row,&curr,wpredok) <= 0)
     {  
      if(ff_prot != NULL)
       fprintf(ff_prot,"Нет отчисления в фонд безработицы в удержаниях работника\n");
     }
   }

  sprintf(strsql,"select kod from Zarsoc where kod=%d",kodbezf);
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
   {  
    float proc=procbezf;
    if(iceb_u_sravmydat(1,mr,gr,1,1,2009) >= 0)
     if(metka_dog != 0)
      {
       proc=procent_fb_dog;
       if(ff_prot != NULL)
        fprintf(ff_prot,"Для работающих по договорам подряда применяется процент:%f\n",proc);
      }
    if(sumabez > nastr.max_sum_for_soc && nastr.max_sum_for_soc != 0)
     sumabez=nastr.max_sum_for_soc;
    bb=sumabez*proc/100.;
    bb=iceb_u_okrug(bb,okrg);

    bbb=sumabezb*proc/100.;
    bbb=iceb_u_okrug(bbb,okrg);
        
    sprintf(strsql,"insert into Zarsocz (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb)  \
values ('%d-%d-01',%d,%d,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
    gr,mr,tabn,kodbezf,shetbez,sumabez,bb,proc,iceb_getuid(wpredok),vrem,bbb,sumabezb);
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zarsoc-%s",gettext("Ошибка записи !"));
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }
 }


}

