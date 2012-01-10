/*$Id: iceb_vibrek.c,v 1.17 2011-04-06 09:49:19 sasa Exp $*/
/*18.02.2009	07.01.2004	Белых А.И. 	iceb_vibrek.c
Выбор реквизитов

*/
#include "iceb_libbuh.h"

int iceb_vibrek(int metka,char *tablica,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);


int iceb_vibrek(int metka, //0-добавлять 1-заменять
const char *tablica,iceb_u_str *kod,GtkWidget *wpredok)
{
class iceb_u_str naim("");
return(iceb_vibrek(metka,tablica,kod,&naim,wpredok));

}

/****************************************/
/****************************************/

int iceb_vibrek(int metka, //0-добавлять 1-заменять
const char *tablica,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok)
{
iceb_u_str kodv;
//iceb_u_str naim;

if(iceb_u_SRAV(tablica,"Plansh",0) == 0)
 {
  if(iceb_l_plansh(1,&kodv,naim,wpredok) == 0)
   {
   if(metka == 0)
    kod->z_plus(kodv.ravno());
   else
    kod->new_plus(kodv.ravno());
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Kontragent",0) == 0)
 {
  if(iceb_l_kontr(1,&kodv,naim,wpredok) == 0)
   {
   if(metka == 0)
    kod->z_plus(kodv.ravno());
   else
    kod->new_plus(kodv.ravno());
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Sklad",0) == 0)
 {
  if(iceb_l_sklad(1,&kodv,naim,wpredok) == 0)
   {
   if(metka == 0)
    kod->z_plus(kodv.ravno());
   else
    kod->new_plus(kodv.ravno());
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Gkont",0) == 0)
 {
  if(iceb_l_gkontr(1,&kodv,naim,wpredok) == 0)
   {
   if(metka == 0)
    kod->z_plus(kodv.ravno());
   else
    kod->new_plus(kodv.ravno());
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Grup",0) == 0)
 {
  if(iceb_l_grupmat(1,&kodv,naim,wpredok) == 0)
   {
   if(metka == 0)
    kod->z_plus(kodv.ravno());
   else
    kod->new_plus(kodv.ravno());
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Edizmer",0) == 0)
 {
  if(iceb_l_ei(1,&kodv,naim,wpredok) == 0)
   {
   if(metka == 0)
    kod->z_plus(kodv.ravno());
   else
    kod->new_plus(kodv.ravno());
   }
  return(0);
 }


if(iceb_u_SRAV(tablica,"Foroplat",0) == 0)
 {
  if(iceb_l_forop(1,&kodv,naim,wpredok) == 0)
   {
   if(metka == 0)
    kod->z_plus(kodv.ravno());
   else
    kod->new_plus(kodv.ravno());
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Prihod",0) == 0)
 {
  if(iceb_l_opmup(1,&kodv,naim,wpredok) == 0)
   {
   if(metka == 0)
    kod->z_plus(kodv.ravno());
   else
    kod->new_plus(kodv.ravno());
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Rashod",0) == 0)
 {
  if(iceb_l_opmur(1,&kodv,naim,wpredok) == 0)
   {
   if(metka == 0)
    kod->z_plus(kodv.ravno());
   else
    kod->new_plus(kodv.ravno());
   }
  return(0);
 }

kodv.new_plus(gettext("Выбор из этой таблицы непредусмотрено !"));
kodv.ps_plus(tablica);
iceb_menu_soob(&kodv,wpredok);
return(1);

}
/*********************************************/
/*********************************************/

int iceb_vibrek(int metka, //0-добавлять 1-заменять
char *tablica,char *kod,int razmer,GtkWidget *wpredok)
{
iceb_u_str kodv;
iceb_u_str naim;

if(iceb_u_SRAV(tablica,"Plansh",0) == 0)
 {
  if(iceb_l_plansh(1,&kodv,&naim,wpredok) == 0)
   {
   if(metka == 0)
     iceb_u_pstrz(kod,razmer,kodv.ravno());
   else
     strncpy(kod,kodv.ravno(),razmer-1);
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Kontragent",0) == 0)
 {
  if(iceb_l_kontr(1,&kodv,&naim,wpredok) == 0)
   {
   if(metka == 0)
     iceb_u_pstrz(kod,razmer,kodv.ravno());
   else
     strncpy(kod,kodv.ravno(),razmer-1);
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Gkont",0) == 0)
 {
  if(iceb_l_gkontr(1,&kodv,&naim,wpredok) == 0)
   {
   if(metka == 0)
     iceb_u_pstrz(kod,razmer,kodv.ravno());
   else
     strncpy(kod,kodv.ravno(),razmer-1);
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Ggup",0) == 0)
 {
  if(iceb_l_grupmat(1,&kodv,&naim,wpredok) == 0)
   {
   if(metka == 0)
     iceb_u_pstrz(kod,razmer,kodv.ravno());
   else
     strncpy(kod,kodv.ravno(),razmer-1);
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Edizmer",0) == 0)
 {
  if(iceb_l_ei(1,&kodv,&naim,wpredok) == 0)
   {
   if(metka == 0)
     iceb_u_pstrz(kod,razmer,kodv.ravno());
   else
     strncpy(kod,kodv.ravno(),razmer-1);
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Sklad",0) == 0)
 {
  if(iceb_l_sklad(1,&kodv,&naim,wpredok) == 0)
   {
   if(metka == 0)
     iceb_u_pstrz(kod,razmer,kodv.ravno());
   else
     strncpy(kod,kodv.ravno(),razmer-1);
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Foroplat",0) == 0)
 {
  if(iceb_l_forop(1,&kodv,&naim,wpredok) == 0)
   {
   if(metka == 0)
     iceb_u_pstrz(kod,razmer,kodv.ravno());
   else
     strncpy(kod,kodv.ravno(),razmer-1);
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Prihod",0) == 0)
 {
  if(iceb_l_opmup(1,&kodv,&naim,wpredok) == 0)
   {
   if(metka == 0)
     iceb_u_pstrz(kod,razmer,kodv.ravno());
   else
     strncpy(kod,kodv.ravno(),razmer-1);
   }
  return(0);
 }

if(iceb_u_SRAV(tablica,"Rashod",0) == 0)
 {
  if(iceb_l_opmur(1,&kodv,&naim,wpredok) == 0)
   {
   if(metka == 0)
     iceb_u_pstrz(kod,razmer,kodv.ravno());
   else
     strncpy(kod,kodv.ravno(),razmer-1);
   }
  return(0);
 }

kodv.new_plus(gettext("Выбор из этой таблицы непредусмотрено !"));
kodv.ps_plus(tablica);
iceb_menu_soob(&kodv,wpredok);

return(1);

}
