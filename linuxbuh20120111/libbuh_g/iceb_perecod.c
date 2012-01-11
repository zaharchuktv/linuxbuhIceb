/*$Id: iceb_perecod.c,v 1.12 2011-08-29 07:13:46 sasa Exp $*/
/*22.08.2011	22.10.2002	Белых А.И.	iceb_perecod.c
Перекодировка файла в ДОС кодировку
*/
#include <glib.h>
#include <errno.h>
#include        <sys/stat.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include <unistd.h>
#include        "iceb_libbuh.h"

extern class iceb_u_str kodirovka_iceb; /*определяется в iceb_start.c iceb_nastsys.c*/

int iceb_perecod(int metka_kod, /*1 - CP866 2 - WINDOWS-1251 3-из WINDOWS-1251 обратно в родную 4- CP1125 5 cp866 без замены символов*/
const char *imaf,GtkWidget *wpredok)
{
struct stat work;
char stroka[512];
FILE *ff_in;
FILE *ff_out;
int kod_simv=0;
class iceb_u_str rashir("");

if(metka_kod == 0)
 return(0);

if(stat(imaf,&work) != 0)
 {
  sprintf(stroka,"%s-%s %s!",__FUNCTION__,gettext("Нет такого файла"),imaf);
  iceb_menu_soob(stroka,wpredok);
  return(1);
 }
class iceb_u_str fromkod(kodirovka_iceb.ravno());
int kolpol=iceb_u_pole2(imaf,'.');
if(kolpol != 0)
 if(iceb_u_polen(imaf,&rashir,kolpol,'.') == 0)
  {
   if(iceb_u_SRAV("DBF",rashir.ravno(),0) == 0 || iceb_u_SRAV("dbf",rashir.ravno(),0) == 0)
    {
     iceb_menu_soob(gettext("DBF-файлы перекодировать ненужно!"),wpredok);
     return(2);
    }
   if(iceb_u_SRAV("PFZ",rashir.ravno(),0) == 0 || iceb_u_SRAV("pfz",rashir.ravno(),0) == 0)
    {
     iceb_menu_soob(gettext("PFZ-файлы перекодировать ненужно!"),wpredok);
     return(2);
    }
   /*Форма 1дф имеет такие расширения файла являясь dbf файлом*/
   if(iceb_u_SRAV("1",rashir.ravno(),0) == 0 || iceb_u_SRAV("2",rashir.ravno(),0) == 0 ||\
      iceb_u_SRAV("3",rashir.ravno(),0) == 0 || iceb_u_SRAV("4",rashir.ravno(),0) == 0)
    {
     iceb_menu_soob(gettext("DBF-файлы перекодировать ненужно!"),wpredok);
     return(2);
    }
  }


char *masiv=new char[work.st_size+1]; /*плюс 1 для того чтобы записть нулевой байт*/

if((ff_in = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(3);
 }

int nomer=0;
while((kod_simv=fgetc(ff_in)) != EOF)
 masiv[nomer++]=kod_simv;  
masiv[nomer]='\0';

fclose(ff_in);


class iceb_u_str tokod("");
if(metka_kod == 1)
 {
  tokod.new_plus("cp866"); /*TRANSLIT -для игнорирования неизвестных кодов */
  if(metka_kod == 1)
   {
    /*Кодировка CP866 не содержит украинских букв Модификация этой кодировки содержащая украинские буквы называтеся CP1125 или RUSCII*/

    if(iceb_u_strstrm(kodirovka_iceb.ravno(),"utf") == 1)
     {
      gchar *tostr=NULL;
      gsize bytes_written=0;
      gchar fallback[8];
      strcpy(fallback,"?");
      if((tostr=g_convert_with_fallback(masiv,work.st_size,"koi8-u",fromkod.ravno(),fallback,NULL,&bytes_written,NULL)) == NULL)
       {
        char bros[512];
        sprintf(bros,"%s-Ошибка конвертации файла %s -> %s errno=%d\n%s %d",__FUNCTION__,kodirovka_iceb.ravno(),tokod.ravno(),errno,strerror(errno),metka_kod);
        iceb_menu_soob(bros,wpredok);

        delete [] masiv;

        return(4);
       }
      strcpy(masiv,tostr);
      g_free(tostr);
     } 
    /*Заменяем неизвестные символы в этой кодировке на известные*/
    for(int nom=0; nom < work.st_size; nom++)
     {
      if(masiv[nom] == -90) /*Маленькая і*/
       masiv[nom]=105;      /*латинская i*/
      if(masiv[nom] == -74) /*большая І*/
       masiv[nom]=73;       /*латинская I*/

      if(masiv[nom] == -83) /*Маленькая ґ*/
       masiv[nom]=-57;      /*русская г*/
      if(masiv[nom] == -67) /*большая Ґ*/
       masiv[nom]=-25;      /*русская Г*/
      if(masiv[nom] == -73) /*большая Ї*/
       masiv[nom]=73;      /*латинская I*/

      if(masiv[nom] == -89) /*маленькая ї*/
       masiv[nom]=105;      /*латинская i*/

      if(masiv[nom] == -92) /*маленькая є*/
       masiv[nom]=-36;      /*русская э*/

      if(masiv[nom] == -76) /*большая Є*/
       masiv[nom]=-4;      /*русская Э*/
     }
    fromkod.new_plus("koi8-u");
   }

 }

if(metka_kod == 2)
 tokod.new_plus("WINDOWS-1251");



if(metka_kod == 3)
 {
  fromkod.new_plus("WINDOWS-1251");
  tokod.new_plus(kodirovka_iceb.ravno());
 } 

if(metka_kod == 4)
 tokod.new_plus("CP1125");

class iceb_u_str imaf_tmp(__FUNCTION__);
imaf_tmp.plus(getpid());
imaf_tmp.plus(".tmp");

gchar *tostr;
gsize bytes_written=0;
gchar fallback[8];
strcpy(fallback,"?");
if((tostr=g_convert_with_fallback(masiv,work.st_size,tokod.ravno(),fromkod.ravno(),fallback,NULL,&bytes_written,NULL)) == NULL)
 {
  char bros[512];
  sprintf(bros,"%s-Ошибка конвертации файла %s -> %s errno=%d\n%s",__FUNCTION__,kodirovka_iceb.ravno(),tokod.ravno(),errno,strerror(errno));
  iceb_menu_soob(bros,wpredok);

  delete [] masiv;

  return(4);
 }

delete [] masiv;

if(tostr == NULL)
 return(5);

if((ff_out = fopen(imaf_tmp.ravno(),"w")) == NULL)
 {
  g_free(tostr);
  iceb_er_op_fil(imaf_tmp.ravno(),"",errno,wpredok);
  return(6);
 }

for(gsize nom=0; nom < bytes_written; nom++)
  fputc(tostr[nom],ff_out);  

g_free(tostr);

fclose(ff_out);

rename(imaf_tmp.ravno(),imaf);

return(0);
}

