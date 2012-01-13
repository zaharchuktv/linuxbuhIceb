/* $Id: rasmothw.c,v 1.15 2011-02-21 07:35:56 sasa Exp $ */
/*14.11.2009    15.05.1997      Белых А.И.      rasmothw.c
Просмотр отсортированного файла и распечатка отчета
*/
#include        <stdlib.h>
#include        <math.h>
#include        <time.h>
#include        <errno.h>
#include	"../headers/buhg_g.h"
#include	"dvtmcf3_r.h"

#define         PL      50  /*Количество строк на первом листе*/
#define         VL      65  /*Количество строк на втором листе*/

extern void     hmoth(short,short,short,short,short,short,short *,short *,short,FILE*,struct tm *);
void itogi(short it,double kol[],double sum[],const char *rp,short sk,double cn,short to,FILE *ff);


extern char     *organ;
extern SQL_baza bd;

void rasmothw(class dvtmcf3_r_data *data,
int kolih_strok,
FILE *ff,FILE *ff2)
{
struct  tm      *bf;
SQL_str         row;
char		strsql[1024];
char            bros[512];
short           kodmz; /*Код материалла*/
short           sklz;      /*Код склада*/
char            kodoz[32]; /*Код контрагента*/
short           sli,kst;
short           tipzz,tp;     /*Приход/расход*/
char            kopz[32];  /*Код операции*/
double          cenaz;     /*цена*/
double          ittip[4],ittips[3];  /*Приход/расход*/
double          itkodm[4],itkodms[3]; /*Код материалла*/
double          itcena[4],itcenas[3]; /*цена*/
double          itkodo[4],itkodos[3]; /*Код контрагента*/
double          itskl[4],itskls[3];   /*Код склада*/
double          itkop[4],itkops[3];   /*Код операции*/
double          bb,bb1;
char            naior[512];
char		oper[40];
short		to;
int		i;
int             i1,i2,i3;
char		kor[32],kop[32];
double		cena,cenau,kolih,voz,oth,fas;
char		str[1024];
int		kodm;
extern double	okrcn;
int		skl;
char		naim[512];
char		ei[32];
char		nn[32];
short		d,m,g;
int		kgrm;
short           dn,mn,gn;
short           dk,mk,gk;
int             kk1=data->spoprp.kolih();  /*Количество операций приходов*/
int             kk2=data->spoprr.kolih();  /*Количество операций расходов*/

//iceb_poldan("Н.Д.С.",bros,"matnast.alx",NULL);
//float pnds=iceb_u_atof(bros);
float pnds=iceb_pnds(NULL);

time_t tmm;
time(&tmm);
bf=localtime(&tmm);
iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window);
voz=oth=0.;

