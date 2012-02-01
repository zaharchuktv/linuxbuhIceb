/* $Id: ostdokw.c,v 1.10 2011-02-21 07:35:55 sasa Exp $ */
/*16.07.2004    21.09.1994      Белых А.И.      ostdokw.c
Получение остатка по записям в документах привязанных
к карточкам
		       С начала года         Период
		       включая период
		       кол.        цена      кол.   цена
Остаток начальный   0  000         000       000    000
Приход              1  000         000       000    000
Расход              2  000         000       000    000
Остаток             3  000         000       000    000

*/
#include        <stdlib.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

int        ostdokw_ras(short dn,short mn,short gn,short dk,short mk,short gk,const char *skk,const char *nkk,class ostatok *data);

extern double   okrg1; /*Округление*/
extern short	startgod; /*Стартовый год просмотров*/
extern SQL_baza	bd;

/**************************************/

int        ostdokw(short dn,short mn,short gn,short dk,short mk,
short gk,int skk,int nkk,class ostatok *data)
 {
  char sklad[40];
  char nomkar[40];
  sprintf(sklad,"%d",skk);
  sprintf(nomkar,"%d",nkk);
  return(ostdokw_ras(dn,mn,gn,dk,mk,gk,sklad,nomkar,data));
 
 }
 
/**************************************/

int        ostdokw(short dn,short mn,short gn,short dk,short mk,
short gk,const char *skk,const char *nkk,class ostatok *data)
 {
  return(ostdokw_ras(dn,mn,gn,dk,mk,gk,skk,nkk,data));
 }

/**************************************/

int        ostdokw_ras(short dn,short mn,short gn,short dk,short mk,
short gk,const char *skk,const char *nkk,class ostatok *data)
{
extern double	okrcn;
double		cena;
int             i;
double          prf;
double          cnn;
short           dp,mp,gp;
short           d,m,g;
double          bb;
double          kol,koll;
short           tz; /*Тип записи*/
SQL_str         row;
char		strsql[512];
int		kolstr=0;
double		kolih=0.;
char		nomd[20];
short		godn;
SQLCURSOR cur;

/*
printf("\n%d.%d.%d %d.%d.%d %s %s\n",dn,mn,gn,dk,mk,gk,skk,nkk);
*/

data->clear();

godn=startgod;
if(godn == 0)
 godn=gn;
if(gn < startgod)
  godn=gn; 
 
/*Читаем реквизиты карточки материалла*/
sprintf(strsql,"select cena from Kart where sklad=%s and nomk=%s",skk,nkk);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  printf("ostdok-Не найдена карточка материалла %s на складе %s !\n",nkk,skk);
  return(1);
 }
cena=atof(row[0]);
cena=iceb_u_okrug(cena,okrcn);



dp=dn;
mp=mn;
gp=gn;

iceb_u_dpm(&dp,&mp,&gp,2);


cnn=prf=0.;

sprintf(strsql,"select datd,kolih,nomd,tipz from Dokummat1 where \
sklad=%s and nomkar=%s and datd >= '%d-01-01' and \
datd <= '%d-%02d-%02d' order by datd asc",
skk,nkk,godn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);

//printw("%s\nkolstr=%ld\n",strsql,kolstr);
//OSTANOV();

if(kolstr == 0)
  return(0);

kol=koll=0.;
while(cur.read_cursor(&row) != 0)
 {
/*
  if(skk == 11 && nkk == 90)
    printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  refresh();
*/
  iceb_u_rsdat(&d,&m,&g,row[0],2);    
  kolih=atof(row[1]);
  strcpy(nomd,row[2]);
  
//  if(SRAV1(gk,mk,dk,g,m,d) > 0)
  if(iceb_u_sravmydat(d,m,g,dk,mk,gk) > 0)
   {
/*    printw("%d.%d.%d %d.%d.%d\n",gk,mk,dk,g,m,d); */
     break;
   }

  kol=kolih;

  
  tz=atoi(row[3]);
 
  if(tz == 1 && iceb_u_SRAV(nomd,"000",0) == 0 && godn != g)
     continue;
  if(tz == 1 && iceb_u_SRAV(nomd,"000",0) == 0 && godn == g)
   {
    data->ostg[0]+=kol;
    bb=kol*cena;
    data->ostgc[0]=iceb_u_okrug(bb,okrg1);
    continue;
   }

  data->ostg[tz]+=kol;
  bb=kol*cena;
  data->ostgc[tz]+=iceb_u_okrug(bb,okrg1);

  /*Период*/
//  if(SRAV1(gn,mn,dn,g,m,d) >= 0)
//  if(SRAV1(gk,mk,dk,g,m,d) <= 0)
  if(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0)
   {
//    printw("Період\n");
    data->ostm[tz]+=kol;
    bb=kol*cena;
    data->ostmc[tz]+=iceb_u_okrug(bb,okrg1);
   }

  /*До периода*/
//  if(SRAV1(gp,mp,dp,g,m,d) <= 0)
  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0)
   {
//    printw("До періода\n");
    if(tz == 2)
       kol*=(-1);
    prf+=kol;
    bb=kol*cena;
    cnn+=iceb_u_okrug(bb,okrg1);
   }

 }
data->ostg[3]=data->ostg[0]+data->ostg[1]-data->ostg[2];
data->ostm[0]=data->ostg[0]+prf;
data->ostm[3]=data->ostm[0]+data->ostm[1]-data->ostm[2];

data->ostgc[3]=data->ostgc[0]+data->ostgc[1]-data->ostgc[2];
data->ostmc[0]=data->ostgc[0]+cnn;
data->ostmc[3]=data->ostmc[0]+data->ostmc[1]-data->ostmc[2];
/*Убираем погрешность вычисления*/
for(i=0; i<4 ; i++)
 {
  if(data->ostg[i] < 0.00000000001 && data->ostg[i] > -0.00000000001)
     data->ostg[i]=0.;
  if(data->ostm[i] < 0.00000000001 && data->ostm[i] > -0.00000000001)
    data->ostm[i]=0.;
 }

return(0);
}
