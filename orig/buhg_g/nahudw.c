/* $Id: nahudw.c,v 1.12 2011-02-21 07:35:55 sasa Exp $ */
/*14.11.2009    07.12.1992      Белых А.И.      nahudw.c
Подпрограмма поиска всех начислений и удержаний на конкретного
человека по конкретному номеру
*/
#include <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        "buhg_g.h"

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short	*obud; /*Обязательные удержания*/
extern double   okrg; /*Округление*/
extern char	shrpz[16]; /*Счет расчетов по зарплате*/
extern char	pktn[16]; /*Приставка к табельному номеру*/
extern SQL_baza bd;

void nahudw(short g,short m, //Дата
int tn, //Табельный номер
double *nah,
double *nahprov, //Сумма начисления на которую надо сделать проводки
double *uder, //Общая сумма удержаний
double *uderprov, //Cумма удержаний для котрой нужно сделать проводки
double *nahb,
double *uderb,
double *uderbprov,
short ras,  //0-не распечатывать 1-распечатывать на печать 2-на экран*/
double sl,  //Сальдо
short mp,  //0-На системный принтер 1-на приэкранную печать
const char *fio,
GtkWidget *wpredok)
{
char            bros[512];
struct  tm      *bf=NULL;
time_t          tmm;
FILE            *ff=NULL;
double          dolg=0.,bbr=0.;
short           uhsl;
double          ius;
char		imaf[20];
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
short		dz,mz,gz;
short		mz1,gz1;
short		prn,knah;
double		sym;
double		sumabu; /*Сумма бюджетных удержаний*/
int		i1;
short		metkaprov=0;
SQLCURSOR curr;
*nah=*nahprov=*uder=*uderprov=*nahb=*uderb=*uderbprov=0.;
uhsl=0;

if(ras != 0)
 {
  time(&tmm);
  bf=localtime(&tmm);
  bros[0]='\0';
  uhsl=0;
  if(iceb_poldan("Учет сальдо в распечатке",bros,"zarnast.alx",wpredok) == 0)
     if(iceb_u_SRAV(bros,"Вык",1) == 0)
       uhsl=1;
 }
sprintf(strsql,"select * from Zarp where datz >='%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%d order by prn,knah asc",
g,m,g,m,tn);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_TABLEACCESS_DENIED_ERROR) //Запрет команды select  
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;

ius=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&dz,&mz,&gz,row[0],2);
  prn=atoi(row[2]);
  knah=atoi(row[3]);
  sym=atof(row[4]);
  gz1=atoi(row[6]);
  mz1=atoi(row[7]);
  if(prn == 1)
   {
    *nah+=sym;
    if(shetb != NULL)
     if(iceb_u_pole1(shetb,row[5],',',0,&i1) == 0 || iceb_u_SRAV(shetb,row[5],0) == 0)
       *nahb+=sym;

    //Проверяем нужно ли делать проводки для этого удержания
    metkaprov=0;
    sprintf(strsql,"select prov from Nash where kod=%d",knah);
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      if(row1[0][0] == '1')
       {
        metkaprov=1;
       }

   if(metkaprov == 0)
    *nahprov+=sym;
   
   }
  if(prn == 2)
   {
    if(uhsl == 1 && ras != 0)
     {
      if(mz1 == m && gz1 == g)
       {
	ius+=sym;
//        printw("sym=%.2f m=%d mz1=%d g=%d gz1=%d\n",
//        sym,m,mz1,g,gz1);
       }
      else
       {
//        printw("*sym=%.2f\n",sym);
        if(obud != NULL)
         for(i1=1; i1<= obud[0]; i1++)
          if(obud[i1] == knah)
           {
    	    ius+=sym;
            break;
           }
       }

     }
    //Проверяем нужно ли делать проводки для этого удержания
    metkaprov=0;
    sprintf(strsql,"select prov from Uder where kod=%d",knah);
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      if(row1[0][0] == '1')
       {
        metkaprov=1;
       }

   *uder+=sym;
   if(metkaprov == 0)
    *uderprov+=sym;

   if(shetbu != NULL)
    if(iceb_u_pole1(shetbu,row[5],',',0,&i1) == 0 || iceb_u_SRAV(shetbu,row[5],0) == 0)
     {
      *uderb+=sym;
      if(metkaprov == 0)
        *uderbprov+=sym;
     }
   }