sprintf(strsql,"%s\n",gettext("Распечатываем"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

oper[0]='\0';

iceb_u_zagolov(gettext("Движение товарно-материалных ценностей"),dn,mn,gn,dk,mk,gk,organ,ff);

dvtmcf3_prp(data,ff);

    
fprintf(ff,"\n");

for(i=0; i<4;i++)
 {
  itkop[i]=itskl[i]=ittip[i]=itkodm[i]=itcena[i]=itkodo[i]=0.;
 }
for(i=0; i<3; i++)
 {
  itkops[i]=itskls[i]=ittips[i]=itkodms[i]=itcenas[i]=itkodos[i]=0.;
 }

tipzz=0;
sli=0;
kst=-1;
to=sklz=0;
kodmz=0;
cenaz=-1.; /*Цена может быть нулевой*/
kopz[0]='\0';
kodoz[0]='\0';

float kolstr1=0.;
while(fgets(str,sizeof(str),ff2) != NULL) 
 {
  iceb_pbar(data->bar,kolih_strok,++kolstr1);    
  
  iceb_u_pole(str,bros,1,'|');
  tp=atoi(bros);
  iceb_u_pole(str,bros,2,'|');
  skl=atoi(bros);

  iceb_u_pole(str,kop,3,'|');

  iceb_u_pole(str,bros,4,'|');
  kodm=atoi(bros);

  iceb_u_pole(str,bros,5,'|');
  cena=atof(bros);
  cena=iceb_u_okrug(cena,okrcn);

  iceb_u_pole(str,kor,6,'|');

  iceb_u_pole(str,bros,7,'|');
  kolih=atof(bros);

  iceb_u_pole(str,naim,8,'|');

  iceb_u_pole(str,bros,9,'|');
  cenau=atof(bros);
  cenau=iceb_u_okrug(cenau,okrcn);

  iceb_u_pole(str,ei,10,'|');

  iceb_u_pole(str,nn,11,'|');

  iceb_u_pole(str,bros,12,'|');
  iceb_u_rsdat(&d,&m,&g,bros,0);

  iceb_u_pole(str,bros,13,'|');
  kgrm=atoi(bros);
   
  iceb_u_pole(str,bros,14,'|');
  fas=atof(bros);
  if( data->rk->metka_ras != 0)
   {
    goto vp;
   }
  
  to=0;
  if(oper[0] != '\0' && tp == 2)
    if(iceb_u_pole1(oper,kop,',',0,&i1) == 0 || iceb_u_SRAV(oper,kop,0) == 0)
      to=1;

  /*Приход расход*/
  if(tipzz != tp)
   {

    if(tipzz != 0)
     {
      itogi(7,itkodo,itkodos,naior,0,0.,to,ff);
      itogi(6,itcena,itcenas,"  ",0,cenaz,to,ff);
      itogi(5,itkodm,itkodms,naim,0,0.,to,ff);
      itogi(4,itkop,itkops,kopz,0,0.,to,ff);
      itogi(3,itskl,itskls,"  ",sklz,0.,to,ff);
      itogi(tipzz,ittip,ittips," ",0,0.,to,ff);
      kst=-1;
      sli=0;
      fprintf(ff,"\f");
     }
    if(tp == 1 && tipzz == 0)
     {
      fprintf(ff,"%s.\n",gettext("Приход"));
      sprintf(strsql,"%s.\n",gettext("Приход"));
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
     }
    if(tp == 2 && tipzz == 1)
     { 
      fprintf(ff,"%s.\n",gettext("Расход"));
      sprintf(strsql,"%s.\n",gettext("Расход"));
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
     }
    hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
    tipzz=tp;
    sklz=0;
    kodmz=0;
    cenaz=-1.; /*Цена может быть нулевой*/
    kopz[0]='\0';
    kodoz[0]='\0';
   }

  /*Склад*/
  if(sklz != skl)
   {
    if(sklz != 0)
     {
        if(itkodo[0] != 0.)
         itogi(7,itkodo,itkodos,naior,0,0.,to,ff);
        if(itcena[0] != 0.)
         itogi(6,itcena,itcenas,"  ",0,cenaz,to,ff);
        if(itkodm[0] != 0.)
	 itogi(5,itkodm,itkodms,naim,0,0.,to,ff);
        if(itkop[0] != 0.)
         itogi(4,itkop,itkops,kopz,0,0.,to,ff);
        if(itskl[0] != 0.)
         itogi(3,itskl,itskls,"  ",sklz,0.,to,ff);
      kst=-1;
      sli=0;
      fprintf(ff,"\f");
     }
    fprintf(ff,"%s:%d\n",gettext("Склад"),skl);
    hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);

    sprintf(strsql,"%s:%d\n",gettext("Склад"),skl);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    sklz=skl;
    sli=0;
    kodmz=0;
    cenaz=-1.; /*Цена может быть нулевой*/
    kopz[0]='\0';
    kodoz[0]='\0';

   }

  /*Код операции*/
  if(iceb_u_SRAV(kopz,kop,0) != 0)
   {
    if(kopz[0] != '\0')
     {
	if(itkodo[0] != 0.)
	 { 
	  itogi(7,itkodo,itkodos,naior,0,0.,to,ff);
          hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
         }
	if(itcena[0] != 0.)
         {
     	  itogi(6,itcena,itcenas,"  ",0,cenaz,to,ff);
          hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
         }
	if(itkodm[0] != 0.)
         {
	  itogi(5,itkodm,itkodms,naim,0,0.,to,ff);
          hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
         }
	if(itkop[0] != 0.)
         {
	  itogi(4,itkop,itkops,kopz,0,0.,to,ff);
          hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
         }
      kst=-1;
      sli=0;
      fprintf(ff,"\f");
     }
    strcpy(kopz,kop);
    fprintf(ff,"%s - %s\n",gettext("Операция"),kopz);
    hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);

    sprintf(strsql,"%s: %s\n",gettext("Операция"),kopz);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    kodmz=0;
    cenaz=-1.; /*Цена может быть нулевой*/
    kodoz[0]='\0';
   }

  /*Код материалла*/
  if(kodmz != kodm)
   {
    if(kodmz != 0)
     {
	if(itkodo[0] != 0.)
         {
	  itogi(7,itkodo,itkodos,naior,0,0.,to,ff);
          hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
         }
	if(itcena[0] != 0.)
         {
	  itogi(6,itcena,itcenas,"  ",0,cenaz,to,ff);
          hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
         }
        if(itkodm[0] != 0.)
         {
	  itogi(5,itkodm,itkodms,naim,0,0.,to,ff);
          hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
         }
     }

    hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
    fprintf(ff,"%s - %s ********\n",gettext("Материал"),naim);

    sprintf(strsql,"******** %s ********\n",naim);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    kodmz = kodm;
    cenaz=-1.; /*Цена может быть нулевой*/
    kodoz[0]='\0';
   }

  /*Цена*/
  if(cenaz != cena)
   {
    if(cenaz > -1.)
     {
	if(itkodo[0] != 0.)
         {
	  itogi(7,itkodo,itkodos,naior,0,0.,to,ff);
          hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
         }
	if(itcena[0] != 0.)
         {
	  itogi(6,itcena,itcenas,"  ",0,cenaz,to,ff);
          hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
         }
     }
    cenaz=cena;
    kodoz[0]='\0';
   }

  /*Код контрагента*/
  if(iceb_u_SRAV(kodoz,kor,0) != 0)
   {
    if(kodoz[0] != '\0')
     {
      if(itkodo[0] != 0.)
       {    
        itogi(7,itkodo,itkodos,naior,0,0.,to,ff);
        hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
       }
     }

     memset(bros,'\0',sizeof(bros));
     iceb_u_str kontr;
     iceb_00_kontr(kor,&kontr);

     naior[0]='\0';
     SQLCURSOR cur;
     sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
     kontr.ravno());
     if(sql_readkey(&bd,strsql,&row,&cur) != 1)
      {
       printf("Не знайден контрагент %s !\n",kontr.ravno());
      }
     else
       strncpy(naior,row[0],sizeof(naior)-1);
     
    strcpy(kodoz,kor);
   }
  itkop[0]=itskl[0]=ittip[0]=itkodm[0]=itcena[0]=itkodo[0]=1.;


  ittip[1]+=kolih;
  itskl[1]+=kolih;
  itkodm[1]+=kolih;
  itcena[1]+=kolih;
  itkodo[1]+=kolih;
  itkop[1]+=kolih;

  ittip[2]+=kolih-voz-oth;
  itskl[2]+=kolih-voz-oth;
  itkodm[2]+=kolih-voz-oth;
  itcena[2]+=kolih-voz-oth;
  itkodo[2]+=kolih-voz-oth;
  itkop[2]+=kolih-voz-oth;

  ittip[3]+=oth;
  itskl[3]+=oth;
  itkodm[3]+=oth;
  itcena[3]+=oth;
  itkodo[3]+=oth;
  itkop[3]+=oth;
  if(data->rk->metka_ras == 2)
    bb=kolih*(cena-cenau);
  else  
    bb=kolih*cena;
  bb=iceb_u_okrug(bb,0.01);
  
  ittips[0]+=bb;
  itskls[0]+=bb;
  itkodms[0]+=bb;
  itcenas[0]=itcenas[0]+bb;
  itkodos[0]+=bb;
  itkops[0]+=bb;

  bb1=(kolih-voz-oth)*cena;
  bb1=iceb_u_okrug(bb1,0.01);

  ittips[1]+=bb1;
  itskls[1]+=bb1;
  itkodms[1]+=bb1;
  itcenas[1]+=bb1;
  itkodos[1]+=bb1;
  itkops[1]+=bb1;

    hmoth(dn,mn,gn,dk,mk,gk,&sli,&kst,to,ff,bf);
    if(to == 0)
     {
      fprintf(ff,"%3d %-*s %-*s %-*s %8.5f %8.8g %10.2f %10.2f %-5s %02d.%02d.%d %s %s\n",
      kodm,
      iceb_u_kolbait(30,naim),naim,
      iceb_u_kolbait(4,ei),ei,
      iceb_u_kolbait(4,kop),kop,
      cena,kolih,
      bb,bb+bb*pnds/100.,nn,d,m,g,kodoz,naior);
     }
    if(to == 1)
      fprintf(ff,"%3d %-*s %-*s %-*s %8.5f %8.8g %10.2f %8.8g %10.2f %8.8g %-5s %02d.%02d.%d %s %s\n",
      kodm,
      iceb_u_kolbait(30,naim),naim,
      iceb_u_kolbait(4,ei),ei,
      iceb_u_kolbait(4,kop),kop,
      cena,kolih,
      bb,kolih-voz-oth,bb1,oth,nn,
      d,m,g,kodoz,naior);

