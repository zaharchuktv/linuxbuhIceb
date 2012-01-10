/* $Id: zarsprprw.c,v 1.18 2011-03-28 06:55:46 sasa Exp $ */
/*11.11.2010    31.08.1994      Белых А.И.      zarsprprw.c
получение справки всех начислений и удержаний
за любой период
*/
#include <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        "buhg_g.h"
#include <unistd.h>
#include "zar_sprav.h"

void             rekvizkon(SQL_str,FILE*);
void zarsprprw_sap(short mn,short gn,short mk,short gk,const char *harac,const char *fam,const char *inn,const char *dolg,const char *podr,short sovm,FILE *ff1);
int zarsprpr_sub_start(short god,const char *kedrpou,const char *fio,const char *inn,const char *dolg,FILE *ff_ras,GtkWidget *wpredok);
int zarsprpr_sub_end(short mn,short gn,short mk,short gk,double suma,FILE *ff_ras,GtkWidget *wpredok);

extern short    ddd,mmm,ggg;
extern char     *organ;/*Наименование организации*/
//extern short    kodpn;   /*Код подоходного налога*/
extern class iceb_u_str kodpn_all;
extern short    kodpen;  /*Код пенсионных отчислений*/
extern short    *kodmp;   /*Коды материальной помощи*/
extern double   snmpz; /*Сумма не облагаемой материальной помощи заданная*/
extern char     *shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short    kodbzr;  /*Код отчисления на безработицу*/
extern short	*obud; /*Обязательные удержания*/
extern short    kodsocstr;  /*Код отчисления на соц-страх*/
extern SQL_baza bd;

void zarsprprw(class zar_sprav_rek *rek,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
struct  tm      *bf;
time_t		tmr;
char            bros[512];
char            str[1024];
short           koltb; /*Количество видов табелей*/
FILE            *ff,*ff1,*ff2,*ff3;
char            imaf[24],imaf1[24],imaf2[24],imafpen[24],imafshet[40];
short           m,g;
double          inah,iuder;
double		inahb; //Итого начислено без кодов не входящих в расчет подоходного налога
double          ino,inob,iuo,bb;
short           kon;     /*Количество статей начисления*/
short           kou;     /*Количество статей удержания*/
char            nai[512];
long            ll;
int             i,i1;
double		peno;  /*Пенсионное отчисление*/
double		ipeno;  /*Итого Пенсионное отчисление*/
double          podoh; /*Подоходный налог за месяц*/
double          ipodoh; /*Подоходный налог за год*/
char            fam[512];
char            fambuh[512];
double          sld;
double          skm,iskm;
double          alim,ialim;
double		fbezr,ifbezr;
short           idn,ikdn;
double          ihas;
short           fo;
short           sovm; /*0-нет 1- Совместитель*/
double		sal;
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
int		kodt;
float		dnei,has,kdnei;
short		prn;
int		knah;
double		sym;
short		dz,mz,gz;
char		inn[24];
short		ots=5;
char		dolg[512];
char		mescc[24];
short		kodalim;
double		saldb;
char		harac[40];
char		shet[32];
char		kodnvrspr[312];
double          soc_strax=0.;
double          isoc_strax=0.;
class iceb_u_str kedrpou("");
char naim_mes[24];
short mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,rek->datan.ravno());
iceb_u_rsdat1(&mk,&gk,rek->datak.ravno());

SQLCURSOR curr;
sprintf(strsql,"select fio,sovm,inn,dolg,harac,podr from Kartb where tabn=%d",rek->tabnom.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),rek->tabnom.ravno_atoi());
  iceb_menu_soob(strsql,wpredok);
  return;
 }


char podr[24];
strncpy(podr,row[5],sizeof(podr)-1);
strncpy(fam,row[0],sizeof(fam)-1);
strncpy(dolg,row[3],sizeof(dolg)-1);
memset(harac,'\0',sizeof(harac));
strncpy(harac,row[4],sizeof(harac)-1);

sovm=0;
if(row[1][0] == '1')
     sovm=1;
strncpy(inn,row[2],sizeof(inn)-1);

fo=iceb_menu_danet(gettext("Развёрнутый отчёт ?"),2,wpredok);


bros[0]='\0';
iceb_poldan("Код удержания алиментов",bros,"zaralim.alx",wpredok);
kodalim=atoi(bros);

kodnvrspr[0]='\0';
iceb_poldan("Коды начислений не входящие в справку о доходах",kodnvrspr,"zarnast.alx",wpredok);

sprintf(strsql,"%d %s.\n",rek->tabnom.ravno_atoi(),fam);
iceb_printw(iceb_u_toutf(strsql),buffer,view);

/*Определяем количество начислений и удержаний*/
sprintf(strsql,"select kod from Nash");
class SQLCURSOR cur;
if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kon == 0)
 {
  iceb_menu_soob(gettext("Не введены начисления !"),wpredok);
  return;
 }

class iceb_u_int MKN; //Массив кодов начислений
i=0;
while(cur.read_cursor(&row) != 0)
  MKN.plus(atoi(row[0]),-1);

sprintf(strsql,"select kod from Uder");

