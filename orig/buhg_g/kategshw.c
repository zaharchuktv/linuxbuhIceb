/* $Id: kategshw.c,v 1.10 2011-02-21 07:35:52 sasa Exp $ */
/*16.11.2006    23.11.1992      Белых А.И.      kategshw.c
Подпрограмма получения всех начислений/счет и 
удержаний/счет по категориям
*/
#include <stdlib.h>
#include        <errno.h>
#include        <math.h>
#include        <time.h>
#include        "buhg_g.h"
#include "zar_kateg.h"

void            sappksh(short,short,char[],short,short,short,FILE*,double*,short*,class iceb_u_spisok*,int,int);
void		rasmaskatsh(FILE*,double*,class iceb_u_spisok*,short,short,short,double*,short*,short*,int);
void kategshh(short mn,short gn,short dkm,short mk,short gk,class zar_kateg_rek *data,int metkaot,FILE *kaw);
int             bih(int,int,class iceb_u_spisok*,int);
void schsh(short mn,short gn,short mk,short gk,class iceb_u_spisok *na,short kon,class iceb_u_spisok *ud,short kou,short *ka,short kka,short *koll,FILE *kaw,double *nao,double *udo,class zar_kateg_rek *data,FILE *ffhoz,FILE *ffbu,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *bar,GtkWidget *wpredok);

extern char     *organ;
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern SQL_baza bd;

