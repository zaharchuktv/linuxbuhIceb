/*$Id: iceb_mydoscopy.c,v 1.19 2011-02-21 07:36:07 sasa Exp $*/
/*24.12.2009	10.07.2001	Белых А.И.	iceb_mydoscopy.c
Копирование файла на дос дискету
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <errno.h>
#include <unistd.h>
#include        "iceb_libbuh.h"


int iceb_mydoscopy(const char *imaf,
GtkWidget *wpredok)
{
char	doscp[112];
char	devdos[112];
char    komand[312];
class iceb_u_str rashir("");
class iceb_u_str imaf_zap;
int metka_dbf=0;

int kolpol=iceb_u_pole2(imaf,'.');
if(kolpol != 0)
 if(iceb_u_polen(imaf,&rashir,kolpol,'.') == 0)
  {
   if(iceb_u_SRAV("DBF",rashir.ravno(),0) == 0 || iceb_u_SRAV("dbf",rashir.ravno(),0) == 0)
    {
     metka_dbf=1;
    }
   if(iceb_u_SRAV("PFZ",rashir.ravno(),0) == 0 || iceb_u_SRAV("pfz",rashir.ravno(),0) == 0)
    {
     metka_dbf=1;
    }
   /*Форма 1дф имеет такие расширения файла являясь dbf файлом*/
   if(iceb_u_SRAV("1",rashir.ravno(),0) == 0 || iceb_u_SRAV("2",rashir.ravno(),0) == 0 ||\
      iceb_u_SRAV("3",rashir.ravno(),0) == 0 || iceb_u_SRAV("4",rashir.ravno(),0) == 0)
    {
     metka_dbf=1;
    }
  }


imaf_zap.plus("doscp");
imaf_zap.plus(getpid());
imaf_zap.plus(".tmp");
//копируем файл для того чтобы вернуть его под старое имя не перекодированным
iceb_cp(imaf,imaf_zap.ravno(),0,wpredok);

if(metka_dbf == 0)
 {
  iceb_u_str titl;
  iceb_u_str zagolovok;
  iceb_u_spisok punkt_m;




  titl.plus(gettext("Выбор"));


  zagolovok.plus(gettext("Выберите нужное"));


  punkt_m.plus(gettext("Записать как есть"));//0
  punkt_m.plus(gettext("Записать перекодированным в DOS кодировку"));//1
  punkt_m.plus(gettext("Записать перекодированным в WIDOWS кодировку"));//2


  int nomer=0;
  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);
  if(nomer < 0 )
   return(1);



  if(nomer > 0)
    iceb_perecod(nomer,imaf,wpredok);
 }

memset(doscp,'\0',sizeof(doscp));
memset(devdos,'\0',sizeof(devdos));
//class iceb_u_str imaf_nastr;
//iceb_imafn("nastsys.alx",&imaf_nastr);
iceb_poldan("Копирование на ДОС дискету",doscp,"nastsys.alx",wpredok);
iceb_poldan("Устройство ДОС для копирования",devdos,"nastsys.alx",wpredok);

if(devdos[0] == '\0')
  strcpy(devdos,"a:");
if(doscp[0] == '\0')
  strcpy(doscp,"mcopy");    

zap:;

sprintf(komand,"%s %s %s",doscp,imaf,devdos);
if(iceb_runsystem(komand,gettext("Записываем на дискету"),wpredok) == 0)
 {
 
  iceb_u_str DANET;
  DANET.plus_ps(gettext("Файл записан на дискету"));
  DANET.plus(gettext("Записать еще на одну дискету ?"));

  if(iceb_menu_danet(&DANET,2,NULL) == 1)
   goto zap;

  rename(imaf_zap.ravno(),imaf);
  return(0);
 }
else
 {
  iceb_u_str DANET;
  DANET.plus_ps(gettext("Ошибка записи !"));
  DANET.plus(gettext("Повторить попытку записи ?"));

  if(iceb_menu_danet(&DANET,2,NULL) == 1)
    goto zap;
 }
rename(imaf_zap.ravno(),imaf);
return(1);

}
