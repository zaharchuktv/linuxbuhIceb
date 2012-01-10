/* $Id: iceb_perzap.c,v 1.11 2011-02-21 07:36:07 sasa Exp $ */
/*20.03.2010	07.05.2005	Белых А.И.	iceb_perzap.c
Определение первого запуска программы в текущем дне
Если файла нет или дата в нем не равна текущей дате - значит
первый запуск,удаляем все файлы *.lst, и в файл записываем текущюю дату
Если вернули 0- всё впорядке
             1- нет
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        "iceb_libbuh.h"

int iceb_perzap1(short d,short m,short g,const char *imaf,int metka_udf,GtkWidget *wpredok);

extern char *imabaz;

int iceb_perzap(int metka, //0-удалять всегда 1-проверять включено удаление или нет
GtkWidget *wpredok)
{
FILE		*ff;
char		imaf[56];
char		str[512];
short		d,m,g;
char		bros[512],bros1[512];
int metka_udf=0; /*0-удалять 1-нет*/
if(metka == 1)
 {
  iceb_poldan("Удаление распечаток",bros,"nastsys.alx",wpredok);
  if(iceb_u_SRAV(bros,"Вкл",1) != 0)
   {
    memset(bros1,'\0',sizeof(bros1));
    sprintf(bros1,"%s",".iceb_nastr.alx");
    iceb_u_poldan("Удаление распечаток",bros,bros1);
    if(iceb_u_SRAV(bros,"Вкл",1) != 0)
      metka_udf=1;
   }
 }
strcpy(imaf,".iceb.dat");
if((ff = fopen(imaf,"r")) == NULL)
 {
  if(errno != ENOENT)
   {
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return(1);
   }
  else
   return(iceb_perzap1(0,0,0,imaf,metka_udf,wpredok));
 }

fgets(str,sizeof(str),ff);  
iceb_u_rsdat(&d,&m,&g,str,1);
fclose(ff);

return(iceb_perzap1(d,m,g,imaf,metka_udf,wpredok));
return(0);
}
/*******************/
/*******************/
int iceb_perzap1(short d,short m,short g,const char *imaf,int metka_udf,GtkWidget *wpredok)
{
struct  tm      *bf;
time_t tmm=time(&tmm);
bf=localtime(&tmm);

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  if(errno == EACCES) /*Permission denied*/
   iceb_er_op_fil(imaf,"Программа должна запускаться из домашнего каталога оператора!",errno,wpredok);
  else
   iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }

fprintf(ff,"%d.%d.%d\n%d:%d:%d\n",
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
bf->tm_hour,bf->tm_min,bf->tm_sec);
fprintf(ff,"NAMEBAZ|%s\n",imabaz);
fclose(ff);


if(d != bf->tm_mday || m != bf->tm_mon+1 || g != bf->tm_year+1900)
 {
  class iceb_u_str repl;
  repl.plus(gettext("Здравствуйте!"));
  repl.ps_plus(gettext("Удачной вам работы и хорошего настроения!"));  
  if(bf->tm_mday == 16 && bf->tm_mon+1 == 6)
   {
    repl.ps_plus(gettext("Можете поздравить с Днем рождения моего разработчика: Белых Александра Ивановича"));
   }
  iceb_menu_soob(&repl,wpredok);
  if(metka_udf == 0)
   iceb_delfil(".","lst,lst~,tmp,tmp~",wpredok);
 }
return(0);
}