int kategshw(class zar_kateg_rek *data,
FILE *kaw,FILE *ffhoz,FILE *ffbu,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *bar,
GtkWidget *wpredok)
{
short           kon;  /*Количество статей начисления/счет*/
short           kou;  /*Количество статей удержания/счет*/
short           kka;  /*Количество категорий*/
int             ik;
SQL_str         row;
char		strsql[512];
short		dkm;
long		dlmas;
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

/*Создаем массив категорий*/
//printw(gettext("Создаем массив категорий.\n"));
//refresh();

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

ik=0;
while(cur.read_cursor(&row) != 0)
  ka[ik++]=atoi(row[0]);

//printw(gettext("Создаем массив начислений/счет.\n"));
//refresh();

/*Создаем массив начислений/счетов*/
sprintf(strsql,"select distinct knah,shet from Zarp where \
datz >= '%04d-%d-1' and datz <= '%04d-%d-31' and prn='1' and suma <> 0. \
order by knah asc",gn,mn,gk,mk);

if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
class iceb_u_spisok nash; //Список начисление/счёт

if(kon == 0)
 {
  iceb_menu_soob(gettext("Не введены начисления !"),wpredok);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
/*
  printw("n-%s %s\n",row[0],row[1]);
  refresh();
*/
  sprintf(strsql,"%s|%s",row[0],row[1]);
  nash.plus(strsql);
 }
 

//printw(gettext("Создаем массив удержаний/счет.\n"));
//refresh();

/*Создаем массив удержаний/счетов*/
sprintf(strsql,"select distinct knah,shet from Zarp where \
datz >= '%04d-%d-1' and datz <= '%04d-%d-31' and prn='2' and suma <> 0. \
order by knah asc",gn,mn,gk,mk);

if((kou=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

class iceb_u_spisok udsh; //Список удержание/счёт
dlmas=0;

if(kou == 0)
 {
  iceb_menu_soob(gettext("Не введены удержания !"),wpredok);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[0],row[1]);
  udsh.plus(strsql);  
 }



double  nao[kka*kon];
memset(nao,'\0',sizeof(nao));

double udo[kka*kou];
memset(udo,'\0',sizeof(udo));

short koll[kka];
memset(koll,'\0',sizeof(koll));



iceb_u_dpm(&dkm,&mk,&gk,5);

kategshh(mn,gn,dkm,mk,gk,data,0,kaw);

if(ffhoz != NULL)
 {
  kategshh(mn,gn,dkm,mk,gk,data,1,ffhoz);
 }
if(ffbu != NULL)
 {
  kategshh(mn,gn,dkm,mk,gk,data,2,ffbu);
 }

schsh(mn,gn,mk,gk,&nash,kon,&udsh,kou,ka,kka,koll,kaw,nao,udo,data,ffhoz,ffbu,view,buffer,bar,wpredok);


iceb_podpis(kaw,wpredok);

if(ffbu != NULL)
 {
  iceb_podpis(ffbu,wpredok);
 }
if(ffhoz != NULL)
 {
  iceb_podpis(ffhoz,wpredok);
 }
return(0);
}


/********/
/*Поиск */
/********/
void schsh(short mn,short gn, //Дата начала
short mk,short gk, //Дата конца
class iceb_u_spisok *na, //Массив кодов начислений
short kon, //Количество статей начисления
class iceb_u_spisok *ud, //Массив кодов удержаний
short kou, //Количество статей удержания
short *ka, //Массив категорий
short kka, //Количество категорий
short *koll,
FILE *kaw,double *nao,double *udo,
class zar_kateg_rek *data,
FILE *ffhoz,FILE *ffbu,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *bar,
GtkWidget *wpredok)
{
int             i,i1;
int             skn=0;
int             mnc;  /*Метка начaла цикла*/
int             katt; /*Код категории*/
int             mtn;  /*Количество человек*/
long            ta;   /*Табельный номер*/
double          sn,su,snb,sub,snnb,sunb;
double          sum,sumb,sumnb;
double          dolgi,dolgib,dolginb;
short           m,g;
double          itsal,itsalb,itsalnb;  /*Итоговое сальдо*/
long		kolstr,kolstr2;
SQL_str         row,row1;
char		strsql[512];
long		tbn;
int		kpd,kkat,knah;
short		prn;
double		sym;
double		saldb;

/*
printw("schsh: %d.%d %d.%d-------\n",mn,gn,mk,gk);
OSTANOV();
*/

for(i=0;i<kka*kou;i++)
 udo[i]=0.;
for(i=0;i<kka*kon;i++)
 nao[i]=0.;
for(i=0;i<kka;i++)
   koll[i]=0;

ta=0;
mtn=0;
itsal=dolgi=sum=0.;
itsalb=dolgib=sumb=0.;
itsalnb=dolginb=sumnb=0.;
snb=sub=0.;
snnb=sunb=0.;
float kolstr1=0.;
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
  sprintf(strsql,"%s %d.%d%s\n",gettext("Дата"),m,g,gettext("г."));
  iceb_printw(iceb_u_toutf(strsql),buffer,view);
//  printw("%s\n",strsql);
  class SQLCURSOR cur;
  sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d",g,m);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }
  if(kolstr == 0)
   {
    continue;
   }

  mnc=katt=0;
  kolstr1=0;
  while(cur.read_cursor(&row) != 0)
   {
    iceb_pbar(bar,kolstr,++kolstr1);    

    if(iceb_u_proverka(data->podr.ravno(),row[1],0,0) != 0)
      continue;
    if(iceb_u_proverka(data->kod_kat.ravno(),row[2],0,0) != 0)
      continue;
    
    tbn=atol(row[0]);
    kpd=atoi(row[1]);
    kkat=atoi(row[2]);

    for(i=0 ; i < kka ; i++)
     if(ka[i] == kkat)
       {
	skn=i;
	break;
       }

    /*Если нет категории в списке читаем дальше*/
    if(i == kka)
     {
//    printw("kkat=%d Нет в списке!\n",kkat);
//    OSTANOV();

      continue;
     }

    /*Читаем сальдо*/
    sum=sumb=sumnb=0.;
    sum=zarsaldw(1,m,g,tbn,&saldb,wpredok);
    sumb=saldb;
    sumnb=sum-saldb;

    itsal+=sum;
    itsalb+=sumb;
    itsalnb+=sumnb;

    sprintf(strsql,"select prn,knah,suma,shet from Zarp where datz >= '%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%ld and suma <> 0.",
    g,m,g,m,tbn);
//  printw("%s\n",strsql);
    class SQLCURSOR cur1;
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
      continue;
     }
    if(kolstr2 == 0)
      continue;

      
    while(cur1.read_cursor(&row1) != 0)
     {
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
      if(shetb != NULL && shetbu != NULL)
       {
        if(row1[0][0] == '1')
         {
          if(iceb_u_pole1(shetb,row1[3],',',0,&i1) == 0 || iceb_u_SRAV(shetb,row1[3],0) == 0)
           {
            sumb+=sym;
            snb+=sym;
           }
          else
           {
            sumnb+=sym;
            snnb+=sym;
           }

         }
        if(row1[0][0] == '2')
         {
          if(iceb_u_pole1(shetbu,row1[3],',',0,&i1) == 0 || iceb_u_SRAV(shetbu,row1[3],0) == 0)
           {
            sumb+=sym;
            sub+=sym;
           }
          else
           {
            sumnb+=sym;
            sunb+=sym;
           }
         }
       }

      if(prn == 1)
       {
        sprintf(strsql,"%s|%s",row1[1],row1[3]);

        if((i=na->find(strsql)) < 0)
         {
          sprintf(strsql,"Не найдено %s в массиве начислений/счетов !",strsql);
          iceb_menu_soob(strsql,wpredok);
          continue;
         }       
	nao[i*kka+skn]+=sym;
       }
      if(prn == 2)
       {
        sprintf(strsql,"%s|%s",row1[1],row1[3]);

        if((i=ud->find(strsql)) < 0)
         {
          sprintf(strsql,"Не найдено %s в массиве удержаний/счетов !",strsql);
          iceb_menu_soob(strsql,wpredok);
          continue;
         }       
	udo[i*kka+skn]+=sym;
       }

     }

     mtn++;
     for(i=0; i<kka ; i++)
       if(ka[i] == kkat)
	 koll[i]++;

    if(sum < 0.)
      dolgi+=sum;
    if(sumb < 0.)
      dolgib+=sumb;
    if(sumnb < 0.)
      dolginb+=sumnb;
    sum = 0.;

   }
  
 }