if((kou=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kou == 0)
 {
  iceb_menu_soob(gettext("Не введены удержания !"),wpredok);
  return;
 }

class iceb_u_int MKU; //Массив кодов удержаний

i=0;
while(cur.read_cursor(&row) != 0)
  MKU.plus(atoi(row[0]),-1);

sprintf(strsql,"select kod from Tabel");

if((koltb=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(koltb == 0)
 {
  iceb_menu_soob(gettext("Не введены виды табеля !"),wpredok);
  return;
 }

class iceb_u_int MKVT; //Массив кодов видов табеля

i=0;
while(cur.read_cursor(&row) != 0)
  MKVT.plus(atoi(row[0]),-1);

ll=kon;
class iceb_u_double MSKN; //Массив содержимого кодов начислений
class iceb_u_double MSKU; //Массив содержимого кодов удержаний
MSKN.make_class(kon);
MSKU.make_class(kou);

class iceb_u_double DNI; //Массив количества рабочих дней
class iceb_u_double KDNI; //Массив количества календарных дней
class iceb_u_double HASI; //Массив количества отработанных часов
DNI.make_class(koltb);
KDNI.make_class(koltb);
HASI.make_class(koltb);

double suma_obud=0.; //Сумма обязательных удержаний за месяц
double suma_obudi=0.; //Сумма обязательных удержаний за период

char imaf_sub[50];

FILE *ff_sub;
sprintf(imaf_sub,"sub%d.lst",getpid());
if((ff_sub = fopen(imaf_sub,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_sub,"",errno,wpredok);
  return;
 }

sprintf(imaf,"sp1%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

sprintf(imaf1,"sp2%d.lst",getpid());
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }

sprintf(imaf2,"sp3%d.lst",getpid());
if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,wpredok);
  return;
 }

sprintf(imafpen,"sp4%d.lst",getpid());
if((ff3 = fopen(imafpen,"w")) == NULL)
 {
  iceb_er_op_fil(imafpen,"",errno,wpredok);
  return;
 }


char imaf_podoh[50];
FILE *ff_podoh;
sprintf(imaf_podoh,"spodoh%d.lst",getpid());
if((ff_podoh = fopen(imaf_podoh,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_podoh,"",errno,wpredok);
  return;
 }


/*Читаем реквизиты организации*/
sprintf(bros,"select * from Kontragent where kodkon='00'");
if(iceb_sql_readkey(bros,&row,&cur,wpredok) != 1)
 {
  iceb_menu_soob(gettext("Не найден код контрагента 00 !"),wpredok);
 }
else
 {
  rekvizkon(row,ff1);
  rekvizkon(row,ff2);
  rekvizkon(row,ff3);
  rekvizkon(row,ff_podoh);
  kedrpou.new_plus(row[5]);
 }

zarsprpr_sub_start(gn,kedrpou.ravno(),fam,inn,dolg,ff_sub,wpredok);

zarsprprw_sap(mn,gn,mk,gk,harac,fam,inn,dolg,podr,sovm,ff1);


fprintf(ff1,"\
--------------------------------------------------------------------------------------\n");
fprintf(ff1,gettext("Месяц  | Начислено |В том числе|В том числе|Отчисления |Отчисл.в|Отчисл.в|Подоходный |\n\
       |           |благ.помощь|  алименты |в фонд без.|пенс.ф. |ф.соц-ст|   налог   |\n"));
fprintf(ff1,"\
--------------------------------------------------------------------------------------\n");

zarsprprw_sap(mn,gn,mk,gk,harac,fam,inn,dolg,podr,sovm,ff3);

fprintf(ff3,"\
---------------------------------------------------------\n");
fprintf(ff3,gettext("Месяц  |   Начислено   |В том числе    |Пенсионное отчи-|\n\
       |               |мат. помощь    |   сление       |\n"));
fprintf(ff3,"\
---------------------------------------------------------\n");

zarsprprw_sap(mn,gn,mk,gk,harac,fam,inn,dolg,podr,sovm,ff_podoh);
fprintf(ff_podoh,"\
-----------------------------------------\n");
fprintf(ff_podoh,gettext("\
Месяц  |   Начислено   |Подоходный налог|\n"));
fprintf(ff_podoh,"\
-----------------------------------------\n");

zarsprprw_sap(mn,gn,mk,gk,harac,fam,inn,dolg,podr,sovm,ff2);

fprintf(ff2,"\
%*s---------------------------------------------\n\
%*s%s\n\
%*s---------------------------------------------\n",
ots," ",
ots," ",gettext("|  Дата |   Начислено   |Удержано| Получено |"),
ots," ");


if(fo == 1)
  fprintf(ff,"%d %s\n\
%s %d.%d%s %s %d.%d%s\n",rek->tabnom.ravno_atoi(),fam,
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("до"),
mk,gk,
gettext("г."));

sprintf(strsql,"\n%d %s\n",rek->tabnom.ravno_atoi(),fam);
iceb_printw(iceb_u_toutf(strsql),buffer,view);

//Создаем класс
class iceb_u_spisok NASH;
class iceb_u_double NASHD;
class iceb_u_spisok UDER;
class iceb_u_double UDERD;

ipodoh=ipeno=ihas=iskm=ialim=ifbezr=sld=ino=inob=iuo=0.;
idn=ikdn=0;
m=mn;
for(g=gn; g <=gk ; g++)
for(;  ; m++)
 {
  if(g == gk && m > mk)
   {
    break;
   }
  if(m == 13)
   {
    m=1;
    break;
   }


  soc_strax=peno=podoh=0.;
  if(fo == 1)
   fprintf(ff,"\
==========================================================\n\
%d.%d%s\n",m,g,gettext("г."));

  /*Ищем табель за месяц*/
  sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and \
  mes=%d and tabn=%d",g,m,rek->tabnom.ravno_atoi());
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }
  suma_obud=0.;
  if(kolstr != 0)
  while(cur.read_cursor(&row) != 0)
   {
    kodt=atoi(row[0]);
    dnei=atof(row[1]);
    has=atof(row[2]);
    kdnei=atof(row[3]);
    
    /*Читаем наименование табеля*/
    memset(nai,'\0',sizeof(nai));
    sprintf(strsql,"select naik from Tabel where kod=%d",
    kodt);
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      strncpy(nai,row1[0],sizeof(nai)-1);
    
    if(fo == 1)
     fprintf(ff,"%d %-*s %s%2.f %s%3.2f %s%2.f\n",
     kodt,iceb_u_kolbait(40,nai),nai,
     gettext("д"),dnei,
     gettext("ч"),has,
     gettext("кд"),kdnei);

    if((i=MKVT.find(kodt)) != -1)
     {
      if((i=MKVT.find(kodt)) == -1)
       {
        sprintf(strsql,"%s %d %s ТН %d %s %d.%d%s\n",
        gettext("Не найден код табеля"),kodt,
        gettext("для"),rek->tabnom.ravno_atoi(),
        gettext("за"),m,g,
        gettext("г."));

        iceb_printw(iceb_u_toutf(strsql),buffer,view);

        if(fo == 1)
         fprintf(ff,"%s %d %s ТН %d %s %d.%d%s\n",
         gettext("Не найден код табеля"),kodt,
         gettext("для"),rek->tabnom.ravno_atoi(),
         gettext("за"),m,g,
         gettext("г."));
       }
      else
       {
        DNI.plus((int)dnei,i);
        KDNI.plus((int)kdnei,i);
        HASI.plus(has,i);
       }
     }
   }

  if(fo == 1)
    fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

  sprintf(strsql,"%d.%d%s\n",m,g,gettext("г."));

  iceb_printw(iceb_u_toutf(strsql),buffer,view);

  sal=zarsaldw(1,m,g,rek->tabnom.ravno_atoi(),&saldb,wpredok);

  sprintf(strsql,"select prn,knah,suma,datz,mesn,godn,kom,shet from \
Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d \
and suma <> 0. order by prn,knah asc",
  g,m,g,m,rek->tabnom.ravno_atoi());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }


  if(m == 1)
   {
    sld=sal;
   }

  fbezr=alim=skm=inah=iuder=inahb=0.;

  if(kolstr != 0)
  while(cur.read_cursor(&row) != 0)
   {
    prn=atoi(row[0]);
    knah=atoi(row[1]);
    sym=atof(row[2]);
    iceb_u_rsdat(&dz,&mz,&gz,row[3],2);
    if(prn == 1)
     {
      sprintf(strsql,"%d,%s",knah,row[7]);
      if((i1=NASH.find(strsql)) == -1)
        NASH.plus(strsql);
      NASHD.plus(sym,i1);

      if((i=MKN.find(knah)) != -1)
        MSKN.plus(sym,i);

      inah+=sym;
      if(iceb_u_proverka(kodnvrspr,row[1],0,1) != 0)
        inahb+=sym;

      if(provkodw(kodmp,knah) >= 0)
       {
	skm+=sym;
	iskm+=sym;
       }

     }

    if(prn == 2)
     {
      sprintf(strsql,"%d,%s",knah,row[7]);
      if((i1=UDER.find(strsql)) == -1)
       {
        UDER.plus(strsql);
        UDERD.plus(sym,-1);
       }      
      else
       {
        UDERD.plus(sym,i1);
       }
      if((i=MKU.find(knah)) != -1)
        MSKU.plus(sym,i);

      iuder+=sym;
//      if(knah == kodpn)
      if(iceb_u_proverka(kodpn_all.ravno(),knah,0,1) == 0)
	podoh+=sym;
      if(knah == kodpen)
	peno+=sym;

      if(knah == kodsocstr)
       soc_strax+=sym;
       
      if(knah == kodalim)
       {
	alim+=sym;
	ialim+=sym;
       }

      if(knah == kodbzr)
       {
	fbezr+=sym;
	ifbezr+=sym;
       }

      if(provkodw(obud,knah) >= 0)
       {
        suma_obud+=sym;
        suma_obudi+=sym;
       }
     }

    if(fo == 1)
     {
      /*Читаем наименование начисления*/
      if(prn == 1)
        sprintf(strsql,"select naik from Nash where kod=%d",knah);
      if(prn == 2)
        sprintf(strsql,"select naik from Uder where kod=%d",knah);
      memset(nai,'\0',sizeof(nai));   
      if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
        strncpy(nai,row1[0],sizeof(nai)-1);

      fprintf(ff,"%3d/%02d/%2s.%s %-*s : %10.2f %-*s %s\n",
      knah,dz,row[4],row[5],iceb_u_kolbait(30,nai),nai,sym,
      iceb_u_kolbait(6,row[7]),row[7],row[6]);

     }

   }


  sprintf(strsql,"select tabn from Zarn where god=%d and mes=%d and \
 tabn=%d",g,m,rek->tabnom.ravno_atoi());
     
  if(iceb_u_strstrm(harac,gettext("Студент")) == 0)
   {
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     {
      fprintf(ff1,"%02d.%d %11.2f %11.2f %11.2f %11.2f %8.2f %8.2f %11.2f\n",m,g,inahb,skm,alim*(-1),fbezr*-1,peno*-1,soc_strax*-1,podoh*(-1));
      fprintf(ff3,"%02d.%d %15.2f %15.2f %15.2f\n",m,g,inahb,skm,peno*(-1));
      fprintf(ff_podoh,"%02d.%d %15.2f %15.2f\n",m,g,inahb,podoh*(-1));
      fprintf(ff2,"%*s|%02d.%d|%15.2f|%8.2f|%10.2f|\n",ots," ",m,g,inahb,suma_obud,inahb+suma_obud);
     }
   }
  else
   {
    fprintf(ff1,"%02d.%d %11.2f %11.2f %11.2f %11.2f %8.2f %8.2f %11.2f\n",m,g,inahb,skm,alim*(-1),fbezr*-1,peno*-1,soc_strax*-1,podoh*(-1));
    fprintf(ff3,"%02d.%d %15.2f %15.2f %15.2f\n",m,g,inahb,skm,peno*(-1));
    fprintf(ff3,"%02d.%d %15.2f %15.2f\n",m,g,inahb,podoh*(-1));
    fprintf(ff2,"%*s|%02d.%d|%15.2f|%8.2f|%10.2f|\n",ots," ",m,g,inahb,suma_obud,inahb+suma_obud);
   }
  memset(naim_mes,'\0',sizeof(naim_mes));
  iceb_mesc(m,0,naim_mes);

  fprintf(ff_sub,"|%-*.*s|%20.2f|%18.2f|%20.2f|\n",
  iceb_u_kolbait(8,naim_mes),iceb_u_kolbait(8,naim_mes),naim_mes,
  inahb,0.,alim*(-1));

  fprintf(ff_sub,"\
-----------------------------------------------------------------------\n");  
  
  if(fo == 1)
    fprintf(ff,"\
     %-*s %15.2f\n\
     %-*s %15.2f\n\
     %-*s %15.2f\n\
     %-*s %15.2f\n",
     iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),sal,
     iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),inah,
     iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),iuder,
     iceb_u_kolbait(20,gettext("Cальдо конечное")),gettext("Cальдо конечное"),inah+iuder+sal);

  ino+=inah;
  inob+=inahb;
  iuo+=iuder;
  ipodoh+=podoh;
  ipeno+=peno;
  isoc_strax+=soc_strax;
 }

