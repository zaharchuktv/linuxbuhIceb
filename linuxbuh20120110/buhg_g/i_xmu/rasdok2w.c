/* $Id: rasdok2w.c,v 1.15 2011-02-21 07:35:56 sasa Exp $ */
/*14.11.2009	11.03.1999	Белых А.И.	rasdok2w.c
Распечатка накладной в две колонки
*/
#include        <stdlib.h>
#include        <time.h>
//#include        <pwd.h>
#include        <errno.h>
#include        "../headers/buhg_g.h"
#include        <unistd.h>


void		vztr1(short,short,short,int,const char*,double*,FILE*,short,GtkWidget*);
void		dvekol(const char*,GtkWidget*);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern char	*organ;
extern short	vtara; /*Код группы возвратная тара*/
extern SQL_baza  bd;
extern char	*printer;

void rasdok2w(short dd,short md,short gd,int skl,const char *nomdok,
short tipnn, //Тип накладной
GtkWidget *wpredok)
{
struct  tm      *bf;
time_t          tmm;
short		mnt; /*Метка наличия тары*/
short           mvnpp=0; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
char		nmo1[512];
char		imaf[24],imaf1[24];
FILE		*ff,*f1;
double		bb,bb1,bb2,bb3,bb4;
double		kol,krat,cena;
char		str[1024];
char		bros[512];
char		naim[512];
double		itogo,itogo1;
double		mest;
double		sumkor;
double		sumt;
short		kgrm;
short		kolz;
SQL_str         row;
char		strsql[512];
short		tipz;
char		kpos[56];
char		kop[56];
char		naimpr[512];
short		vtar;
SQLCURSOR cur;

/*
printw("Печать двойного документа.\n");
OSTANOV();
*/
/*Читаем шапку накладной*/

sprintf(strsql,"select tip,kontr,kodop,pn from Dokummat \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
gd,md,dd,skl,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"rasdok2w-%s",gettext("Не найден документ !")),
  repl.plus(strsql);
  repl.ps_plus(strsql);
  
  sprintf(strsql,"%s %d.%d.%d %s %s %s %d",
  gettext("Дата"),dd,md,gd,
  gettext("Документ"),nomdok,
  gettext("Слад"),skl);
  
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  
  return;
 }
float pnds=atof(row[3]);
tipz=atoi(row[0]);
strcpy(kpos,row[1]);
strcpy(kop,row[2]);

/*Читаем наименование операции*/
if(tipz == 1)
 strcpy(bros,"Prihod");
if(tipz == 2)
 strcpy(bros,"Rashod");
memset(naimpr,'\0',sizeof(naimpr));

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros,kop);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код операции"));
  repl.plus(" ");
  repl.plus(kop);
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);  
 }
else
 {
  mvnpp=atoi(row[1]);
  strncpy(naimpr,row[0],sizeof(naimpr)-1);
 }

memset(nmo1,'\0',sizeof(nmo1));


/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/


if(iceb_u_atof(kpos) != 0. || kpos[0] != '0')
 {

  sprintf(bros,"select * from Kontragent where kodkon='%s'",kpos);
  if(sql_readkey(&bd,bros,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",
    gettext("Не найден код контрагента"),kpos);
    iceb_menu_soob(strsql,wpredok);
    return;
   }
  else
   {
    strncpy(nmo1,row[1],sizeof(nmo1)-1);
   }
 }


sortnakw(dd,md,gd,skl,nomdok,wpredok);

sprintf(imaf,"sn%d.lst",getpid());
if((f1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
  return;
 }

sprintf(imaf1,"sn%d.tmp",getpid());
if((ff = fopen(imaf1,"r")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,NULL);
  return;
 }


fprintf(f1,"%s\n\
                  %s %s\n\
                     %s %d.%d.%d%s\n",
organ,gettext("С ч е т  фактура"),nomdok,
gettext("от"),
dd,md,gd,
gettext("г."));


fprintf(f1,"%.73s\n",nmo1);


/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gd,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
//  fprintf(f1,gettext("Доверенность N"));
  iceb_u_pole(row[0],bros,1,'#');
  fprintf(f1," %s",bros);
//  iceb_u_pole(row[0],bros,2,'#');
//  fprintf(f1," %s %s",gettext("от"),bros);
  fprintf(f1,"\n");
 }

/*Читаем льготы по ндс*/
int lnds=0;
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gd,nomdok,skl,11);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 lnds=atoi(row[0]);

