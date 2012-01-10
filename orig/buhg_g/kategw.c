/* $Id: kategw.c,v 1.13 2011-02-21 07:35:52 sasa Exp $ */
/*08.09.2009    23.11.1992      Белых А.И.      kategw.c
Подпрограмма получения всех начислений и удержаний по категориям
Если вернули 0-расчёт сделан
             1-нет
*/
#include <stdlib.h>
#include        <errno.h>
#include        <math.h>
#include        <time.h>
#include        "buhg_g.h"
#include "zar_kateg.h"

void sch(short mn,short gn,short mk,short gk,short *na,short kon,short *ud,short kou,short *ka,short kka,short *kollnah,short *kollud,FILE *kaw,double *nao,double *udo,class zar_kateg_rek *data,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *bar,GtkWidget *wpredok);
void rasmaskat(FILE*,double*,short*,short,short,short,double*,short*,short*,GtkWidget*);

extern char     *organ;
extern SQL_baza bd;

int kategw(class zar_kateg_rek *data,FILE *kaw,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *bar,
GtkWidget *wpredok)
{
short           kon=0;  /*Количество статей начисления*/
short           kou=0;  /*Количество статей удержания*/
short           kka=0;  /*Количество категорий*/
int             in,iu,ik;
SQL_str         row;
char		strsql[512];
short		dkm=0;
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

/*Определяем количество начислений и удержаний*/

kka=kon=kou=0;
in=iu=ik=0;

sprintf(strsql,"select kod from Nash");
class SQLCURSOR cur;
if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kon == 0)
 {
  iceb_menu_soob(gettext("Не введены начисления !"),wpredok);
  return(1);
 }
short na[kon];
while(cur.read_cursor(&row) != 0)
  na[in++]=atoi(row[0]);

sprintf(strsql,"select kod from Uder");