sprintf(strsql,"\
-------------------------------------------------\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n",
iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),sld,
iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),ino,
iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),iuo,
iceb_u_kolbait(20,gettext("Cальдо конечное")),gettext("Cальдо конечное"),ino+iuo+sld);

iceb_printw(iceb_u_toutf(strsql),buffer,view);

fprintf(ff_sub,"|%*.*s|%20.2f|%18.2f|%20.2f|\n",
iceb_u_kolbait(8,gettext("Итого")),
iceb_u_kolbait(8,gettext("Итого")),
gettext("Итого"),
ino,0.,ialim*(-1));

fprintf(ff_sub,"\
-----------------------------------------------------------------------\n");  

zarsprpr_sub_end(mn,gn,mk,gk,ino+ialim,ff_sub,wpredok);


memset(str,'\0',sizeof(str));
iceb_u_preobr(inob,str,0);


fprintf(ff1,"\
--------------------------------------------------------------------------------------\n");

fprintf(ff1,"%*s %11.2f %11.2f %11.2f %11.2f %8.2f %8.2f %11.2f\n\n\
%s:\n%-*.*s\n",
iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),inob,iskm,ialim*(-1),ifbezr*-1,ipeno*-1,isoc_strax*-1,ipodoh*(-1),
gettext("Общая сумма доходов"),
iceb_u_kolbait(60,str),iceb_u_kolbait(60,str),str);

