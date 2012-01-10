/* $Id: ostkarw.c,v 1.15 2011-04-01 08:00:50 sasa Exp $ */
/*20.05.2004    21.04.1997      Белых А.И.      ostkarw.c
получение остатка материалла по карточкам
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
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
int           ostkarw1(short dn,short mn,short gn,short dk,short mk,short gk,const char *skl,const char *nk,class ostatok *data);

extern short	startgod; /*Стартовый год просмотров*/
extern SQL_baza	bd;

/*****************************/

int ostkarw(short dn,short mn,short gn,short dk,short mk,
short gk,int skl,int nk,class ostatok *data)
 {
  char sklad[32];
  sprintf(sklad,"%d",skl);
  char nomkart[32];
  sprintf(nomkart,"%d",nk);
  
  return(ostkarw1(dn,mn,gn,dk,mk,gk,sklad,nomkart,data));
 }

/*****************************/

int ostkarw(short dn,short mn,short gn,short dk,short mk,
short gk,const char *skl,const char *nk,class ostatok *data)
 {
  return(ostkarw1(dn,mn,gn,dk,mk,gk,skl,nk,data));
 }


/*****************************/

int ostkarw1(short dn,short mn,short gn,short dk,short mk,
short gk,const char *skl,const char *nk,class ostatok *data)
{
extern double   okrg1; /*Округление*/
extern double	okrcn;
short           dp,mp,gp;
short		d,m,g;
double          bb;
double          prf,prf1;
double          cnn;
short           i;
short           mkg;
char		nn[64];
short 		tp;
double		kolih,cena;
SQL_str         row;
char		strsql[512];
int		kolstr;
char		ei[64];
double		fas;
short		godn;
/**********
printf("ostkarw-%d.%d.%d %d.%d.%d %s %s\n",
dn,mn,gn,dk,mk,gk,skl,nk);
************/
SQLCURSOR cur;

data->clear();

godn=startgod;
if(godn == 0)
 godn=gn;
if(gn < startgod)
  godn=gn; 
/*Читаем реквизиты карточки материалла*/
sprintf(strsql,"select ei,cena,fas from Kart where sklad=%s and nomk=%s",
skl,nk);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"ostkarw-Не найдена карточка %s на складе %s !\n",nk,skl);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }
strncpy(ei,row[0],sizeof(ei)-1);
cena=atof(row[1]);
cena=iceb_u_okrug(cena,okrcn);

fas=atof(row[2]);

mkg=0;
if(iceb_u_SRAV(ei,"кг",0) != 0 && iceb_u_SRAV(ei,"Кг",0) != 0 &&
 iceb_u_SRAV(ei,"КГ",0) != 0)
   mkg=1;

if(mn == 0)
 mn=1;
if(mk == 0)
 mk=12;

dp=dn;
mp=mn;
gp=gn;

iceb_u_dpm(&dp,&mp,&gp,2);


sprintf(strsql,"select datdp,nomd,kolih,tipz from Zkart where sklad=%s \
and nomk=%s and datdp >= '%04d-01-01' and datdp <= '%04d-%02d-%02d' \
order by datdp asc",
skl,nk,godn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }
 
if(kolstr == 0)
 {
  return(0);
 } 

cnn=prf=prf1=0.;
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s %s\n",
  row[0],row[1],row[2],row[3]);
*/

  iceb_u_rsdat(&d,&m,&g,row[0],2);

  tp=atoi(row[3]);
  strcpy(nn,row[1]);
  kolih=atof(row[2]);
        
  //if(SRAV1(gk,mk,dk,g,m,d) > 0)
  if(iceb_u_sravmydat(d,m,g,dk,mk,gk) > 0)
     break;

  if(tp == 1 && iceb_u_SRAV(nn,"000",0) == 0 && g != godn)
    continue;
  if(tp == 1 && iceb_u_SRAV(nn,"000",0) == 0 && g == godn)
   {
    /*printw("Остаток\n");*/
    data->ostg[0]+=kolih;
    if(mkg == 1 )
      data->kgg[0]+=kolih*fas;
    else
      data->kgg[0]+=kolih;
    
    bb=kolih*cena;
    data->ostgc[0]=iceb_u_okrug(bb,okrg1);
    continue;
   }

  data->ostg[tp]+=kolih;
  if(mkg == 1 )
    data->kgg[tp]+=kolih*fas;
  else 
    data->kgg[tp]+=kolih;

  bb=kolih*cena;
  data->ostgc[tp]+=iceb_u_okrug(bb,okrg1);

    /*Период*/
//  if(SRAV1(gn,mn,dn,g,m,d) >= 0)
//   if(SRAV1(gk,mk,dk,g,m,d) <= 0)
  if(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0)
     {
/*      printw("Период\n");*/
      data->ostm[tp]+=kolih;
      if(mkg == 1 )
	 data->kgmm[tp]+=kolih*fas;
      else
	 data->kgmm[tp]+=kolih;
      bb=kolih*cena;
      data->ostmc[tp]+=iceb_u_okrug(bb,okrg1);
     }

    /*До периода*/
//  if(SRAV1(gp,mp,dp,g,m,d) <= 0)
  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0)
   {
/*      printw("До периода\n");*/

      if(tp == 2)
	kolih*=(-1);

      prf+=kolih;

      if(mkg == 1 )
	  prf1+=kolih*fas;
      else
	  prf1+=kolih;

      bb=kolih*cena;
      cnn+=iceb_u_okrug(bb,okrg1);
   }
 }
//OSTANOV();
data->ostg[3]=data->ostg[0]+data->ostg[1]-data->ostg[2];
data->kgg[3]=data->kgg[0]+data->kgg[1]-data->kgg[2];
data->ostm[0]=data->ostg[0]+prf;
data->kgmm[0]=data->kgmm[0]+prf1;
data->ostm[3]=data->ostm[0]+data->ostm[1]-data->ostm[2];
data->kgmm[3]=data->kgmm[0]+data->kgmm[1]-data->kgmm[2];

data->ostgc[3]=data->ostgc[0]+data->ostgc[1]-data->ostgc[2];
data->ostmc[0]=data->ostgc[0]+cnn;
data->ostmc[3]=data->ostmc[0]+data->ostmc[1]-data->ostmc[2];

/*Убираем погрешность вычисления*/
for(i=0; i<4 ; i++)
 {
  if(data->ostg[i] < 0.0000000001 && data->ostg[i] > -0.0000000001)
     data->ostg[i]=0.;
  if(data->ostm[i] < 0.0000000001 && data->ostm[i] > -0.0000000001)
    data->ostm[i]=0.;
 }
/*
printw("Склад:%d карточка:%d Цена:%f\n%f %f\n%f %f\n%f %f\n%f %f\n",
skl,nk,cena,
data->ostm[0],data->ostmc[0],data->ostm[1],data->ostmc[1],data->ostm[2],data->ostmc[2],data->ostm[3],data->ostmc[3]);
OSTANOV();
*/
return(0);

}