if((kou=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kou == 0)
 {
  iceb_menu_soob(gettext("Не введены удержания !"),wpredok);
  return(1);
 }

short ud[kou];

while(cur.read_cursor(&row) != 0)
  ud[iu++]=atoi(row[0]);

sprintf(strsql,"select kod from Kateg");

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
while(cur.read_cursor(&row) != 0)
  ka[ik++]=atoi(row[0]);

double nao[kka*kon];
memset(&nao,'\0',sizeof(nao));

double udo[kka*kou];
memset(&udo,'\0',sizeof(udo));

short kollnah[kka];
short kollud[kka];
memset(&kollnah,'\0',sizeof(kollnah));
memset(&kollud,'\0',sizeof(kollud));


iceb_u_dpm(&dkm,&mk,&gk,5);


iceb_u_zagolov(gettext("Расчёт начислений и удержаний по категориям"),1,mn,gn,dkm,mk,gk,organ,kaw);

if(data->podr.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Код подразделения"),data->podr.ravno());
  iceb_printcod(kaw,"Podr","kod","naik",0,data->podr.ravno(),&klst);
 }
else
 fprintf(kaw,"%s\n",gettext("По всем подразделениям"));

if(data->kod_nah.ravno()[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Код начисления"),data->kod_nah.ravno());
if(data->kod_ud.ravno()[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Код удержания"),data->kod_ud.ravno());
if(data->kod_kat.ravno()[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Коды категории"),data->kod_kat.ravno());
if(data->tabnom.ravno()[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Табельный номер"),data->tabnom.ravno());
if(data->shet.ravno()[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Счет"),data->shet.ravno());

sch(mn,gn,mk,gk,na,kon,ud,kou,ka,kka,kollnah,kollud,kaw,nao,udo,data,view,buffer,bar,wpredok);


iceb_podpis(kaw,wpredok);

return(0);
}

/********/
/*Поиск */
/********/
void            sch(short mn,short gn,short mk,short gk,
short *na, //Массив кодов начислений
short kon, //Количество статей начисления
short *ud,  //Массив кодов удержаний
short kou,  //Количество статей удержания
short *ka,  //Коды категорий
short kka, //Количество категорий
short *kollnah, //Массив количества человек в каждой категории
short *kollud,
FILE *kaw,
double *nao, //Массив содержимого начислений по категориям
double *udo, //Массив содержимого удержаний по категориям
class zar_kateg_rek *data,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *bar,
GtkWidget *wpredok)
{
printf("sch-работаем %d.%d %d.%d\n",mn,gn,mk,gk);
int             i;
int             skn=0;
int             mnc=0;  /*Метка начaла цикла*/
int             katt; /*Код категории*/
int             kkk=0;
int             mtn;  /*Количество человек*/
long            ta;   /*Табельный номер*/
double          sn,su;
double          sum;
double          dolgi;
short           m,g;
double          itsal;  /*Итоговое сальдо*/
int		kolstr,kolstr2;
SQL_str         row,row1;
char		strsql[512];
int		tbn;
int		kpd,kkat,knah;
short		prn;
double		sym;
double		saldb;
short		metkas=0;
/*
printw("sch: %d.%d %d.%d-------\n",mn,gn,mk,gk);
OSTANOV();
*/

for(i=0;i<kka*kou;i++)
 udo[i]=0.;
for(i=0;i<kka*kon;i++)
 nao[i]=0.;
//for(i=0;i<kka;i++)
//   koll[i]=0;

ta=0;
mtn=0;
itsal=dolgi=sum=0.;

g=gn;
class SQLCURSOR cur;
float kolstr1=0.;
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

  sprintf(strsql,"%s %d.%d%s\n",gettext("Дата"),m,g,gettext("г."));
  iceb_printw(iceb_u_toutf(strsql),buffer,view);
  
  sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d",g,m);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }
  if(kolstr == 0)
    continue;
  mnc=katt=0;
  kolstr1=0;
  while(cur.read_cursor(&row) != 0)
   {
    iceb_pbar(bar,kolstr,++kolstr1);    

    if(iceb_u_proverka(data->tabnom.ravno(),row[0],0,0) != 0)
      continue;
    if(iceb_u_proverka(data->podr.ravno(),row[1],0,0) != 0)
      continue;
    if(iceb_u_proverka(data->kod_kat.ravno(),row[2],0,0) != 0)
      continue;
    if(iceb_u_proverka(data->shet.ravno(),row[3],0,0) != 0)
      continue;

    tbn=atol(row[0]);
    kpd=atoi(row[1]);
    kkat=atoi(row[2]);

    for(i=0 ; i < kka ; i++)
     if(ka[i] == kkat)
       {
	skn=i+1;
	break;
       }

    /*Если нет категории в списке читаем дальше*/
    if(i == kka)
     {
      if(kkat == 0)
       sprintf(strsql,"%s %d %s !",gettext("Табельный номер"),
       tbn,gettext("В карточку не введён код категории"));
      else
       sprintf(strsql,"%s %d !",gettext("Не найден код категории"),kkat);

      iceb_menu_soob(strsql,wpredok);
      continue;
     }

    /*Читаем сальдо*/
    sum=0.;
    if(m == mn && g == gn)
      sum=zarsaldw(1,m,g,tbn,&saldb,wpredok);
    itsal+=sum;

    sprintf(strsql,"select prn,knah,suma,shet from Zarp where \
datz >= '%d-%02d-01' and datz <= '%d-%02d-31' and tabn=%d and \
suma <> 0.",g,m,g,m,tbn);

//  printw("%s\n",strsql);
    class SQLCURSOR cur1;
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
      continue;
     }
    metkas=0;
    if(kolstr2 == 0)
     {
      if(sum < 0.)
       {
        dolgi+=sum;
        metkas=1;
       }
      continue;
     }

    short metkanah=0;
    short metkaud=0;      
    while(cur1.read_cursor(&row1) != 0)
     {
/*
      printw("Zarp-%s %s %s\n",row1[0],row1[1],row1[2]);
      refresh();
*/
      prn=atoi(row1[0]);

      if(prn == 1)
        if(iceb_u_proverka(data->kod_nah.ravno(),row1[1],0,0) != 0)
          continue;

      if(prn == 2)
        if(iceb_u_proverka(data->kod_ud.ravno(),row1[1],0,0) != 0)
          continue;

      if(iceb_u_proverka(data->shet.ravno(),row1[3],0,0) != 0)
         continue;

      knah=atoi(row1[1]);
      sym=atof(row1[2]);

      sum+=sym;

      if(prn == 1)
       {
        if(metkanah == 0)
         {
          for(i=0; i<kka ; i++)
           if(ka[i] == kkat)
            {
             kollnah[i]++;
             break;
            }
          metkanah++;
         }
        for(i=0 ; i < kon ; i++)
	  if(na[i] == knah)
	   {
	    kkk=i+1;
	    break;
	   }
	nao[(kkk-1)*kka+skn-1]+=sym;
       }

      if(prn == 2)
       {
        if(metkaud == 0)
         {
          for(i=0; i<kka ; i++)
           if(ka[i] == kkat)
            {
             kollud[i]++;
             break;
            }
          metkaud++;
         }

        for(i=0 ; i < kou ; i++)
	  if(ud[i] == knah)
	   {
	    kkk=i+1;
	    break;
	   }
	udo[(kkk-1)*kka+skn-1]+=sym;
       }

     }

    mtn++;
    if(sum < 0.)
      dolgi+=sum;
    sum = 0.;

   }
  
 }

if(sum < 0. && metkas == 0)
   dolgi+=sum;


/*Распечатываем начисления*/
sprintf(strsql,"%s\n-------------------------\n",
gettext("Распечатываем начисления"));

iceb_printw(iceb_u_toutf(strsql),buffer,view);

sn=su=0.;
rasmaskat(kaw,nao,na,kka,kon,1,&sn,ka,kollnah,wpredok);

sprintf(strsql,"\n%s\n-------------------------\n",
gettext("Распечатываем удержания"));
iceb_printw(iceb_u_toutf(strsql),buffer,view);

/*Распечатываем удержания*/
rasmaskat(kaw,udo,ud,kka,kou,2,&su,ka,kollud,wpredok);


sprintf(strsql,"\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%s: %d\n",
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsal,
iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),sn,
iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),su,
iceb_u_kolbait(20,gettext("Итого долгов")),gettext("Итого долгов"),dolgi,
iceb_u_kolbait(20,gettext("К выдаче")),gettext("К выдаче"),sn+su-dolgi+itsal,
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),sn+su+itsal,
gettext("Количество человек"),mtn);



iceb_printw(iceb_u_toutf(strsql),buffer,view);


fprintf(kaw,"\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n",
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsal,
iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),sn,
iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),su,
iceb_u_kolbait(20,gettext("Итого долгов")),gettext("Итого долгов"),dolgi,
iceb_u_kolbait(20,gettext("К выдаче")),gettext("К выдаче"),sn+su-dolgi+itsal,
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),sn+su+itsal);

}

/***************/
/*Шапка таблицы*/
/***************/
void            sappk(short kk, //1-начисления  2-удержания
short kka,  //Количество категорий
char st[],
short pn,  //Позиция начала
short pk,  //Позиция конца
short konu, //Количество начислений или удержаний
FILE *kaw,double *nao,short *ka)
{
int             i,i1,i2;
char            nai[512];
double          br2;
SQL_str         row;
SQLCURSOR curr;
char		strsql[512];

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

if(kk == 1)
 fprintf(kaw,"Код|%-*s",iceb_u_kolbait(30,gettext("Начисления")),gettext("Начисления"));
if(kk == 2)
 fprintf(kaw,"Код|%-*s",iceb_u_kolbait(30,gettext("Удержания")),gettext("Удержания"));
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

  sprintf(strsql,"select naik from Kateg where kod=%d",ka[i]);
  memset(nai,'\0',sizeof(nai));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(nai,row[0],sizeof(nai)-1);

  fprintf(kaw,"| %-*.*s",iceb_u_kolbait(11,nai),iceb_u_kolbait(11,nai),nai);
 }

fprintf(kaw,"| %-*.*s|\n",iceb_u_kolbait(11,gettext("Итого")),iceb_u_kolbait(11,gettext("Итого")),gettext("Итого"));

fprintf(kaw,"   |%-30s"," ");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
    br2+=nao[i1];
/*
    if(kk == 1)
     br2+=nao[i1];
    if(kk == 2)
     br2+=udo[i1];
*/
   }
  if(br2 != 0.)
    fprintf(kaw,"| %-11d",ka[i]);
  else
   i2++;
 }