fprintf(ff3,"==========================================================\n\
%*s %15.2f %15.2f %15.2f\n\n\
%s:\n%-.*s\n",
iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),inob,iskm,ipeno*(-1),
gettext("Общая сумма доходов"),
iceb_u_kolbait(60,str),str);


fprintf(ff_podoh,"\
-----------------------------------------\n");
fprintf(ff_podoh,"%*s %15.2f %15.2f\n\n\
%s:\n%-.*s\n",
iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),inob,ipodoh*(-1),
gettext("Общая сумма доходов"),
iceb_u_kolbait(60,str),str);

if(iceb_u_strlen(str) > 60)
 {
  fprintf(ff1,"%s\n",iceb_u_adrsimv(60,str));
  fprintf(ff3,"%s\n",iceb_u_adrsimv(60,str));
  fprintf(ff_podoh,"%s\n",iceb_u_adrsimv(60,str));
 }

fprintf(ff2,"\
%*s---------------------------------------------\n\
%*s %*s %15.2f %8.2f %10.2f\n\n\
%s:\n%s\n",
ots," ",
ots," ",
iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),
inob,suma_obudi,inob+suma_obudi,
gettext("Общая сумма доходов"),str);


bros[0]='\0';
iceb_poldan("Табельный номер руководителя",bros,"zaralim.alx",wpredok);
if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
   strncpy(bros,row[0],sizeof(bros)-1);
 }

