/* $Id: otrshw.c,v 1.8 2011-02-21 07:35:55 sasa Exp $ */
/*21.11.2006    13.07.1996      Белых А.И.      otrshw.c
Подпрограмма получения всех отработанных часов и дней
по категориям
Если вернули 0- отчёт сделан
             1- нет
*/
#include <stdlib.h>
#include        <errno.h>
#include        <math.h>
#include        <time.h>
#include        "buhg_g.h"
#include "zar_otrsh.h"

void            sappk1(short,char[],short,short,short,double*,FILE*,short*);
void		rasmasov(FILE*,double*,short*,short,short,double*,short*,short*);
void sch1(short mn,short gn,short mk,short gk,short *na,short *ka,double *nao,double *udo,double *kal_dnei,short *koll,FILE *kaw,short kon,short kka,class zar_otrsh_rek *data,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);

extern char     *organ;
extern SQL_baza bd;

int otrshw(class zar_otrsh_rek *data,
FILE *kaw,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
short           kon;  /*Количество видов табелей*/
short           kka;  /*Количество категорий*/
int             in,ik;
SQL_str         row;
char		strsql[512];
int		klst=0;

short mn,gn;
short mk,gk;

iceb_u_rsdat1(&mn,&gn,data->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->datak.ravno());
if(mk == 0)
 {
  mk=mn; 
  gk=gn;
 }

sprintf(strsql,"%s %d.%d %s %d.%d\n",gettext("Период с"),mn,gn,
gettext("по"),mk,gk);

iceb_printw(iceb_u_toutf(strsql),buffer,view);
/*Определяем количество табелей*/

kka=kon=0;
in=ik=0;
sprintf(strsql,"select kod from Kateg");
class SQLCURSOR cur;
if((kka=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kka == 0)
 {
  iceb_menu_soob(gettext("Не введены категории !"),wpredok);
  return(1);
 }
short ka[kka];
memset(ka,'\0',sizeof(ka));


while(cur.read_cursor(&row) != 0)
  ka[ik++]=atoi(row[0]);

sprintf(strsql,"select kod from Tabel");

if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kon == 0)
 {
  iceb_menu_soob(gettext("Не введены виды табеля !"),wpredok);
  return(1);
 }

short na[kon];
memset(na,'\0',sizeof(na));


while(cur.read_cursor(&row) != 0)
  na[in++]=atoi(row[0]);

double nao[kka*kon];
memset(nao,'\0',sizeof(nao));

double udo[kka*kon];
memset(udo,'\0',sizeof(udo));

double kal_dnei[kka*kon];
memset(kal_dnei,'\0',sizeof(kal_dnei));

short koll[kka];
memset(koll,'\0',sizeof(koll));

short dkm=1;
iceb_u_dpm(&dkm,&mk,&gk,5);

iceb_u_zagolov(gettext("Расчёт отработанного времени по категориям"),1,mn,gn,dkm,mk,gk,organ,kaw);

if(data->podr.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Код подразделения"),data->podr.ravno());
  iceb_printcod(kaw,"Podr","kod","naik",0,data->podr.ravno(),&klst);
 }
else
 fprintf(kaw,"%s\n",gettext("По всем подразделениям"));

if(data->kod_kat.ravno()[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Коды категории"),data->kod_kat.ravno());
if(data->kod_tab.ravno()[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Код табеля"),data->kod_kat.ravno());
if(data->tabnom.ravno()[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Табельный номер"),data->tabnom.ravno());


sch1(mn,gn,mk,gk,na,ka,nao,udo,kal_dnei,koll,kaw,kon,kka,data,view,buffer,wpredok);

return(0);
}


/********/
/*Поиск */
/********/
void sch1(short mn,short gn,short mk,short gk,
short *na, //Массив кодов табелей
short *ka, //Массив категорий
double *nao, //Массив содержимого табелей часов
double *udo, //Массив содержимого табелей дней
double *kal_dnei, //Массив содержащий календарные дни
short *koll, //Массив количества человек в каждой категории
FILE *kaw,
short kon, //Количество видов табелей
short kka, //Количество категорий
class zar_otrsh_rek *data,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
int             i;
int             skn=0;
int             mnc;  /*Метка начaла цикла*/
int             katt; /*Код категории*/
int             kkk=0;
unsigned short  mtn;  /*Количество человек*/
long            ta;   /*Табельный номер*/
double          sn,su;
short           m,g;
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
long		tbn;
int		kkat,kpd;
short		vt;
float		dnei=0.,has=0.,kal_dn=0.;
/*
printw("sch1-00000000000\n");
refresh();
*/
for(i=0;i<kka*kon;i++)
 nao[i]=udo[i]=0.;
for(i=0;i<kka;i++)
   koll[i]=0;

ta=0;
mtn=0;


g=gn;

for(m=mn; ; m++)
 {
  if(m==13)
   {
    m=1;
    g++;
   }
  if(g>gk)
    break;
  if(g == gk && m > mk)
    break;

  sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d",
  g,m);
  SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }

  if(kolstr == 0)
   {
    continue;
   }

  mnc=katt=0;

  while(cur.read_cursor(&row) != 0)
   {
    if(iceb_u_proverka(data->tabnom.ravno(),row[0],0,0) != 0)
      continue;

    if(iceb_u_proverka(data->podr.ravno(),row[1],0,0) != 0)
      continue;

    if(iceb_u_proverka(data->kod_kat.ravno(),row[2],0,0) != 0)
      continue;


    tbn=atol(row[0]);
    kpd=atoi(row[1]);
    kkat=atoi(row[2]);
        

    sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and \
mes=%d and tabn=%ld",
    g,m,tbn);
    SQLCURSOR cur1;
    if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
      continue;
     }

    if(kolstr == 0)
     {
      continue;
     }

    while(cur1.read_cursor(&row1) != 0)
     {
      if(iceb_u_proverka(data->kod_tab.ravno(),row1[0],0,0) != 0)
        continue;

      vt=atoi(row1[0]);
      dnei=atof(row1[1]);
      has=atof(row1[2]);
      kal_dn=atof(row1[3]);
      
      for(i=0 ; i < kka ; i++)
        if(ka[i] == kkat)
         {
   	  skn=i;
  	  break;
         }
      /*Если нет категории в списке читаем дальше*/
      if(i == kka)
       continue;


      for(i=0 ; i < kon ; i++)
       if(na[i] == vt)
	{
	 kkk=i;
	 break;
	}

      nao[kkk*kka+skn]+=has;
      udo[kkk*kka+skn]+=dnei;
      kal_dnei[kkk*kka+skn]+=kal_dn;
     }

     mtn++;
     for(i=0; i<kka ; i++)
       if(ka[i] == kkat)
	 koll[i]++;

   }
 }

/*Распечатываем*/
//printw("%s\n-------------------------\n",
//gettext("Распечатываем отработанные часы"));
fprintf(kaw,"%s\n",gettext("Отработанные часы"));
sn=su=0.;
rasmasov(kaw,nao,na,kka,kon,&sn,ka,koll);

//printw("\n%s\n-------------------------\n",
//gettext("Распечатываем отработанные дни"));

fprintf(kaw,"%s\n",gettext("Отработанные дни"));

rasmasov(kaw,udo,na,kka,kon,&su,ka,koll);

fprintf(kaw,"%s\n",gettext("Календаные дни"));
double ikdn=0.;
rasmasov(kaw,kal_dnei,na,kka,kon,&ikdn,ka,koll);

sprintf(strsql,"\n\
%s:%.2f\n\
%s:%.2f\n\
%s:%.2f\n\
%s:%d\n",
gettext("Итого часов"),sn,
gettext("Итого дней"),su,
gettext("Итого календарных дней"),ikdn,
gettext("Количество человек"),mtn);

iceb_printw(iceb_u_toutf(strsql),buffer,view);

fprintf(kaw,"\n\
%s:%.2f\n\
%s:%.2f\n\
%s:%.2f\n\
%s:%d\n",
gettext("Итого часов"),sn,
gettext("Итого дней"),su,
gettext("Итого календарных дней"),ikdn,
gettext("Количество человек"),mtn);

}

/***************/
/*Шапка таблицы*/
/***************/
void            sappk1(short kka, //Количество категорий
char st[],
short pn, //Позиция начала
short pk, //Позиция конца
short konu,  //Количество начислений или удержаний
double *nao,
FILE *kaw,short *ka)
{
int             i,i1,i2;
char            nai[512];
double          br2;
SQL_str         row;
char		strsql[512];
SQLCURSOR curr;
/*Формируем строку подчеркивания*/
/*Полка над наименованием*/
strcpy(st,"-----------------------------------");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
    br2+=nao[i1];
   }
  if(br2 != 0. )
    strcat(st,"-------------");
  else
   i2++;
 }
/*Полка над ИТОГО*/
strcat(st,"-------------");

fprintf(kaw,"\n%s\n",st);

fprintf(kaw,"%*s|%-*s",
iceb_u_kolbait(3,gettext("Код")),gettext("Код"),
iceb_u_kolbait(30,gettext("Т а б е л я")),gettext("Т а б е л я"));

i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
    br2+=nao[i1];
   }
  if(br2 == 0.)
   {
    i2++;
    continue;
   }

  memset(nai,'\0',sizeof(nai));
  sprintf(strsql,"select naik from Kateg where kod=%d",ka[i]);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(nai,row[0],sizeof(nai)-1);

  fprintf(kaw,"|%-*.*s",iceb_u_kolbait(12,nai),iceb_u_kolbait(12,nai),nai);
 }