fprintf(kaw,"| %-11.11s|\n"," ");

fprintf(kaw,"%s\n",st);

}

/******************************/
/*Программа распечатки массива*/
/******************************/

void		rasmaskat(FILE *kaw,
double *naon, //Массив параметров*/
short *nan,  //Массив с перечнем параметров
short kkan,  //Количество категорий
short konn,  //Количество параметров
short metka, //1-начисления 2-удержания
double *sn, //Сумма по массиву
short *ka,short *koll,
GtkWidget *wpredok)
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
*sn=0;
for(ii=0 ; ii < kkan; ii += kkol)
 {
  /*printw("ii-%d kkan-%d korr-%d\n",ii,kkan,korr);*/
  /*Проверяем если все равно нулю то печатаем следующюю группу столбиков*/
  i2=0; br2=0;
  for(i=ii; i<ii+kkol+i2 && i < kkan;i++)
   {
    br1=0.;
    for(skn=i;skn< kkan*(konn-1)+ii+kkol && skn<kkan*konn; skn+=kkan)
      br1+=naon[skn];
    if(br1 == 0.)
     i2++;
    br2+=br1;
   }
  if(br2 == 0.)
    continue;
  memset(st,'\0',sizeof(st));
  sappk(metka,kkan,st,ii,(short)(ii+kkol),konn,kaw,naon,ka);
  br1=0.;
  for(i=0 ; i<konn;i++)
   {
    /*Проверяем если по горизонтали сумма нулевая то строчку не печатаем*/
    br1=0.;
     for(skn=0;skn< kkan ; skn++)
      {
       br1+=naon[skn+i*kkan];
      }

    if(br1 == 0.)
      continue;

    memset(nai,'\0',sizeof(nai));
    if(metka == 1)
      sprintf(strsql,"select naik from Nash where kod=%d",nan[i]);
    if(metka == 2)
      sprintf(strsql,"select naik from Uder where kod=%d",nan[i]);

    if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
      strncpy(nai,row[0],sizeof(nai)-1);
  
    fprintf(kaw,"%3d %-*.*s",
    nan[i],
    iceb_u_kolbait(30,nai),iceb_u_kolbait(30,nai),nai);

    /*Предыдущая информация*/
    br1=0.;
    if(ii > 0 )
     {
      for(skn=i*kkan; skn<i*kkan+ii && skn < kkan*konn ;skn++)
       {
	 br1+=naon[skn];
       }
     }
    i2=0;
    for(skn=i*kkan+ii; skn<i*kkan+ii+kkol+i2 && skn < i*kkan+kkan ;skn++)
     {

      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=skn-(i*kkan); i1< kkan*konn ; i1+=kkan)
	 br2+=naon[i1];

      if(br2 != 0.)
       {
	fprintf(kaw," %12.2f",naon[skn]);
       }
      else
       {
	i2++;
       }
      br1+=naon[skn];
     }
    fprintf(kaw," %12.2f\n",br1);
    /*printw("%3d %-30s %.2f\n",zr2.tn,nai,br1);*/
   }
  fprintf(kaw,"%s\n",st);

  fprintf(kaw,"%-*s",iceb_u_kolbait(34,gettext(" И т о г о")),gettext(" И т о г о"));

  i2=0;
  for(i=ii; i<ii+kkol+i2 && i < kkan ;i++)
   {
    br1=0.;
    for(skn=i;skn < kkan*(konn-1)+ii+kkol+i2 && skn< kkan*konn ; skn+=kkan)
      br1+=naon[skn];

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

  for(i=0; i<ii && i<kkan;i++)
    khel+=koll[i];

  i2=0;
  for(i=ii; i<ii+kkol+i2 && i<kkan;i++)
   {
      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=i; i1< kkan*konn ; i1+=kkan)
	 br2+=naon[i1];

      if(br2 != 0.)
       {
	fprintf(kaw," %12d",koll[i]);
	khel+=koll[i];
       }
      else
	i2++;
   }
  fprintf(kaw," %12d\n\n",khel);
  if(i == kkan)
   break;
  ii+=i2;
 }

}