memset(fambuh,'\0',sizeof(fam));
iceb_poldan("Табельный номер бухгалтера",fambuh,"zaralim.alx",wpredok);
if(fambuh[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",fambuh);
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
   strncpy(fambuh,row[0],sizeof(fambuh)-1);
 }


time(&tmr);
bf=localtime(&tmr);
memset(mescc,'\0',sizeof(mescc));
iceb_mesc(bf->tm_mon+1,1,mescc);

fprintf(ff1,"\n\
Прибутковий податок і відрахування в соціальні фонди утримані і перераховані повністю.\n");

fprintf(ff1,"\n\n\
    %-*s %s\n\n\
    %-*s %s\n\n",
iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),bros,
iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),fambuh);

fprintf(ff1,"\
    %02d.%02d.%d%s\n",
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));


fprintf(ff3,"\n\n\
    %-*s %s\n\n\
    %-*s %s\n\n\
    %02d.%02d.%d%s\n",
iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),bros,
iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),fambuh,
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));

fprintf(ff_podoh,"\n\n\
    %-*s %s\n\n\
    %-*s %s\n\n\
    %02d.%02d.%d%s\n",
iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),bros,
iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),fambuh,
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));

fprintf(ff2,"\n\n\
    %-*s %s\n\n\
    %-*s %s\n\n\
    %02d.%02d.%d%s\n",
iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),bros,
iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),fambuh,
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));


if(fo == 1)
  fprintf(ff,"\
-------------------------------------------------\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n",
iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),sld,
iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),ino,
iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),iuo,
iceb_u_kolbait(20,gettext("Cальдо конечное")),gettext("Cальдо конечное"),ino+iuo+sld);


fprintf(ff,"%d %s\n\
%s %d.%d%s %s %d.%d%s\n",rek->tabnom.ravno_atoi(),fam,
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."));


sprintf(strsql,"\n%s:\n",gettext("Начисления"));
iceb_printw(iceb_u_toutf(strsql),buffer,view);

fprintf(ff,"\n%s:\n",gettext("Начисления"));
bb=0.;
for(i=0 ; i<kon;i++)
 {
  knah=MKN.ravno(i);
  sym=MSKN.ravno(i);
  if(sym == 0.)
    continue;
  sprintf(strsql,"select naik from Nash where kod=%d",knah);
  memset(nai,'\0',sizeof(nai));
     
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(nai,row1[0],sizeof(nai)-1);

  sprintf(strsql,"%3d %-*s %15.2f\n",knah,iceb_u_kolbait(30,nai),nai,sym);
  iceb_printw(iceb_u_toutf(strsql),buffer,view);

  fprintf(ff,"%3d %-*s %15.2f\n",knah,iceb_u_kolbait(30,nai),nai,sym);

  bb+=sym;
 }

if(fo == 1)
 {
  sprintf(strsql,"----------------------------------------------------\n\
%3s %*s %15.2f\n%s:\n"," ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),
  bb,gettext("Удержания"));

  iceb_printw(iceb_u_toutf(strsql),buffer,view);

  fprintf(ff,"----------------------------------------------------\n\
%3s %*s %15.2f\n%s:\n"," ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),
  bb,gettext("Удержания"));
  bb=0.;
  for(i=0 ; i<kou;i++)
   {
    sym=MSKU.ravno(i);
    if(sym == 0.)
      continue;
    knah=MKU.ravno(i);
    memset(nai,'\0',sizeof(nai));
    sprintf(strsql,"select naik from Uder where kod=%d",knah);
       
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      strncpy(nai,row1[0],sizeof(nai)-1);

    sprintf(strsql,"%3d %-*s %15.2f\n",knah,iceb_u_kolbait(30,nai),nai,sym);
    iceb_printw(iceb_u_toutf(strsql),buffer,view);

    fprintf(ff,"%3d %-*s %15.2f\n",knah,iceb_u_kolbait(30,nai),nai,sym);
    bb+=sym;
   }

  sprintf(strsql,"----------------------------------------------------\n\
%3s %*s %15.2f\n"," ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),bb);
  iceb_printw(iceb_u_toutf(strsql),buffer,view);
 }

/*Распечатываем суммы по табелям*/

sprintf(strsql,"%s: %d\n",gettext("Количество видов табеля"),koltb);
iceb_printw(iceb_u_toutf(strsql),buffer,view);

fprintf(ff,"----------------------------------------------------\n\
%3s %*s %15.2f\n%s:\n"," ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),bb,
gettext("Табель"));

fprintf(ff,"\
-----------------------------------------------------------------\n");
fprintf(ff,gettext("Код|           Наименование табеля          |Дни | Часы   |К.дни|\n"));
fprintf(ff,"\
-----------------------------------------------------------------\n");

for(i=0 ; i<koltb;i++)
 {
  dnei=DNI.ravno(i);
  kdnei=KDNI.ravno(i);
  has=HASI.ravno(i);
  if(dnei == 0 && kdnei == 0. && has == 0)
   continue;  

  knah=MKVT.ravno(i);
  memset(nai,'\0',sizeof(nai));
  sprintf(strsql,"select naik from Tabel where kod=%d",knah);
  nai[0]='\0';    
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(nai,row1[0],sizeof(nai)-1);

  fprintf(ff,"%3d %-*s %4.f %8.8g %4.f\n",knah,iceb_u_kolbait(40,nai),nai,dnei,has,kdnei);

  idn+=(int)dnei;
  ikdn+=(int)kdnei;
  ihas+=has;
 }
fprintf(ff,"\
-----------------------------------------------------------------\n\
%3s %*s %4d %8.8g %4d\n"," ",iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),idn,ihas,ikdn);




m=mn;
for(g=gn; g <=gk ; g++)
for(;  ; m++)
 {
  if(g == gk && m > mk)
   {
    break;
   }
  if(m == 13)
   {
    m=1;
    break;
   }
  fprintf(ff,"----------------- %d.%d%s -----------------\n",
  m,g,gettext("г."));
  vztbw(rek->tabnom.ravno_atoi(),m,g,&kon,&ihas,&kou,1,ff,view,buffer,wpredok);
 }




iceb_podpis(ff,wpredok);
iceb_podpis(ff1,wpredok);
iceb_podpis(ff2,wpredok);
iceb_podpis(ff3,wpredok);
iceb_podpis(ff_podoh,wpredok);
iceb_podpis(ff_sub,wpredok);

fclose(ff);
fclose(ff1);
fclose(ff2);
fclose(ff3);
fclose(ff_podoh);
fclose(ff_sub);

//Распечатка начислений/удержаний по счетам

sprintf(imafshet,"sp5_%d.lst",getpid());
if((ff = fopen(imafshet,"w")) == NULL)
 {
  iceb_er_op_fil(imafshet,"",errno,wpredok);
  return;
 }

fprintf(ff,"%d %s\n\
%s %d.%d%s %s %d.%d%s\n",rek->tabnom.ravno_atoi(),fam,
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."));

fprintf(ff,"\n%s:\n",gettext("Начисления"));
i=1;
bb=0.;
//while(NASH.pol_masiv_char(i++,bros) == 0)
for(i=0; i < NASH.kolih() ; i++)
 {
//  printf("%s\n",NASH.ravno(i));
  iceb_u_pole(NASH.ravno(i),str,1,',');
  iceb_u_pole(NASH.ravno(i),shet,2,',');
  sprintf(strsql,"select naik from Nash where kod=%s",str);
  memset(nai,'\0',sizeof(nai));
     
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(nai,row1[0],sizeof(nai)-1);
  fprintf(ff,"%2s %-*s %*s %8.2f\n",str,iceb_u_kolbait(30,nai),nai,iceb_u_kolbait(5,shet),shet,NASHD.ravno(i));
  bb+=NASHD.ravno(i);
 }

fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n%s:\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),
bb,gettext("Удержания"));
i=1;
bb=0.;