fprintf(kaw,"|%-*.*s|\n",iceb_u_kolbait(12,gettext("Итого")),iceb_u_kolbait(12,gettext("Итого")),gettext("Итого"));

fprintf(kaw,"   |%-30s"," ");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
    br2+=nao[i1];
   }
  if(br2 != 0.)
    fprintf(kaw,"|%-12d",ka[i]);
  else
   i2++;
 }
fprintf(kaw,"|%-12.12s|\n"," ");

fprintf(kaw,"%s\n",st);

}

/********************/
/*Распечатка массива*/
/*********************/
void rasmasov(FILE *kaw,
double *nao, //Массив параметров
short *na,  //Массив с перечнем параметров
short kka,  //Количество категорий
short kon, //Количество параметров
double *sn, //Сумма по массиву
short *ka,short *koll)
{
int		ii,i1,i2,i,skn,korr;
short           kkol; /*Количество колонок в распечатке*/
double		br1,br2;
int		khel;
char		st[300];
SQL_str         row;
char		nai[512];
char		strsql[512];
SQLCURSOR curr;
/*
При ужатом режиме 234 знака при 10 знаках на дюйм
                  275 знаков при 12 знаках на дюйм
(275-35)/13=18.46
*/
kkol=17;

korr=0;
*sn=0.;
for(ii=0 ; ii < kka; ii += kkol)
 {
  /*printw("ii-%d kka-%d korr-%d\n",ii,kka,korr);*/
  /*Проверяем если все равно нулю то печатаем следующюю группу столбиков*/
  i2=0; br2=0.;
  for(i=ii; i<ii+kkol+i2 && i < kka;i++)
   {
    br1=0.;
    for(skn=i;skn< kka*(kon-1)+ii+kkol && skn<kka*kon; skn+=kka)
      br1+=nao[skn];
    if(br1 == 0.)
     i2++;
    br2+=br1;
   }
  if(br2 == 0.)
    continue;
  memset(st,'\0',sizeof(st));
  sappk1(kka,st,ii,(short)(ii+kkol),kon,nao,kaw,ka);
  br1=0.;
  for(i=0 ; i<kon;i++)
   {
    /*Проверяем если по горизонтали сумма нулевая то строчку не печатаем*/
/*  printw("\ni-%d kka-%d\n",i,kka);*/
    br1=0.;
     for(skn=0;skn< kka ; skn++)
      {
/*     printw("skn-%d nao[%d]=%.2f\n",skn,skn+i*kka,nao[skn+i*kka]);*/
       br1+=nao[skn+i*kka];
      }
/*  printw("\n\n");
    OSTANOV();   */
    if(br1 == 0.)
      continue;

    memset(nai,'\0',sizeof(nai));
    sprintf(strsql,"select naik from Tabel where kod=%d",na[i]);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      strncpy(nai,row[0],sizeof(nai)-1);

    fprintf(kaw,"%3d %-*.*s",na[i],iceb_u_kolbait(30,nai),iceb_u_kolbait(30,nai),nai);

    /*Предыдущая информация*/
    br1=0.;
    if(ii > 0 )
     {
      for(skn=i*kka; skn<i*kka+ii && skn < kka*kon ;skn++)
       {
	/*printw(" skn=%d",skn);*/
	 br1+=nao[skn];
       }
      /*printw("br1=%.2f\n",br1);*/
     }
    i2=0;
    for(skn=i*kka+ii; skn<i*kka+ii+kkol+i2 && skn < i*kka+kka ;skn++)
     {

      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=skn-(i*kka); i1< kka*kon ; i1+=kka)
	 br2+=nao[i1];

      if(br2 != 0.)
       {
	/*printw("skn-%d (%d) br2=%.2f %d  \n",skn,i*kka+ii+kkk+i2,br2,kka*kon);*/
	fprintf(kaw," %12.2f",nao[skn]);
       }
      else
       {
	i2++;
       }
      br1+=nao[skn];
     }
    fprintf(kaw," %12.2f\n",br1);
    /*printw("%3d %-30s %.2f\n",zr2.tn,nai,br1);*/
   }
  fprintf(kaw,"%s\n",st);
  fprintf(kaw,"%-*s",iceb_u_kolbait(34,gettext("Итого")),gettext("Итого"));
  i2=0;
  for(i=ii; i<ii+kkol+i2 && i < kka ;i++)
   {

    br1=0.;
    for(skn=i;skn < kka*(kon-1)+ii+kkol+i2 && skn< kka*kon ; skn+=kka)
     {
      br1+=nao[skn];
     }
    if(br1 != 0.)
       fprintf(kaw," %12.2f",br1);
    else
      i2++;
    *sn=*sn+br1;
   }
  fprintf(kaw," %12.2f\n",*sn);

  fprintf(kaw,"%s\n",st);

  /*Распечатываем количество человек*/
  fprintf(kaw,"%-*s",iceb_u_kolbait(34,gettext("Количество человек")),gettext("Количество человек"));
  /*Предыдущая информация по людям*/
  khel=0;

  for(i=0; i<ii && i<kka;i++)
    khel+=koll[i];

  i2=0;
  for(i=ii; i<ii+kkol+i2 && i<kka;i++)
   {
      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=i; i1< kka*kon ; i1+=kka)
	 br2+=nao[i1];

      if(br2 != 0.)
       {
	fprintf(kaw," %12d",koll[i]);
	khel+=koll[i];
       }
      else
	i2++;
   }
  fprintf(kaw," %12d\n\n",khel);
  if(i == kka)
   break;
  ii+=i2;
 }
}