if(sum < 0.)
   dolgi+=sum;

sn=su=0.;

if(kon > 0)
 {
  /*Распечатываем начисления*/
  sprintf(strsql,"%s\n-------------------------\n",
  gettext("Распечатываем начисления"));

  iceb_printw(iceb_u_toutf(strsql),buffer,view);
  /*Распечатываем начисления*/
  rasmaskatsh(kaw,nao,na,kka,kon,1,&sn,ka,koll,0);
 }

if(kou > 0)
 {
  sprintf(strsql,"\n%s\n-------------------------\n",
  gettext("Распечатываем удержания"));
  iceb_printw(iceb_u_toutf(strsql),buffer,view);
  /*Распечатываем удержания*/
  rasmaskatsh(kaw,udo,ud,kka,kou,2,&su,ka,koll,0);
 }

sprintf(strsql,"\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%s - %d\n",
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsal,
iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),sn,
iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),su,
iceb_u_kolbait(20,gettext("Итого долгов")),gettext("Итого долгов"),dolgi,
iceb_u_kolbait(20,gettext("К выдаче")),gettext("К выдаче"),sn+su-dolgi+itsal,
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsal+sn+su,
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
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsal+sn+su);

if(shetbu != NULL && shetb != NULL)
 {


  sprintf(strsql,"%s.\n------------------------------------------------\n",
  gettext("Бюджет"));
  iceb_printw(iceb_u_toutf(strsql),buffer,view);


  sprintf(strsql,"\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n",
  iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsalb,
  iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),snb,
  iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),sub,
  iceb_u_kolbait(20,gettext("Итого долгов")),gettext("Итого долгов"),dolgib,
  iceb_u_kolbait(20,gettext("К выдаче")),gettext("К выдаче"),snb+sub-dolgib+itsalb,
  iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsalb+snb+sub);

  iceb_printw(iceb_u_toutf(strsql),buffer,view);

  if(kon > 0)
   {
//    printw("%s\n-------------------------\n",
//    gettext("Распечатываем начисления"));
//    refresh();
    /*Распечатываем начисления*/
    rasmaskatsh(ffbu,nao,na,kka,kon,1,&sn,ka,koll,2);
   }

  if(kou > 0)
   {
//    printw("\n%s\n-------------------------\n",
//    gettext("Распечатываем удержания"));
//    refresh();
    /*Распечатываем удержания*/
    rasmaskatsh(ffbu,udo,ud,kka,kou,2,&su,ka,koll,2);
   }

  fprintf(ffbu,"%s.\n------------------------------------------------\n",
  gettext("Бюджет"));


  fprintf(ffbu,"\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n",
  iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsalb,
  iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),snb,
  iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),sub,
  iceb_u_kolbait(20,gettext("Итого долгов")),gettext("Итого долгов"),dolgib,
  iceb_u_kolbait(20,gettext("К выдаче")),gettext("К выдаче"),snb+sub-dolgib+itsalb,
  iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsalb+snb+sub);

  sprintf(strsql,"%s.\n------------------------------------------------\n",
  gettext("Хозрасчет"));
  iceb_printw(iceb_u_toutf(strsql),buffer,view);
  

  sprintf(strsql,"\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n",
  iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsalnb,
  iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),snnb,
  iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),sunb,
  iceb_u_kolbait(20,gettext("Итого долгов")),gettext("Итого долгов"),dolginb,
  iceb_u_kolbait(20,gettext("К выдаче")),gettext("К выдаче"),snnb+sunb-dolginb+itsalnb,
  iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsalnb+snnb+sunb);

  iceb_printw(iceb_u_toutf(strsql),buffer,view);

  if(kon > 0)
   {
    /*Распечатываем начисления*/
    rasmaskatsh(ffhoz,nao,na,kka,kon,1,&sn,ka,koll,1);
   }

  if(kou > 0)
   {
    /*Распечатываем удержания*/
    rasmaskatsh(ffhoz,udo,ud,kka,kou,2,&su,ka,koll,1);
   }

  fprintf(ffhoz,"%s.\n------------------------------------------------\n",
  gettext("Хозрасчет"));


  fprintf(ffhoz,"\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n",
  iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsalnb,
  iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),snnb,
  iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),sunb,
  iceb_u_kolbait(20,gettext("Итого долгов")),gettext("Итого долгов"),dolginb,
  iceb_u_kolbait(20,gettext("К выдаче")),gettext("К выдаче"),snnb+sunb-dolginb+itsalnb,
  iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsalnb+snnb+sunb);
  
 }
}

