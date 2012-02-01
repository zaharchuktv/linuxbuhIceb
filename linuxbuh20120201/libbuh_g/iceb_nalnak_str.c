/*$Id: iceb_nalnak_str.c,v 1.2 2011-08-29 07:13:46 sasa Exp $*/
/*23.07.2011	13.01.2011	Белых А.И.	iceb_nalnak_str.c
Вывод строки в счет-фактуру
*/
#include <stdio.h>
#include "iceb_libbuh.h"


void iceb_nalnak_str(int metka_str,
int tipnn,
const char *razdel,const char *datop,
const char *naim,
const char *ei,
double kolih,
double cena,
double suma,
FILE *ff)
{

if(metka_str == 1)
  fprintf(ff,"|%-4s|%-10s|%-*.*s|%-*s|%-*.*s|%10s",
  razdel,datop,
  iceb_u_kolbait(40,naim),iceb_u_kolbait(40,naim),naim,
  iceb_u_kolbait(7,gettext("руб.")),gettext("руб."),
  iceb_u_kolbait(9,gettext("Услуга")),iceb_u_kolbait(9,gettext("Услуга")),gettext("Услуга"),
  iceb_prcn(cena));
else
  fprintf(ff,"|%-4s|%-10s|%-*.*s|%-*.*s|%9.9g|%10s",
  razdel,datop,
  iceb_u_kolbait(40,naim),iceb_u_kolbait(40,naim),naim,
  iceb_u_kolbait(7,ei),iceb_u_kolbait(7,ei),ei,
  kolih,iceb_prcn(cena));

if(tipnn == 0)
  fprintf(ff,"|%10s|%9s|%9s|%9s|%11s|\n",
   iceb_prcn(suma)," "," "," "," ");

if(tipnn == 1)
  fprintf(ff,"|%10s|%9s|%9s|%9s|%11s|\n",
  " ",iceb_prcn(suma)," "," "," ");

if(tipnn == 2)
  fprintf(ff,"|%10s|%9s|%9s|%9s|%11s|\n",
  " "," ",iceb_prcn(suma)," "," ");

if(tipnn == 3)
  fprintf(ff,"|%10s|%9s|%9s|%9s|%11s|\n",
  " "," "," ",iceb_prcn(suma)," ");

for(int nom=40; nom < iceb_u_strlen(naim); nom+=40)
 fprintf(ff,"|%4s|%-10s|%-*.*s|%-7.7s|%9.9s|%10s|%10s|%9s|%9s|%9s|%11s|\n",
  " "," ",
  iceb_u_kolbait(40,iceb_u_adrsimv(nom,naim)),
  iceb_u_kolbait(40,iceb_u_adrsimv(nom,naim)),
  iceb_u_adrsimv(nom,naim),
  " "," "," "," "," "," "," "," ");

}
 