//while(UDER.pol_masiv_char(i++,bros) == 0)
for(i=0; i < UDER.kolih(); i++)
 {
  iceb_u_pole(UDER.ravno(i),str,1,',');
  iceb_u_pole(UDER.ravno(i),shet,2,',');
  sprintf(strsql,"select naik from Uder where kod=%s",str);
  memset(nai,'\0',sizeof(nai));
     
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(nai,row1[0],sizeof(nai)-1);

  fprintf(ff,"%2s %-*s %*s %8.2f\n",str,iceb_u_kolbait(30,nai),nai,iceb_u_kolbait(5,shet),shet,UDERD.ravno(i));
  bb+=UDERD.ravno(i);
 }
fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),bb);

//Распечатка бюджет/не бюджет

if(shetb != NULL)
 {
  fprintf(ff,"\n%s:\n",gettext("Начисления"));
  fprintf(ff,"%s.\n",gettext("Бюджет"));
  i=1;
  bb=0.;
  for(i=0; i < NASH.kolih(); i++)
   {
    iceb_u_pole(NASH.ravno(i),str,1,',');
    iceb_u_pole(NASH.ravno(i),shet,2,',');

    if(iceb_u_proverka(shetb,shet,0,1) != 0)
     continue;

    sprintf(strsql,"select naik from Nash where kod=%s",str);
    memset(nai,'\0',sizeof(nai));
       
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      strncpy(nai,row1[0],sizeof(nai)-1);

    fprintf(ff,"%2s %-*s %*s %8.2f\n",str,iceb_u_kolbait(30,nai),nai,iceb_u_kolbait(5,shet),shet,NASHD.ravno(i));
    bb+=NASHD.ravno(i);
   }


  fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n\n%s.\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),
  bb,
  gettext("Хозрасчёт"));


  i=1;
  bb=0.;
//  while(NASH.pol_masiv_char(i++,bros) == 0)
  for(i=0; i < NASH.kolih(); i++)
   {
    iceb_u_pole(NASH.ravno(i),str,1,',');
    iceb_u_pole(NASH.ravno(i),shet,2,',');

    if(iceb_u_proverka(shetb,shet,0,1) == 0)
     continue;
//    if(pole1(shetb,shet,',',0,&i1) == 0 || iceb_u_SRAV(shetb,shet,0) == 0)
//       continue;


    sprintf(strsql,"select naik from Nash where kod=%s",str);
    memset(nai,'\0',sizeof(nai));
       
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      strncpy(nai,row1[0],sizeof(nai)-1);

    fprintf(ff,"%2s %-*s %*s %8.2f\n",str,iceb_u_kolbait(30,nai),nai,iceb_u_kolbait(5,shet),shet,NASHD.ravno(i));
    bb+=NASHD.ravno(i);
   }


  fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),bb);

 }