if(lnds == 0)
 {
  fprintf(f1,"\
-------------------------------------------------------------------------\n");

  fprintf(f1,gettext("\
N |    Наименование продукции    |Коли- |Ц е н а|Ц е н а| С у м м а|Кол.|\n"));
  fprintf(f1,gettext("\
  |         (товару)             |чество| з НДС |без НДС| без НДС  |мест|\n"));

  fprintf(f1,"\
-------------------------------------------------------------------------\n");
 }
else
 {
  fprintf(f1,"\
-------------------------------------------------------------------------\n");

  fprintf(f1,gettext("\
N |        Наименование продукции        |Коли- |Ц е н а| С у м м а|Кол.|\n"));
  fprintf(f1,gettext("\
  |             (товару)                 |чество|без НДС| без НДС  |мест|\n"));

  fprintf(f1,"\
-------------------------------------------------------------------------\n");

 }
itogo=itogo1=0.;
kolz=0;
kol=0.;
mnt=0;
/*printw("\nvtara=%d\n",vtara);*/
while(fgets(str,sizeof(str),ff) != NULL)
 {
/*  printw("%s",str);*/

  iceb_u_pole(str,bros,1,'|');
  kgrm=atoi(bros);

  iceb_u_pole(str,naim,2,'|');
  iceb_u_pole(str,bros,5,'|');
  cena=atof(bros);
  iceb_u_pole(str,bros,6,'|');
  krat=atof(bros);

  iceb_u_pole(str,bros,8,'|');
  vtar=atoi(bros);  

  iceb_u_pole(str,bros,10,'|');
  kol=atof(bros);
/*
  printw("nds=%f kgrm=%d\n",nds,kgrm);
  OSTANOV();
*/
  if(vtar == 1)
   {
    mnt++;
    continue;
   } 

  if(vtara != 0 && kgrm == vtara)
   {
    mnt++;
    continue;
   } 
  mest=0;
  if(krat > 0.001)
   {
    mest=kol/krat;
    mest=iceb_u_okrug(mest,0.1);
   }

/*  printw("%d kol=%f krat=%f %f\n",kolz,kol,krat,mest);*/

  bb=cena*kol;
  bb=iceb_u_okrug(bb,okrg1);
  bb1=cena+(cena*pnds/100.);
  bb1=iceb_u_okrug(bb1,okrg1);

  itogo+=bb;

  kolz++;
  if(lnds == 0)
   fprintf(f1,"%2d %-*.*s %6.6g %7.2f %7s",
   kolz,
   iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,
   kol,bb1,iceb_prcn(cena));
  else
   fprintf(f1,"%2d %-*.*s %6.6g %7s",
   kolz,
   iceb_u_kolbait(38,naim),iceb_u_kolbait(38,naim),naim,
   kol,iceb_prcn(cena));

  fprintf(f1," %10s",iceb_prcn(bb));
  if(mest != 0)
     fprintf(f1," %4.4g\n",mest);
  else
     fprintf(f1,"\n");


 }

fclose(ff);
unlink(imaf1);

fprintf(f1,"\
-------------------------------------------------------------------------\n");
fprintf(f1,"\
%*s: %10s\n",iceb_u_kolbait(55,gettext("Итого")),gettext("Итого"),iceb_prcn(itogo));

