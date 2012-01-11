/*$Id: buh_rhw.c,v 1.6 2011-02-21 07:35:51 sasa Exp $*/
/*09.11.2009	07.11.2006	Белых А.И.	buh_rhw.c
Распечатка шахматки
Возвращаем итог по шахматке
*/
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

int buh_rh_sumk(int sk,int kolih_d,int kolih_k,class iceb_u_double *sum_pr,int metka);
void buh_rh_ps(int kolih_d,int kolih_k,class iceb_u_double *sum_pr,int metka_dk,FILE *ff_dv);


double buh_rhw(int metka_dkv, //0-дебет по вертикали 1-кредит по вертикали
class iceb_u_spisok *sheta_deb,
class iceb_u_spisok *sheta_kre,
class iceb_u_double *sum_pr,
FILE *ff)
{

int kolih_d=sheta_deb->kolih();
int kolih_k=sheta_kre->kolih();
double suma=0.;
double itogo_gor=0.;
double itogo_kol=0.;

if(metka_dkv == 0)
 {
  //Распечатываем дебет по вертикали кредит по горизонтали
  //*******************************************************
  //Первая полка над шапкой
  buh_rh_ps(kolih_d,kolih_k,sum_pr,0,ff);

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(10,gettext("Счёт")),iceb_u_kolbait(10,gettext("Счёт")),gettext("Счёт"));

  for(int ii=0; ii < kolih_k; ii++)
   {
    if(buh_rh_sumk(ii,kolih_d,kolih_k,sum_pr,0) == 0)
       continue;
    fprintf(ff,"%-*.*s|",iceb_u_kolbait(10,sheta_kre->ravno(ii)),iceb_u_kolbait(10,sheta_kre->ravno(ii)),sheta_kre->ravno(ii));
   }

  fprintf(ff,"%-*.*s|\n",iceb_u_kolbait(10,gettext("Итого")),iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));


  //Вторая полка над шапкой
  buh_rh_ps(kolih_d,kolih_k,sum_pr,0,ff);

  itogo_gor=0.; //Итого по горизонтальной строке
  itogo_kol=0.; //Итого по колонке
  for(int sd=0; sd < kolih_d; sd++)
   {
    //проверяем строку по горизонтали есть ли хоть одна сумма
    for(int sk=0; sk < kolih_k; sk++)
      if((suma=sum_pr->ravno(sd*kolih_k+sk)) != 0.)
       break;
    if(suma == 0.)
     continue;
    itogo_gor=0.;
    fprintf(ff,"%-10.10s|",sheta_deb->ravno(sd));
    for(int sk=0; sk < kolih_k; sk++)
     {
      if(buh_rh_sumk(sk, kolih_d,kolih_k,sum_pr,0) == 0)
       continue;
      suma=sum_pr->ravno(sd*kolih_k+sk);
      if(suma != 0.)
       fprintf(ff,"%10.2f|",suma);
      else 
       fprintf(ff,"%10s|","");
      itogo_gor+=suma;
      
     }
    fprintf(ff,"%10.2f|\n",itogo_gor);
   }
  buh_rh_ps(kolih_d,kolih_k,sum_pr,0,ff);

  fprintf(ff,"%*.*s|",iceb_u_kolbait(10,gettext("Итого")),iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));
  itogo_gor=0.;
  for(int sk=0; sk < kolih_k; sk++)
   {
    if(buh_rh_sumk(sk, kolih_d,kolih_k,sum_pr,0) == 0)
     continue;
    itogo_kol=0.;
    for(int sd=0; sd < kolih_d; sd++)
     {
      suma=sum_pr->ravno(sd*kolih_k+sk);
      itogo_kol+=suma;
     }
    if(itogo_kol != 0.)
     fprintf(ff,"%10.2f|",itogo_kol);
    else 
       fprintf(ff,"%10s|","");
    itogo_gor+=itogo_kol;
   }
  fprintf(ff,"%10.2f|\n",itogo_gor);

 }