if(shetbu != NULL)
 {
  fprintf(ff,"\n%s:\n",gettext("Удержания"));
  fprintf(ff,"%s.\n",gettext("Бюджет"));
  i=1;
  bb=0.;
  for(i=0; i < UDER.kolih(); i++)
   {
  
    iceb_u_pole(UDER.ravno(i),str,1,',');
    iceb_u_pole(UDER.ravno(i),shet,2,',');
  
    if(iceb_u_proverka(shetbu,shet,0,1) != 0)
     continue;
    sprintf(strsql,"select naik from Uder where kod=%s",str);
    memset(nai,'\0',sizeof(nai));
       
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      strncpy(nai,row1[0],sizeof(nai)-1);
    fprintf(ff,"%2s %-*s %*s %8.2f\n",str,iceb_u_kolbait(30,nai),nai,iceb_u_kolbait(5,shet),shet,UDERD.ravno(i));
    bb+=UDERD.ravno(i);
   }


  fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n\n%s.\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),
  bb,
  gettext("Хозрасчёт"));


  i=1;
  bb=0.;
  for(i=0; i < UDER.kolih(); i++)
   {
    iceb_u_pole(UDER.ravno(i),str,1,',');
    iceb_u_pole(UDER.ravno(i),shet,2,',');

    if(iceb_u_proverka(shetbu,shet,0,1) == 0)
     continue;


    sprintf(strsql,"select naik from Uder where kod=%s",str);
    memset(nai,'\0',sizeof(nai));
       
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      strncpy(nai,row1[0],sizeof(nai)-1);

    fprintf(ff,"%2s %-*s %*s %8.2f\n",str,iceb_u_kolbait(30,nai),nai,iceb_u_kolbait(5,shet),shet,UDERD.ravno(i));
    bb+=UDERD.ravno(i);
   }


  fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),bb);

 }

iceb_podpis(ff,wpredok);

fclose(ff);

rek->imaf.plus(imaf);
rek->imaf.plus(imaf1);
rek->imaf.plus(imaf2);
rek->imaf.plus(imafpen);
rek->imaf.plus(imafshet);
rek->imaf.plus(imaf_podoh);
rek->imaf.plus(imaf_sub);

rek->naimf.plus(gettext("Развёрнутый отчёт о начислениях/удержаниях"));
rek->naimf.plus(gettext("Справка с удержаниями в соц.фонды"));
rek->naimf.plus(gettext("Справка с начислениями и удержаниями"));
rek->naimf.plus(gettext("Справка с начислениями и удержаниями в пенсионный фонд"));
rek->naimf.plus(gettext("Свёрнутый отчёт о доходах и удержаниях"));
rek->naimf.plus(gettext("Справка с подоходным налогом"));
rek->naimf.plus(gettext("Справка для оформления социальной помощи (жилищной субсидии)"));

for(int nomer=0; nomer < rek->imaf.kolih(); nomer++)
 iceb_ustpeh(rek->imaf.ravno(nomer),3,wpredok);
}
/***********************/
/*Реквизиты организации*/
/***********************/
void  rekvizkon(SQL_str row,FILE *ff)
{
int otstup=30;
int hag=35;
fprintf(ff,"%*s %s\n",otstup,"",row[1]);
int hag1=hag-iceb_u_strlen(gettext("Адрес"))-1;

//fprintf(ff,"%*s %s %-*.*s\n",otstup,"",gettext("Адрес"),hag1,hag1,row[3]);
fprintf(ff,"%*s %s %-*.*s\n",
otstup,"",gettext("Адрес"),
iceb_u_kolbait(hag1,row[3]),
iceb_u_kolbait(hag1,row[3]),
row[3]);

for(int ii=hag1; ii < iceb_u_strlen(row[3]); ii+=hag)
 fprintf(ff,"%*s %-*.*s\n",
 otstup,"",
 iceb_u_kolbait(hag,iceb_u_adrsimv(ii,row[3])),
 iceb_u_kolbait(hag,iceb_u_adrsimv(ii,row[3])),
 iceb_u_adrsimv(ii,row[3]));
// fprintf(ff,"%*s %-*.*s\n",otstup,"",hag,hag,&row[3][ii]);

fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Код ЕГРПОУ"),row[5]);
fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Р/С"),row[7]);
fprintf(ff,"%*s %s %s\n",otstup,"",gettext("МФО"),row[6]);

char bros[512];

sprintf(bros,"%s %s %s %s",gettext("в"),row[2],gettext("г."),row[4]);
for(int ii=0; ii < iceb_u_strlen(bros); ii+=hag)
  fprintf(ff,"%*s %-*.*s\n",
  otstup," ",
  iceb_u_kolbait(hag,iceb_u_adrsimv(ii,bros)),
  iceb_u_kolbait(hag,iceb_u_adrsimv(ii,bros)),
  iceb_u_adrsimv(ii,bros));
//  fprintf(ff,"%*s %-*.*s\n",otstup," ",hag,hag,&bros[ii]);
  
fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Налоговый номер"),row[8]);
fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Номер свидетельства плательщика НДС"),row[9]);
fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Телефон"),row[10]);

}

