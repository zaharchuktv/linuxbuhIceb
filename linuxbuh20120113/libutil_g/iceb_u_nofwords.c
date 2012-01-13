/*$Id: iceb_u_nofwords.c,v 1.2 2011-07-21 12:43:31 sasa Exp $*/
/*12.07.2011	05.07.2011	Белых А.И.	iceb_u_nofwords.c
Получение числа прописью
дробная часть до 6 знаков полсе запятой !!!!
*/
#include <math.h>
#include "iceb_util.h"

const char *iceb_u_nofwords(double number)
{
static class iceb_u_str str_voz;
char number_of_words[2048];
char drob_str[2048];
char drob_str_char[2048];
char naim_drob[512];
double cel_h=0.;
double drob_h=0.;

memset(number_of_words,'\0',sizeof(number_of_words));

strcpy(number_of_words,iceb_u_propis(number,1));

//printw("%s-%s\n",__FUNCTION__,number_of_words);

str_voz.new_plus(number_of_words);
//printw("%s-%s\n",__FUNCTION__,str_voz.ravno());

drob_h=modf(number,&cel_h);

if(fabs(drob_h) > 0.00000009)
 {
  if(cel_h == 1.0)
   str_voz.plus(gettext(" целая и "));
  else  
   str_voz.plus(gettext(" целых и "));
 }
//printw("%s-%.15f\n",__FUNCTION__,drob_h);

sprintf(drob_str,"%.10g",drob_h);
iceb_u_polen(drob_str,drob_str_char,sizeof(drob_str_char),2,'.');

memset(drob_str,'\0',sizeof(drob_str));
memset(naim_drob,'\0',sizeof(naim_drob));

int kolznak=strlen(drob_str_char);
switch(kolznak)
 {
  case 0: /*нет дробной части*/

   break;

  case 1:

   strcpy(drob_str,iceb_u_propis(drob_h*10.*kolznak,0));
   drob_h=iceb_u_okrug(drob_h,0.1); /*округляем чтобы правильно выполнилось первое условие*/

   if(drob_h == 0.1)
    strcpy(naim_drob,gettext("десятая"));
   else    
    strcpy(naim_drob,gettext("десятых"));
   break;

  case 2:

   strcpy(drob_str,iceb_u_propis(drob_h*10.*kolznak,0));
   drob_h=iceb_u_okrug(drob_h,0.01); /*округляем чтобы правильно выполнилось первое условие*/

   if(drob_h == 0.01)
    strcpy(naim_drob,gettext("сотая"));
   else    
    strcpy(naim_drob,gettext("сотых"));
   break;

  case 3:

   strcpy(drob_str,iceb_u_propis(drob_h*10.*kolznak,0));
   drob_h=iceb_u_okrug(drob_h,0.001); /*округляем чтобы правильно выполнилось первое условие*/

   if(drob_h == 0.001)
    strcpy(naim_drob,gettext("тысячная"));
   else    
    strcpy(naim_drob,gettext("тысячных"));
   break;


  case 4:

   strcpy(drob_str,iceb_u_propis(drob_h*10.*kolznak,0));
   drob_h=iceb_u_okrug(drob_h,0.0001); /*округляем чтобы правильно выполнилось первое условие*/
   if(drob_h == 0.0001) 
     strcpy(naim_drob,gettext("десятитысячная"));
   else    
     strcpy(naim_drob,gettext("десятитысячных"));
   break;


  case 5:

   strcpy(drob_str,iceb_u_propis(drob_h*10.*kolznak,0));
   drob_h=iceb_u_okrug(drob_h,0.000001); /*округляем чтобы правильно выполнилось первое условие*/

   if(drob_h == 0.00001)
    strcpy(naim_drob,gettext("стотысячная"));
   else    
    strcpy(naim_drob,gettext("стотысячных"));
   break;


  case 6:

   strcpy(drob_str,iceb_u_propis(drob_h*10.*kolznak,0));
   drob_h=iceb_u_okrug(drob_h,0.0000001); /*округляем чтобы правильно выполнилось первое условие*/

   if(drob_h == 0.000001)
    strcpy(naim_drob,gettext("милионная"));
   else    
    strcpy(naim_drob,gettext("милионных"));
   break;
  
  default:
   strcpy(naim_drob,gettext("не предусмотренная величина"));
   break;   
 }
str_voz.plus(drob_str);
str_voz.plus(" ",naim_drob);
//printw("%s-%s\n",__FUNCTION__,str_voz.ravno());

return(str_voz.ravno());
}