/************
  if(ras == 2)
   {
    if(prn == 1)
      sprintf(strsql,"select naik from Nash where kod=%s",row[3]);
    if(prn == 2)
      sprintf(strsql,"select naik from Uder where kod=%s",row[3]);
       
    memset(nai,'\0',sizeof(nai));
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
     {
      if(prn == 1)
       printf("%s %s !\n",gettext("Не найден код начисления"),row[3]);
      if(prn == 2)
       printf("%s %s\n",gettext("Не найден код удержания"),row[3]);
     }
    else
      strncpy(nai,row1[0],sizeof(nai)-1);
    printw("%2d %02d %-30.30s %2s.%s %15s %s\n",
    knah,dz,nai,row[7],row[6],iceb_u_prnbr(sym),row[12]);
   }
******************/
 }

*nah=iceb_u_okrug(*nah,okrg);
*nahprov=iceb_u_okrug(*nahprov,okrg);
*uder=iceb_u_okrug(*uder,okrg);
*uderprov=iceb_u_okrug(*uderprov,okrg);

*nahb=iceb_u_okrug(*nahb,okrg);
*uderb=iceb_u_okrug(*uderb,okrg);
*uderbprov=iceb_u_okrug(*uderbprov,okrg);

if(ras != 0)
 {

  if(uhsl == 0)
     dolg=bbr=*nah+*uder+sl;

  if(uhsl == 1)
   {
    bbr=prbperw(m,g,bf->tm_mon+1,bf->tm_year+1900,tn,&sumabu,0,wpredok);
    dolg=bbr=*nah+ius+bbr;
//    printw("%.2f+%.2f+%.2f\n",*nah,ius,bbr);
//    OSTANOV();
   }

  if(dolg < 0.)
    bbr=0.;
  if(ras == 1)
   {
    time(&tmm);
    bf=localtime(&tmm);
    sprintf(imaf,"ko%d.lst",tn);
    char osnovan[50];
    sprintf(osnovan,"%s %d.%d%s",
    gettext("Заработная плата"),m,g,
    gettext("г."));
    //Узнаем номер документа
    char nomd[20];
    char vidan[300];
    memset(nomd,'\0',sizeof(nomd));
    memset(vidan,'\0',sizeof(vidan));
    sprintf(strsql,"select nomp,vidan,datvd from Kartb where tabn=%d",
    tn);
    if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
     {
      short d=0,m,g;
      iceb_u_rsdat(&d,&m,&g,row[2],2);
      strncpy(nomd,row[0],sizeof(nomd)-1);
      if(d == 0)
       sprintf(vidan,"%s %s %s",
       gettext("Паспорт"),nomd,row[1]);
      else
       sprintf(vidan,"%s %s %02d.%02d.%d%s %s",
       gettext("Паспорт"),nomd,
       d,m,g,
       gettext("г."),
       row[1]);
      
     }
    sprintf(bros,"%s%d",pktn,tn);    
    iceb_kasord2(imaf,"",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,shrpz,bros,bbr,fio,osnovan,"",vidan,"",ff,wpredok);
    
    class iceb_u_spisok imafr;
    class iceb_u_spisok naimfr;
    imafr.plus(bros);
    naimfr.plus(gettext("Распечатка кассового ордера"));    
    iceb_rabfil(&imafr,&naimfr,"",0,wpredok);
   }
/***********
  if(ras == 2)
   {
    bbr=(*nah)+(*uder);
    printw("\
------------------------------------------------------\n\
%39s %15s\n",gettext("Итого"),iceb_u_prnbr(bbr));
   }
******************/
 }

}