vp:;

  /*Заполняем массивы*/

  if((i1=data->mko.find(kodm)) < 0)
    continue;


  if((i3=data->mkg.find(kgrm)) < 0)
    continue;
  

  if(tp == 1)
   {
    i2=data->spoprp.find(kop);
    
    /*printw("%s\n%d %s\n",spoprp,i2,kop);*/

    /*Материалы*/
    data->mkm1p.plus(kolih,i2+i1*kk1);

    if(fas > 0.0000001)
      data->mkm2p.plus(kolih*fas,i2+i1*kk1);
    else
      if(iceb_u_SRAV(ei,"Кг",0) == 0 || iceb_u_SRAV(ei,"кг",0) == 0 || iceb_u_SRAV(ei,"КГ",0) == 0)
        data->mkm2p.plus(kolih,i2+i1*kk1);
     
    data->mkm3p.plus(kolih*cenau,i2+i1*kk1);

    /*Группы материалов*/
    data->mkg1p.plus(kolih,i2+i3*kk1);

    if(fas > 0.0000001)
      data->mkg2p.plus(kolih*fas,i2+i3*kk1);
    else
      if(iceb_u_SRAV(ei,"Кг",0) == 0 || iceb_u_SRAV(ei,"кг",0) == 0 || iceb_u_SRAV(ei,"КГ",0) == 0)
        data->mkg2p.plus(kolih,i2+i3*kk1);
     
    data->mkg3p.plus(kolih*cenau,i2+i3*kk1);

    /*Заполняем массив динамики приходов*/
    data->mkdp.plus(kolih,d+(m-1)*31 + (31*12)*(g-gn));

   }

  if(tp == 2)
   {
    i2=data->spoprr.find(kop);
    
    /*printw("%s\n%d %s\n",spoprr,i2,kop);*/

    /*Материалы*/
    data->mkm1r.plus(kolih-voz-oth,i2+i1*kk2);
    if(data->rk->metka_ras != 2)
     {
     if(fas > 0.0000001)
       data->mkm2r.plus((kolih-voz-oth)*fas,i2+i1*kk2);
     else
      if(iceb_u_SRAV(ei,"Кг",0) == 0 || iceb_u_SRAV(ei,"кг",0) == 0 || iceb_u_SRAV(ei,"КГ",0) == 0)
        data->mkm2r.plus(kolih-voz-oth,i2+i1*kk2);
     }
    if(data->rk->metka_ras != 2)
      bb=(kolih-voz-oth)*cena;
    else
      bb=(kolih-voz-oth)*(cena-cenau);

    bb=iceb_u_okrug(bb,0.01);
    data->mkm3r.plus(bb,i2+i1*kk2);

    /*Группы материалов*/
    data->mkg1r.plus(kolih-voz-oth,i2+i3*kk2);
    if(data->rk->metka_ras != 2)
     {
     if(fas > 0.0000001)
       data->mkg2r.plus((kolih-voz-oth)*fas,i2+i3*kk2);
     else
      if(iceb_u_SRAV(ei,"Кг",0) == 0 || iceb_u_SRAV(ei,"кг",0) == 0 || iceb_u_SRAV(ei,"КГ",0) == 0)
        data->mkg2r.plus(kolih-voz-oth,i2+i3*kk2);
     }
    if(data->rk->metka_ras == 2)
      data->mkg2r.plus(kolih*(cena-cenau),i2+i3*kk2);
    
    if(data->rk->metka_ras == 2)
     bb=(kolih-voz-oth)*(cena-cenau);
    else
     bb=(kolih-voz-oth)*cena;
    bb=iceb_u_okrug(bb,0.01);
    data->mkg3r.plus(bb,i2+i3*kk2);

    /*Отходы*/
    data->mko1r.plus(oth,i2+i1*kk2);
    if(fas > 0.0000001)
      data->mko2r.plus(oth*fas,i2+i1*kk2);
    else
      if(iceb_u_SRAV(ei,"Кг",0) == 0 || iceb_u_SRAV(ei,"кг",0) == 0 || iceb_u_SRAV(ei,"КГ",0) == 0)
         data->mko2r.plus(oth,i2+i1*kk2);
    bb=oth*cena;
    bb=iceb_u_okrug(bb,0.01);
    data->mko3r.plus(bb,i2+i1*kk2);

    /*Заполняем массив динамики расходов*/
    data->mkdr.plus(kolih,d+(m-1)*31 + (31*12)*(g-gn));
   }

 }

