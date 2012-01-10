/*$Id: vuslw.c,v 1.13 2011-02-21 07:35:58 sasa Exp $*/
/*02.11.2005	14.10.2005	Белых А.И.	vuslw.c
Ввод записи в документ учёта услуг
Если вернули 0-запись ввели
             1-нет
*/
#include <ctype.h>
#include <stdlib.h>
#include "buhg_g.h"



int vuslw(short dd,short md,short gd, //Дата документа
int tipz,
int podr,
const char *nomdok,
int metka, //0-материал 1-услуга
float pnds,
GtkWidget *wpredok) 
{
iceb_u_str kodzap;
iceb_u_str repl;
if(metka == 1)
 repl.plus(gettext("Введите код или наименование услуги."));
if(metka == 0)
 repl.plus(gettext("Введите код или наименование материалла."));

if(iceb_menu_vvod1(&repl,&kodzap,40,wpredok) != 0)
 return(1);

int kodus=atoi(kodzap.ravno());
iceb_u_str kodv; 
kodv.plus(kodzap.ravno());
if(kodzap.ravno()[0] == '\0' || isdigit(kodzap.ravno()[0]) == 0)
 {
  char bros[512];
  iceb_u_str naimzap;
  memset(bros,'\0',sizeof(bros)); 
  if(iceb_u_pole(kodzap.ravno(),bros,2,'+') != 0)
    strcpy(bros,kodzap.ravno());
  kodus=0;

  if(isdigit(bros[0]) != 0)
     kodus=atoi(bros);

  naimzap.plus(kodzap.ravno());

  if(kodus != 0)
    naimzap.new_plus(kodus);

  if(metka == 1)
//   if((i=dirusl(&kgru,&kodus,naimus,1,"\0")) != 0)
   if(l_uslugi(1,&kodv,&naimzap,wpredok) != 0)
     return(1);

//   if((i=dirmatr1(&kgru,&kodus,naimus,1,0,"\0",0,0,1)) != 0)
  if(metka == 0)
    if(l_mater1(&kodv,&naimzap,0,"",0,1,wpredok) != 0)
     return(1);
 }


return(vuslw_v(dd,md,gd,tipz,podr,nomdok,kodv.ravno(),metka,-1, pnds,wpredok));

}
