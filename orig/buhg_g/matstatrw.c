/* $Id: matstatrw.c,v 1.7 2011-02-21 07:35:55 sasa Exp $ */
/*14.11.2009	17.03.1998	Белых А.И. matstatrw.c
Распечатка массива для графического отображение динамики реализации
товаров за любой период времени
*/
             
#include        <errno.h>
#include        <time.h>
#include        "buhg_g.h"
#include <unistd.h>
#include "dvtmcf3_r.h"

extern char     *organ;

short matstatrw(class dvtmcf3_r_data *data,
short dn,short mn,short gn,short dk,short mk,
short gk,char imaf[])
{
struct  tm      *bf;
time_t		tmm;
FILE		*ff;
unsigned short  i,i1,i2;
double		big;
short		d,m,g;
double		itogo;

time(&tmm);
bf=localtime(&tmm);

sprintf(imaf,"gr%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
 }
iceb_u_startfil(ff);

iceb_u_zagolov(gettext("Движение товарно-материалных ценностей"),dn,mn,gn,dk,mk,gk,organ,ff);



dvtmcf3_prp(data,ff);


/*Определяем наибольшее значение*/
/*
printw("Період %d.%d.%d > %d %d %d Днів=%f/%f\n",
dn,mn,gn,dk,mk,gk,mkdp[0],mkdr[0]);
*/
big=0.;
for(i=0; i < data->mkdp.kolih() ;i++)
 if(data->mkdp.ravno(i) > big)
  big=data->mkdp.ravno(i);
/*printw("Прибутки=%f\n",big);*/
d=dn;
m=mn;
g=gn;  
itogo=0.;
if(big != 0.)
 {
  fprintf(ff,"%s\n",gettext("Приходы"));
  fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff,gettext("   Дата   |Количество|\n"));
  fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
  for(i=31*(mn-1)+dn; i< data->mkdp.kolih() ; i++)
   {
    if(g == gk && m == mk && d > dk)
     break;

    if(d > 31)
     {
      d=1; m++;
     }
    if(m > 12)
     {
      m=1;
      g++;
     }  

    i1=0;
    if(data->mkdp.ravno(i-1) != 0.)
      i1=(int)(data->mkdp.ravno(i-1)*100./big);
 
    fprintf(ff,"%02d.%02d.%d|%10.2f|",d,m,g,data->mkdp.ravno(i-1));
    for(i2=0; i2<i1 ; i2++)
      fprintf(ff,"*");
    fprintf(ff,"\n");
    
    itogo+=data->mkdp.ravno(i-1);

    d++;
  
   }
 }
if(itogo > 0.)
 fprintf(ff,"%*s: %10.2f\n\n",iceb_u_kolbait(9,gettext("Итого")),gettext("Итого"),itogo);

big=0.;
for(i=0;i<data->mkdr.kolih();i++)
 if(data->mkdr.ravno(i) > big)
  big=data->mkdr.ravno(i);
/*printw("Відатки=%f\n",big);*/

d=dn;
m=mn;
g=gn;  

itogo=0.;

if(big != 0.)
 {
  fprintf(ff,"%s\n",gettext("Расходы"));
  fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff,gettext("   Дата   |Количество|\n"));
  fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");

  for(i=31*(mn-1)+dn; i<data->mkdr.kolih() ; i++)
   {
    if(g == gk && m == mk && d > dk)
     break;

    if(d > 31)
     {
      d=1; m++;
     }
    if(m > 12)
     {
      m=1;
      g++;
     }  
 

    i1=0;
    if(data->mkdr.ravno(i-1) != 0.)
      i1=(int)(data->mkdr.ravno(i-1)*100./big);
 
    fprintf(ff,"%02d.%02d.%d|%10.2f|",d,m,g,data->mkdr.ravno(i-1));
    for(i2=0; i2<i1 ; i2++)
      fprintf(ff,"*");
    fprintf(ff,"\n");
    itogo+=data->mkdr.ravno(i-1);  
    d++;
  
   }
 }
if(itogo > 0.)
  fprintf(ff,"%*s: %10.2f\n\n",iceb_u_kolbait(9,gettext("Итого")),gettext("Итого"),itogo);

iceb_podpis(ff,data->window);


fclose(ff);
return(0);
}
