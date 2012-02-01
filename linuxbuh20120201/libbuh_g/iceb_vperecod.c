/*$Id: iceb_vperecod.c,v 1.7 2011-08-29 07:13:46 sasa Exp $*/
/*21.02.2010	15.02.2010	Белых А.И. 	iceb_t_vperecok.c
Определение нужной перекодировки по расширению файла до передачи на WINDOWS машину
-1 - не смогли определить нужно перекодировать или нет
 0 - без перекодировки 
 1 - CP866 
 2 - WINDOWS-1251
*/
#include "iceb_libbuh.h"
int iceb_vperecod_menu(GtkWidget *wpredok);


int iceb_vperecod(const char *imaf,GtkWidget *wpredok)
{
class iceb_u_str rashir("");

int kolpol=iceb_u_pole2(imaf,'.');

if(kolpol == 0)
 return(iceb_vperecod_menu(wpredok));
 
if(iceb_u_polen(imaf,&rashir,kolpol,'.') != 0)
 return(iceb_vperecod_menu(wpredok));

if(iceb_u_SRAV("DBF",rashir.ravno(),0) == 0 || iceb_u_SRAV("dbf",rashir.ravno(),0) == 0)
 {
  return(0);
 }
if(iceb_u_SRAV("PFZ",rashir.ravno(),0) == 0 || iceb_u_SRAV("pfz",rashir.ravno(),0) == 0)
 {
  return(0);
 }
  /*Форма 1дф имеет такие расширения файла являясь dbf файлом*/
if(iceb_u_SRAV("1",rashir.ravno(),0) == 0 || iceb_u_SRAV("2",rashir.ravno(),0) == 0 ||\
   iceb_u_SRAV("3",rashir.ravno(),0) == 0 || iceb_u_SRAV("4",rashir.ravno(),0) == 0)
 {
  return(0);
 }

if(iceb_u_SRAV("TXT",rashir.ravno(),0) == 0 || iceb_u_SRAV("txt",rashir.ravno(),0) == 0)
 {
  return(2);
 }
if(iceb_u_SRAV("LST",rashir.ravno(),0) == 0 || iceb_u_SRAV("lst",rashir.ravno(),0) == 0)
 {
  return(2);
 }
if(iceb_u_SRAV("XML",rashir.ravno(),0) == 0 || iceb_u_SRAV("xml",rashir.ravno(),0) == 0)
 {
  return(2);
 }

return(iceb_vperecod_menu(wpredok));


}
/***************************/
/*меню выбора*/
/****************************/
int iceb_vperecod_menu(GtkWidget *wpredok)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;




titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Записать как есть"));//0
punkt_m.plus(gettext("Записать перекодированным в WIDOWS-1251"));//2


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

switch(nomer)
 {
  case -1:
    return(-1);
  case 0:
    return(0);
  case 1:
    return(2);        
 }

return(-1);





}
