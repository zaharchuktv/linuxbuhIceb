/*$Id: menuvnpw.c,v 1.6 2011-02-21 07:35:55 sasa Exp $*/
/*15.06.2009	20.05.2009	Белых А.И.	menuvnpw.c
Выбор файла настройки проводок
*/

//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

extern char *organ;

void menuvnpw(int metka,/*1-материальный учёт 2-учёт услуг 3-учёт основных средств*/
int tipz, /*0 всё 1-приходные 2-расходные*/
GtkWidget *wpredok)
{
int kom=0,kom1=0;
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));
zagolovok.plus(organ,40);
zagolovok.ps_plus(gettext("Выберите нужное"));

if(metka != 3)
 {

  punkt_m.plus(gettext("Настройка проводок в таблице"));//0
  punkt_m.plus(gettext("Настройка списков проводок"));//1


  if((kom=iceb_menu_mv(&titl,&zagolovok,&punkt_m,kom,NULL)) < 0)
   return;
 }
else
 kom=1;

if(tipz == 1)
 {
  if(metka == 1)
   {
    if(kom == 0)
      iceb_f_redfil("avtpro.alx",0,wpredok);
    else
      iceb_f_redfil("avtpromu1.alx",0,wpredok);
    return;
   }    
  if(metka == 2)
   {
    if(kom == 0)
      iceb_f_redfil("avtprouslp.alx",0,wpredok);
    else
      iceb_f_redfil("avtprousl1.alx",0,wpredok);
   }    
  if(metka == 3)
   {
    iceb_f_redfil("avtprouos1.alx",0,wpredok);
   }    
  return;
 }

if(tipz == 2)
 {
  if(metka == 1)
   {
    if(kom == 0)
      iceb_f_redfil("avtpro.alx",0,wpredok);
    else
      iceb_f_redfil("avtpromu2.alx",0,wpredok);
    return;
   }    
  if(metka == 2)
   {
    if(kom == 0)
      iceb_f_redfil("avtprouslr.alx",0,wpredok);
    else
      iceb_f_redfil("avtprousl2.alx",0,wpredok);
   }    
  if(metka == 3)
   {
    iceb_f_redfil("avtprouos2.alx",0,wpredok);
   }    
  return;
 }

switch (kom)
 {
  case 2:
  case -1:
    return;

  case 0:
    if(metka == 1)
     {
      iceb_f_redfil("avtpro.alx",0,wpredok);
      return;
     }    

    punkt_m.free_class();
    punkt_m.plus(gettext("Настройка выполнения проводок для приходных документов"));//0
    punkt_m.plus(gettext("Настройка выполнения проводок для расходных документов"));//1


    kom1=0;
    if((kom1=iceb_menu_mv(&titl,&zagolovok,&punkt_m,kom1,NULL)) < 0)
     return;

    printf("kom1=%d\n",kom1);
    switch(kom1)
     {

      case 0:
        iceb_f_redfil("avtprouslp.alx",0,wpredok);
        break;

      case 1:
        iceb_f_redfil("avtprouslr.alx",0,wpredok);
        break;
     }
    

    break;

  case 1:

      punkt_m.free_class();
      punkt_m.plus(gettext("Настройка выполнения проводок для приходных документов"));//0
      punkt_m.plus(gettext("Настройка выполнения проводок для расходных документов"));//1


      kom1=0;
      if((kom1=iceb_menu_mv(&titl,&zagolovok,&punkt_m,kom1,NULL)) < 0)
       return;

      switch(kom1)
       {

        case 0:
          if(metka == 1)
            iceb_f_redfil("avtpromu1.alx",0,wpredok);
          if(metka == 2)
            iceb_f_redfil("avtprousl1.alx",0,wpredok);
          if(metka == 3)
            iceb_f_redfil("avtprouos1.alx",0,wpredok);
          break;
        case 1:
          if(metka == 1)
            iceb_f_redfil("avtpromu2.alx",0,wpredok);
          if(metka == 2)
            iceb_f_redfil("avtprousl2.alx",0,wpredok);
          if(metka == 3)
            iceb_f_redfil("avtprouos2.alx",0,wpredok);
          break;
       }
      
    break;
 }


}