/***************/
/*Шапка таблицы*/
/***************/
void            sappksh(short kk, //1-начисления  2-удержания*/
short kka,  //Количество категорий
char st[],
short pn,  //Позиция начала
short pk,  //Позиция конца
short konu,  //Количество начислений или удержаний
FILE *kaw,double *nao,short *ka,
class iceb_u_spisok *nan,
int metka,int metkash)
{
int             i,i1,i2;
char            nai[512];
double          br2;
SQL_str         row;
SQLCURSOR curr;
char		strsql[512];
int		nomstr=0;
/*Формируем строку подчеркивания*/
/*Полка над наименованием*/
strcpy(st,"-----------------------------------------");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  nomstr=0;
  for(i1=i; i1< kka*konu ; i1+=kka)
   if(bih(metka,metkash,nan,nomstr++) == 0)
    br2+=nao[i1];

  if(br2 != 0. )
    strcat(st,"-------------");
  else
   i2++;
 }
/*Полка над ИТОГО*/
strcat(st,"-------------");

fprintf(kaw,"\n%s\n",st);

if(kk == 1)
 fprintf(kaw,"Код|%-*s|%-*.*s",
 iceb_u_kolbait(30,gettext("Начисления")),gettext("Начисления"),
 iceb_u_kolbait(5,gettext("Счет")),iceb_u_kolbait(5,gettext("Счет")),gettext("Счет"));