if(metka_dkv == 1)
 {
  //Распечатываем кредит по вертикали дебет по горизонтали
  //*******************************************************
  //Первая полка над шапкой
  buh_rh_ps(kolih_d,kolih_k,sum_pr,1,ff);

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(10,gettext("Счёт")),iceb_u_kolbait(10,gettext("Счёт")),gettext("Счёт"));

  for(int ii=0; ii < kolih_d; ii++)
   {
    if(buh_rh_sumk(ii,kolih_d,kolih_k,sum_pr,1) == 0)
       continue;
    fprintf(ff,"%-10.10s|",sheta_deb->ravno(ii));
   }

  fprintf(ff,"%-*.*s|\n",iceb_u_kolbait(10,gettext("Итого")),iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));


  //Вторая полка над шапкой
  buh_rh_ps(kolih_d,kolih_k,sum_pr,1,ff);

  itogo_gor=0.; //Итого по горизонтальной строке
  itogo_kol=0.; //Итого по колонке

  for(int sk=0; sk < kolih_k; sk++)
   {
    //проверяем строку по горизонтали есть ли хоть одна сумма
    for(int sd=0; sd < kolih_d; sd++)
      if((suma=sum_pr->ravno(sd*kolih_k+sk)) != 0.)
       break;
    if(suma == 0.)
     continue;    

    itogo_gor=0.;
    fprintf(ff,"%-10.10s|",sheta_kre->ravno(sk));
    for(int sd=0; sd < kolih_d; sd++)
     {
      if(buh_rh_sumk(sd, kolih_d,kolih_k,sum_pr,1) == 0)
       continue;
      suma=sum_pr->ravno(sd*kolih_k+sk);
      if(suma != 0.)
       fprintf(ff,"%10.2f|",suma);
      else 
       fprintf(ff,"%10s|","");
      itogo_gor+=suma;
      
     }
    fprintf(ff,"%10.2f|\n",itogo_gor);
   }
  buh_rh_ps(kolih_d,kolih_k,sum_pr,1,ff);

  fprintf(ff,"%-*.*s|\n",iceb_u_kolbait(10,gettext("Итого")),iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));
  itogo_gor=0.;
  for(int sd=0; sd < kolih_d; sd++)
   {
    if(buh_rh_sumk(sd, kolih_d,kolih_k,sum_pr,1) == 0)
     continue;
    itogo_kol=0.;
    for(int sk=0; sk < kolih_k; sk++)
     {
      suma=sum_pr->ravno(sd*kolih_k+sk);
      itogo_kol+=suma;
     }

    if(itogo_kol != 0.)
      fprintf(ff,"%10.2f|",itogo_kol);
    else 
       fprintf(ff,"%10s|","");
    itogo_gor+=itogo_kol;
   }
  fprintf(ff,"%10.2f|\n",itogo_gor);
 }

return(itogo_gor);

}

/****************************/
/*выдача подчёркивающей строки*/
/******************************/
void buh_rh_ps(int kolih_d,int kolih_k,class iceb_u_double *sum_pr,int metka_dk,FILE *ff)
{
if(metka_dk == 0)
 for(int ii=0; ii < kolih_k+2; ii++) 
  {
   if(ii > 0 && ii < kolih_k+1)
    if(buh_rh_sumk(ii-1,kolih_d,kolih_k,sum_pr,metka_dk) == 0)
     continue;

   fprintf(ff,"-----------");

  }

if(metka_dk == 1)
 for(int ii=0; ii < kolih_d+2; ii++) 
  {
   if(ii > 0 && ii < kolih_d+1)
    if(buh_rh_sumk(ii-1,kolih_d,kolih_k,sum_pr,metka_dk) == 0)
     continue;

   fprintf(ff,"-----------");

  }
fprintf(ff,"\n");

}

/*************************/
/*Получить ответ 0 в колонке ничено нет 1-есть*/
/****************************/

int buh_rh_sumk(int sk, //номер колонки, которую проверяем
int kolih_d,
int kolih_k,
class iceb_u_double *sum_pr,
int metka) //0-дебет по вертикали 1-кредит по вертикали
{
if(metka == 0)  
 for(int shd=0; shd < kolih_d; shd++)
  if(sum_pr->ravno(shd*kolih_k+sk) != 0.)
   return(1);

if(metka == 1)  
 for(int shd=0; shd < kolih_k; shd++)
  {
   if(sum_pr->ravno(sk*kolih_k+shd) != 0.)
    {
     return(1);
    }

  }

return(0);

}