if(data->rk->metka_ras == 0)
 {
  itogi(7,itkodo,itkodos,naior,0,0.,to,ff);
  itogi(6,itcena,itcenas,"  ",0,cenaz,to,ff);
  itogi(5,itkodm,itkodms,naim,0,0.,to,ff);
  itogi(4,itkop,itkops,kopz,0,0.,to,ff);
  itogi(3,itskl,itskls,"  ",sklz,0.,to,ff);
  itogi(tipzz,ittip,ittips," ",0,0.,to,ff);
 }

}

/***************************/
/*Шапка материального отчета*/
/**************************/

void            hmoth(short dn,short mn,short gn,short dk,short mk,
short gk,short *sl,short *kst,
short tsh, //0-для об.опер 1-реал на рынке
FILE *ff,struct tm *bf)
{
*kst=*kst+1;

if((*kst >= VL && *sl > 1) || (*kst >= PL && *sl == 1) ||
(*kst == 0 && *sl == 0))
 {
  if((*kst >= VL && *sl > 1) ||  (*kst >= PL && *sl == 1))
      fprintf(ff,"\f");

  *sl=*sl+1;
  *kst=1;

  if(tsh == 0)
   {
    fprintf(ff,"\
%s %d.%d.%d%s %s - %d:%d %50s%s N%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min,
"",gettext("Лист"),
*sl);

    fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(ff,gettext("К/М|  Наименование материалла     |Е.и.|К/О | Цена   |Количес.| Сумма    |Сум. с НДС| Н/Д |  Дата    | Наименование контрагента          |\n"));
    fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------\n");
   }
 }

}