if(mvnpp == 0) /*Внешнее перемещение*/
 {

  /*Читаем сумму корректировки*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gd,nomdok,skl,13);
  sumkor=0.;

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    sumkor=atof(row[0]);
    sumkor=iceb_u_okrug(sumkor,0.01);
   }


  bb=bb1=bb2=bb3=bb4=0.;

  if(sumkor != 0)
   {
    if(sumkor > 0.)
      bb=sumkor*100./itogo;
    if(sumkor < 0.)
      bb=100*sumkor/(itogo-sumkor*-1);
    bb=iceb_u_okrug(bb,0.01);
    if(sumkor < 0)
      sprintf(bros,"%s %.6g%% :",gettext("Снижка"),bb*(-1));
    if(sumkor > 0)
      sprintf(bros,"%s %.6g%% :",gettext("Надбавка"),bb);

    fprintf(f1,"%*s %10.2f/%.f\n",iceb_u_kolbait(56,bros),bros,sumkor,sumkor+sumkor*pnds/100.);

    if(sumkor < 0)
      sprintf(bros,gettext("Итого со снижкой :"));
    if(sumkor > 0)
      sprintf(bros,gettext("Итого с надбавкой :"));

    fprintf(f1,"%*s %10.2f\n",iceb_u_kolbait(56,bros),bros,sumkor+itogo);

   }

  bb=0.;

  if(tipnn == 0 )
   {
    sprintf(bros,"%s %.2f%% :",gettext("НДС"),pnds);
    bb=(bb1+bb3+itogo+sumkor)*pnds/100.;
    bb=iceb_u_okrug(bb,0.01);
    fprintf(f1,"%*s %10.2f\n",iceb_u_kolbait(56,bros),bros,bb);
   }

  sumt=0.;
  if(mnt != 0)
     vztr1(dd,md,gd,skl,nomdok,&sumt,f1,vtara,wpredok);
        
  itogo1=(itogo+sumkor)+bb1+bb3+bb+sumt;
  strcpy(bros,gettext("Итого к оплате:"));
  if(tipz == 1)
    strcpy(bros,gettext("Итого:"));
  fprintf(f1,"%*s %10.2f\n",iceb_u_kolbait(56,bros),bros,itogo1);
 }
else
 { 
  sumt=0.;
  if(mnt != 0)
     vztr1(dd,md,gd,skl,nomdok,&sumt,f1,vtara,wpredok);
  itogo1+=sumt;
 }
memset(str,'\0',sizeof(str));
iceb_u_preobr(itogo1,str,0);

fprintf(f1,"%s:%s\n",gettext("Сумма прописью"),str);
fprintf(f1,"\n\%s_____________ %s_______________\n",
gettext("Отпустил"),
gettext("Принял"));

time(&tmm);
bf=localtime(&tmm);


fprintf(f1,"%s: %s\n",gettext("Распечатал"),iceb_getfioop(wpredok));
fprintf(f1,"\
%s %d.%d.%d%s %s - %d:%d\n",gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

iceb_podpis(f1,wpredok);

fclose(f1);

dvekol(imaf,wpredok);
iceb_pehf(imaf,1,wpredok);

}

/*****************/
/*Распечатка тары*/
/*****************/

void vztr1(short dd,short md,short gd,int skl,const char *nomdok,
double *sumt,FILE *f1,short vt,GtkWidget *wpredok)
{
double		itogo,bb;
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
double		kolih,cena;
int		nk,kgrm,vtr;
char		naim[512];
int		i;

sprintf(strsql,"select * from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 { 
  return;
 } 

itogo=0.;
i=0;
while(cur.read_cursor(&row) != 0)
 {

  /*Читаем наименование материалла*/
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",
  row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
    {
     printf("vztr-Не нашли наименование материалла по коду %s\n",row[4]);
    }
  else
    strncpy(naim,row1[0],sizeof(naim)-1);

  kgrm=atoi(row1[1]);
  vtr=atoi(row[8]);

  if(vtr == 0. && (vtara != 0 && kgrm != vtara))
    continue;
  if(vtr == 0 && vtara == 0 )
    continue;

  nk=atoi(row[3]);
  kolih=atof(row[5]);
  cena=atof(row[6]);
  cena=iceb_u_okrug(cena,okrcn);
  
  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);
  i++;
  if(i == 1)
   fprintf(f1,gettext("Оборотная тара:\n"));

  fprintf(f1,"%2d %-*.*s %6.6g %7s %7s",
  i,
  iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,
  kolih," ",iceb_prcn(cena));
  fprintf(f1," %10s\n",iceb_prcn(bb));

  itogo+=bb;

 }


*sumt=itogo;

}

/******************************/
/*Преобразование в две колонки*/
/******************************/
void dvekol(const char *imaf,GtkWidget *wpredok)
{
FILE		*ff,*f1;
char		imaf1[20];
char		strm[100];

if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

sprintf(imaf1,"sn%d.tmp",getpid());
if((f1 = fopen(imaf1,"w")) == NULL)
 {
  fclose(ff);
  iceb_er_op_fil(imaf1,"",errno,NULL);
  return;
 }



while(iceb_u_fgets(strm,sizeof(strm),ff) != 0)
 {
  fprintf(f1,"%-*s %s\n",iceb_u_kolbait(80,strm),strm,strm);
 }


fclose(f1);
fclose(ff);
iceb_cp(imaf1,imaf,0,wpredok);
unlink(imaf1);
iceb_ustpeh(imaf,1,wpredok);
}