/***********************************/
/*Шапка*/
/************************************/
void zarsprprw_sap(short mn,short gn,
short mk,short gk,
const char *harac,
const char *fam,
const char *inn,
const char *dolg,
const char *podr,
short sovm,
FILE *ff1)
{
char strsql[512];
if(iceb_u_strstrm(harac,gettext("Студент")) == 0)
 {

  fprintf(ff1,"\n\
	      %s\n\
%s %d.%d%s %s %d.%d%s\n\
%s %s %s",
  gettext("С п р а в к а"),
  gettext("о заработной плате за период с"),
  mn,gn,
  gettext("г."),
  gettext("по"),
  mk,gk,
  gettext("г."),
  gettext("Работник"),
  fam,
  gettext("работает"));

  if(sovm == 0)
   {
    if(harac[0] == '\0')
      fprintf(ff1," %s.\n",gettext("постоянно"));
    else
      fprintf(ff1," %s.\n",harac);
   }
  else
   fprintf(ff1," %s.\n",gettext("по совместительству"));

  fprintf(ff1,"%s %s\n",gettext("Идентификационный код"),inn);
  fprintf(ff1,"%s: %s\n",gettext("Должность"),dolg);
 }
else
 {
  SQL_str row;
  class SQLCURSOR cur;
  char	naipodr[512];
  memset(naipodr,'\0',sizeof(naipodr));
  
  sprintf(strsql,"select naik from Podr where kod=%d",atoi(podr));
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    strncpy(naipodr,row[0],sizeof(naipodr)-1);
    
  fprintf(ff1,"\n\
	      %s\n\
%s %d.%d%s %s %d.%d%s\n",
gettext("С п р а в к а"),
gettext("о начислении стипендии за период с"),
mn,gn,
gettext("г."),
gettext("до"),
mk,gk,
gettext("г."));
  fprintf(ff1,"%s. %s %s\n",
  gettext("Учится на дневном отделении"),
  gettext("Группа"),naipodr);
  
  fprintf(ff1,"%s %s\n",gettext("Студент"),fam);

  fprintf(ff1,"%s %s\n",gettext("Идентификационный код"),inn);

 }



}
/*******************************************/
/*Заголовок справки для получения субсидии*/
/********************************************/
int zarsprpr_sub_start(short god,const char *kedrpou,const char *fio,const char *inn,const char *dolg,FILE *ff_ras,
GtkWidget *wpredok)
{
char stroka[1024];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zarsns_start.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }


int nomer_str=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch(nomer_str)
   {
    case 7:
     iceb_u_bstab(stroka,organ,20,72,1);
     break;
    case 10:
     iceb_u_bstab(stroka,kedrpou,31,72,1);
     break;
    case 15:
     iceb_u_bstab(stroka,fio,11,72,1);
     break;
    case 17:
     iceb_u_bstab(stroka,organ,1,72,1);
     break;
    case 19:
     iceb_u_bstab(stroka,dolg,14,72,1);
     break;
    case 20:
     iceb_u_bstab(stroka,inn,23,72,1);
     break;
    case 23:
     iceb_u_bstab(stroka,god,1,5,1);
     break;
   }
  fprintf(ff_ras,"%s",stroka);
 }

return(0);
}
/*************************/
/*концовка справки на субсидию*/
/******************************/
int zarsprpr_sub_end(short mn,short gn,short mk,short gk,double suma,FILE *ff_ras,GtkWidget *wpredok)
{
class iceb_fioruk_rk rukov;
class iceb_fioruk_rk glavbuh;

char strsql[512];
char stroka[1024];
short dk=0;
iceb_u_dpm(&dk,&mk,&gk,5);

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
const char *imaf_alx={"zarsns_end.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

iceb_fioruk(1,&rukov,wpredok);
iceb_fioruk(2,&glavbuh,wpredok);

int nomer_str=0;
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
     sprintf(strsql,"01.%02d.%04d %s %s %02d.%02d.%04d %s",
     mn,gn,
     gettext("г."),
     gettext("по"),
     dk,mk,gk,
     gettext("г."));
     
     iceb_u_bstab(stroka,strsql,33,63,1);
     break;

    case 2:
     memset(strsql,'\0',sizeof(strsql));
     iceb_u_preobr(suma,strsql,0);
     iceb_u_bstab(stroka,strsql,20,72,1);
     
     break;

    case 9:
     iceb_u_bstab(stroka,rukov.dolg.ravno(),0,34,1);
     sprintf(strsql,"%s %s",rukov.famil.ravno(),rukov.inic);
     iceb_u_bstab(stroka,strsql,52,72,1);
     break;
    case 14:
     sprintf(strsql,"%s %s",glavbuh.famil.ravno(),glavbuh.inic);
     iceb_u_bstab(stroka,strsql,52,72,1);
     break;
   }
  fprintf(ff_ras,"%s",stroka);
 }
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
fprintf(ff_ras,"%02d.%02d.%d %s N______\n",dt,mt,gt,gettext("г."));

return(0);

}