/****************/
/*Выдача итогов*/
/***************/

void itogi(short it,double kol[],double sum[],const char *rp,
short sk,double cn,
short to,  //0-об. опер 1-виездная торговля
FILE *ff)
{
char		bros[512];

if(it == 1)
 {
    fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
    sprintf(bros,"%s:",gettext("Итого по приходу"));
 }

if(it == 2)
 {
    fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
    sprintf(bros,"%s:",gettext("Итого по расходу"));
 }

if(it == 3)
 {
    fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    sprintf(bros,"%s %d:",gettext("Итого по складу"),sk);
 }

if(it == 4)
 {
    fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
    sprintf(bros,"%s %s:",gettext("Итого по операции"),rp);
 }

if(it == 5)
 {
  sprintf(bros,"%s %s:",gettext("Итого по мат-лу"),rp);
 }

if(it == 6)
 {
  sprintf(bros,"%s %.2f:",gettext("Итого по цене"),cn);
 }

if(it == 7)
 {
  sprintf(bros,"%s %s:",gettext("Итого по кон-ту"),rp);
 }
 
if(to == 0)
 { 
  fprintf(ff,"%*s %8.8g %10.2f\n",
  iceb_u_kolbait(53,bros),bros,kol[1],sum[0]);
/*
  printw("%-30.30s %8.8g %10.2f\n",
  bros,kol[1],sum[0]);
  */
 }
if(to == 1)
 {
  fprintf(ff,"%*s %8.8g %10.2f %8.8g %10.2f %8.8g\n",
  iceb_u_kolbait(53,bros),bros,kol[1],sum[0],kol[2],sum[1],kol[3]);
/*
  printw("%-30.30s %8.8g %8.2f %8.8g %8.2f %8.8g\n",
  bros,kol[1],sum[0],kol[2],sum[1],kol[3]);
  */
 }

kol[0]=kol[1]=kol[2]=kol[3]=0.;
sum[0]=sum[1]=sum[2]=0.;

}