if(kk == 2)
 fprintf(kaw,"Код|%-*s|%-*.*s",
 iceb_u_kolbait(30,gettext("Удержания")),gettext("Удержания"),
 iceb_u_kolbait(5,gettext("Счет")),iceb_u_kolbait(5,gettext("Счет")),gettext("Счет"));

i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  nomstr=0;
  for(i1=i; i1< kka*konu ; i1+=kka)
   if(bih(metka,metkash,nan,nomstr++) == 0)
    br2+=nao[i1];

  if(br2 == 0.)
   {
    i2++;
    continue;
   }
  memset(nai,'\0',sizeof(nai));
  sprintf(strsql,"select naik from Kateg where kod=%d",ka[i]);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(nai,row[0],sizeof(nai)-1);

  fprintf(kaw,"| %-*.*s",iceb_u_kolbait(11,nai),iceb_u_kolbait(11,nai),nai);
 }

fprintf(kaw,"| %-*.*s|\n",iceb_u_kolbait(11,gettext(" Итого")),iceb_u_kolbait(11,gettext(" Итого")),gettext(" Итого"));

fprintf(kaw,"   |%-30s|%5.5s"," "," ");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  nomstr=0;
  for(i1=i; i1< kka*konu ; i1+=kka)
   if(bih(metka,metkash,nan,nomstr++) == 0)
    br2+=nao[i1];

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

void		rasmaskatsh(FILE *kaw,
double *naon, //Массив параметров
class iceb_u_spisok *nan,    //Массив с перечнем параметров
short kkan,   //Количество категорий
short konn,   //Количество параметров
short metka,  //1-начисления 2-удержания
double *sn,   //Сумма по массиву
short *ka,short *koll,
int metkash) //0-все 1-хозрасчет 2-бюджет
{
int		ii,i1,i2,i,skn,korr;
short           kkol=15; /*Количество колонок в распечатке*/
double		br1,br2;
int		khel;
char		st[300];
SQL_str         row;
char		nai[512];
char		strsql[512];
char		kod[32];
char		shet[32];
int		nomstr=0;
SQLCURSOR curr;
korr=0;
*sn=0.;
for(ii=0 ; ii < kkan; ii += kkol)
 {
  /*printw("ii-%d kkan-%d korr-%d\n",ii,kkan,korr);*/
  /*Проверяем если все равно нулю то печатаем следующюю группу столбиков*/
  i2=0; br2=0;
  for(i=ii; i<ii+kkol+i2 && i < kkan;i++)
   {
    br1=0.;
    nomstr=0;   
    for(skn=i;skn< kkan*(konn-1)+ii+kkol && skn<kkan*konn; skn+=kkan)
      if( bih(metka,metkash,nan,nomstr++) == 0)
       br1+=naon[skn];

    if(br1 == 0.)
     i2++;
    br2+=br1;
   }
  if(br2 == 0.)
    continue;
  memset(st,'\0',sizeof(st));
  sappksh(metka,kkan,st,ii,(short)(ii+kkol),konn,kaw,naon,ka,nan,metka,metkash);
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

    iceb_u_polen(nan->ravno(i),kod,sizeof(kod),1,'|');
    iceb_u_polen(nan->ravno(i),shet,sizeof(shet),2,'|');
    
    if(metka == 1) //Начисления
     {
      if(metkash == 2) //Бюджет
       if(iceb_u_proverka(shetb,shet,0,1) != 0)
         continue;

      if(metkash == 1) //Хозрасчет
       if(iceb_u_proverka(shetb,shet,0,1) == 0)
         continue;
      sprintf(strsql,"select naik from Nash where kod=%s",kod);
     }            
    if(metka == 2)
     {
      if(metkash == 2) //Бюджет
       if(iceb_u_proverka(shetbu,shet,0,1) != 0)
         continue;

      if(metkash == 1) //Хозрасчет
       if(iceb_u_proverka(shetbu,shet,0,1) == 0)
         continue;
      sprintf(strsql,"select naik from Uder where kod=%s",kod);
     }
    memset(nai,'\0',sizeof(nai));
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     strncpy(nai,row[0],sizeof(nai)-1);

    fprintf(kaw,"%3s %-*.*s %-*.*s",
    kod,
    iceb_u_kolbait(30,nai),iceb_u_kolbait(30,nai),nai,
    iceb_u_kolbait(5,shet),iceb_u_kolbait(5,shet),shet);

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
      nomstr=0;
      for(i1=skn-(i*kkan); i1< kkan*konn ; i1+=kkan)
       if(bih(metka,metkash,nan,nomstr++) == 0)
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
   }
  fprintf(kaw,"%s\n",st);

  fprintf(kaw,"%-*s",iceb_u_kolbait(39,gettext("Итого")),gettext("Итого"));

  i2=0;
  for(i=ii; i<ii+kkol+i2 && i < kkan ;i++)
   {
    br1=0.;
    nomstr=0;
    for(skn=i;skn < kkan*(konn-1)+ii+kkol+i2 && skn< kkan*konn ; skn+=kkan)
      if(bih(metka,metkash,nan,nomstr++) == 0)
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
  fprintf(kaw,"%-*s",iceb_u_kolbait(39,gettext("Количество человек")),gettext("Количество человек"));
  /*Предыдущая информация по людям*/
  khel=0;

  for(i=0; i<ii && i<kkan;i++)
    khel+=koll[i];

  i2=0;
  for(i=ii; i<ii+kkol+i2 && i<kkan;i++)
   {
      /*Если колонка нулевая не печатать*/
      br2=0.;
      nomstr=0;
      for(i1=i; i1< kkan*konn ; i1+=kkan)
       if(bih(metka,metkash,nan,nomstr++) == 0)
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
/**********************************/
/*Шапка отчета                    */
/**********************************/

void kategshh(short mn,short gn,
short dkm,short mk,short gk,
class zar_kateg_rek *data,
int metkaot, //0-все 1-хозрасчет 2-бюджет
FILE *kaw)
{
int		klst=0;

iceb_u_zagolov(gettext("Расчёт начислений и удержаний по категориям"),1,mn,gn,dkm,mk,gk,organ,kaw);

if(metkaot == 1)
 fprintf(kaw,"%s\n",gettext("Хозрасчет"));
if(metkaot == 2)
 fprintf(kaw,"%s\n",gettext("Бюджет"));
 
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
}

/***************************************/
/*Рзаборка с бюджетом и хозрасчетом    */
/***************************************/
//Если вернули 0 - подходить 1-нет
int    bih(int metka,int metkash,
class iceb_u_spisok *nan,int i)    //Массив с перечнем параметров
{
char  shet[32];

iceb_u_polen(nan->ravno(i),shet,sizeof(shet),2,'|');

if(metka == 1) //Начисления
 {
  if(metkash == 2) //Бюджет
   if(iceb_u_proverka(shetb,shet,0,1) != 0)
     return(1);

  if(metkash == 1) //Хозрасчет
   if(iceb_u_proverka(shetb,shet,0,1) == 0)
     return(1);
 }            
if(metka == 2)
 {
  if(metkash == 2) //Бюджет
   if(iceb_u_proverka(shetbu,shet,0,1) != 0)
     return(1);

  if(metkash == 1) //Хозрасчет
   if(iceb_u_proverka(shetbu,shet,0,1) == 0)
     return(1);
 }

return(0);

}
