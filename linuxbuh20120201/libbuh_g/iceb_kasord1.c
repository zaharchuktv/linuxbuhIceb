/*$Id: iceb_kasord1.c,v 1.20 2011-02-21 07:36:07 sasa Exp $*/
/*10.12.2010	03.10.2000	Белых А.И.	iceb_kasord1.c
Распечатка приходного кассового ордера
*/
#include <stdlib.h>
#include        <errno.h>
#include <unistd.h>
#include	"iceb_libbuh.h"

extern SQL_baza	bd;

void iceb_kasord1_0(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,double suma,const char *fio,const char *osnov,const char *dopol,GtkWidget *wpredok);
void iceb_kasord1_1(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,double suma,const char *fio,const char *osnov,const char *dopol,const char*,FILE *ff_f,GtkWidget *wpredok);

void	iceb_kasord1(const char *imaf,
const char *nomd,
short dd,short md,short gd,
const char *shet,
const char *shetkas,
double suma,const char *fio,const char *osnov,const char *dopol,
const char *kod_cel_naz,
GtkWidget *wpredok)
{
if(iceb_u_sravmydat(16,6,2009,dd,md,gd) > 0)
  iceb_kasord1_0(imaf,nomd,dd,md,gd,shet,shetkas,suma,fio,osnov,dopol,wpredok);
else
  iceb_kasord1_1(imaf,nomd,dd,md,gd,shet,shetkas,suma,fio,osnov,dopol,kod_cel_naz,NULL,wpredok);

}
/*******************************************/
/*******************************************/

void	iceb_kasord1_0(const char *imaf,
const char *nomd,
short dd,short md,short gd,
const char *shet,
const char *shetkas,
double suma,const char *fio,const char *osnov,const char *dopol,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
SQL_str		row;
SQLCURSOR       cur;
char		naim[512];
char		kodedrp[24];
FILE		*ff;
char		strf[1024];
short		kols;
char		sumac[200];


/*printf("iceb_kasord1_0\n");*/

memset(naim,'\0',sizeof(naim));
memset(kodedrp,'\0',sizeof(kodedrp));
memset(sumac,'\0',sizeof(sumac));
iceb_u_preobr(suma,sumac,0);
sprintf(strsql," (%.2f)",suma);
strcat(sumac,strsql);

/*Читаем реквизиты организации*/

sprintf(strsql,"select naikon,kod from Kontragent where kodkon='00'");

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  strncpy(naim,row[0],sizeof(naim)-1);
  strncpy(kodedrp,row[1],sizeof(kodedrp)-1);
 }

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

sprintf(strsql,"select str from Alx where fil='kasorder.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob("Не найдены настройки kasorder.alx",wpredok);
  return;
 }


kols=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  strncpy(strf,row_alx[0],sizeof(strf)-1);
  kols++;
  switch (kols)
   {
    case 1:
     break;

    case 2:
      iceb_u_bstab(strf,naim,0,40,1);
      iceb_u_bstab(strf,naim,76,116,1);
      break;     

    case 6:
      iceb_u_bstab(strf,kodedrp,19,31,1);
      iceb_u_bstab(strf,nomd,100,110,1);
      break;     

    case 10:
      iceb_u_bstab(strf,fio,91,118,1);
      break;     

    case 11:
      iceb_u_bstab(strf,&fio[118-91],77,116,1);
      break;

    case 13:
      iceb_u_bstab(strf,osnov,87,118,1);
      break;     

    case 14:
      if(iceb_u_strlen(osnov) > 31)
       iceb_u_bstab(strf,iceb_u_adrsimv(31,osnov),77,118,1);
      break;

    case 15:
      iceb_u_bstab(strf,nomd,1,9,1);
      sprintf(strsql,"%02d.%02d.%d",dd,md,gd);
      iceb_u_bstab(strf,strsql,9,20,1);
      iceb_u_bstab(strf,shet,23,32,1);
      iceb_u_bstab(strf,shetkas,34,40,1);
      sprintf(strsql,"%15.2f",suma);
      iceb_u_bstab(strf,strsql,41,56,1);
      if(iceb_u_strlen(osnov) > 31+41)
       iceb_u_bstab(strf,iceb_u_adrsimv(31+41,osnov),77,118,1);
      
      break;


    case 17:
      iceb_u_bstab(strf,fio,14,71,1);
      iceb_u_bstab(strf,sumac,79,118,1);
      break;     

    case 18:
      iceb_u_bstab(strf,osnov,10,72,1);
      iceb_u_bstab(strf,&sumac[118-79],77,116,1);
      break;     

    case 19:
      if(iceb_u_strlen(osnov) > 62)
       iceb_u_bstab(strf,iceb_u_adrsimv(62,osnov),0,72,1);
      break;     

    case 20:
      iceb_u_bstab(strf,sumac,14,71,1);
      break;     

    case 21:
      sprintf(strsql,"%02d.%02d.%d%s",
      dd,md,gd,gettext("г."));
      iceb_u_bstab(strf,strsql,84,102,1);
      break;     

    case 22:
      iceb_u_bstab(strf,dopol,9,71,1);
      break;     
      
   }
  fprintf(ff,"%s",strf);
  memset(strf,'\0',sizeof(strf));

 }
 
fclose(ff);
iceb_ustpeh(imaf,1,wpredok);

}
/*****************************************/
/*****************************************/

void iceb_kasord1_1(const char *imaf,
const char *nomd,
short dd,short md,short gd,
const char *shet,
const char *shetkas,
double suma,const char *fio,const char *osnov,const char *dopol,
const char *kod_cel_naz,
FILE *ff_f,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
SQL_str		row;
SQLCURSOR       cur;
char		naim[512];
char		kodedrp[20];
char		strf[1024];
short		kols;
char		sumac[200];
char naim_mes[50];
FILE		*ff=ff_f;
class iceb_fioruk_rk glbuh;
char glbuh_str[512];
char kassir_str[512];

iceb_fioruk(2,&glbuh,wpredok); /*узнаём фамилию главного бухгалтера*/

/*записываем со смещением в право*/
memset(glbuh_str,'\0',sizeof(glbuh_str));
sprintf(glbuh_str,"%*.*s",
iceb_u_kolbait(35,glbuh.famil_inic.ravno()),
iceb_u_kolbait(35,glbuh.famil_inic.ravno()),
glbuh.famil_inic.ravno());

for(int nom=0; nom < (int)strlen(glbuh_str); nom++)
 if(glbuh_str[nom] == ' ')
  glbuh_str[nom]='_';

memset(kassir_str,'\0',sizeof(kassir_str));

/*Чтобы узнать фамилию кассира нужно узнать номер кассы*/
sprintf(strsql,"select kassa from Kasord where nomd='%s' and datd='%04d-%02d-%02d' and tp=1",
nomd,gd,md,dd);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  sprintf(strsql,"select fio from Kas where kod=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   sprintf(kassir_str,"%*.*s",iceb_u_kolbait(40,row[0]),iceb_u_kolbait(40,row[0]),row[0]); /*чтобы получить со смещением вправо*/
 }
for(int nom=0; nom < (int)strlen(kassir_str); nom++)
 if(kassir_str[nom] == ' ')
  kassir_str[nom]='_';

memset(naim_mes,'\0',sizeof(naim_mes));
memset(naim,'\0',sizeof(naim));
memset(kodedrp,'\0',sizeof(kodedrp));
memset(sumac,'\0',sizeof(sumac));

iceb_u_preobr(suma,sumac,0);
sprintf(strsql," (%.2f)",suma);
strcat(sumac,strsql);
/*Читаем реквизиты организации*/

sprintf(strsql,"select naikon,kod from Kontragent where kodkon='00'");

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  strncpy(naim,row[0],sizeof(naim)-1);
  strncpy(kodedrp,row[1],sizeof(kodedrp)-1);
 }


if(imaf[0] != '\0')
 {
  if((ff = fopen(imaf,"w")) == NULL)
    {
     iceb_er_op_fil(imaf,"",errno,wpredok);
     return;
    }
 }

sprintf(strsql,"select str from Alx where fil='kasord1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob("Не найдены настройки kasord1.alx",wpredok);
  return;
 }


iceb_mesc(md,1,naim_mes);

kols=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  strncpy(strf,row_alx[0],sizeof(strf)-1);
  kols++;
  switch (kols)
   {
    case 7:
      iceb_u_bstab(strf,kodedrp,28,54,1);
      break;

    case 8:
      iceb_u_bstab(strf,naim,0,54,1);
      iceb_u_bstab(strf,naim,58,112,1);
      break;     

    case 10:
      iceb_u_bstab(strf,nomd,29,54,1);
      break;     

    case 11:
      sprintf(strsql,"%02d",dd);
      iceb_u_bstab(strf,strsql,5,7,1);

      sprintf(strsql,"%s %d р.",naim_mes,gd);
      iceb_u_bstab(strf,strsql,9,54,1);

      iceb_u_bstab(strf,nomd,91,112,1);
      break;     

    case 12:

      sprintf(strsql,"%02d",dd);
      iceb_u_bstab(strf,strsql,63,65,1);
      sprintf(strsql,"%s %d р.",naim_mes,gd);
      iceb_u_bstab(strf,strsql,67,112,1);

      break;     

    case 14:
      iceb_u_bstab(strf,fio,70,112,1);
      break;

    case 16:
      iceb_u_bstab(strf,osnov,66,112,1);
       
      break;

    case 17:
      iceb_u_bstab(strf,shet,1,15,1);
      iceb_u_bstab(strf,shetkas,16,26,1);
      sprintf(strsql,"%.2f",suma);
      iceb_u_bstab(strf,strsql,27,40,1);
      if(atoi(kod_cel_naz) != 0)
       iceb_u_bstab(strf,kod_cel_naz,41,51,1);

      if(iceb_u_strlen(osnov) > 46)
        iceb_u_bstab(strf,iceb_u_adrsimv(46,osnov),58,112,1);

      break;     

    case 18:
      iceb_u_bstab(strf,fio,13,54,1);

      if(iceb_u_strlen(osnov) > 100)
        iceb_u_bstab(strf,iceb_u_adrsimv(100,osnov),58,112,1);

      break;     

    case 19:
      iceb_u_bstab(strf,osnov,9,54,1);
      iceb_u_bstab(strf,sumac,62,112,1);
      break;     

      break;     

    case 20:
      if(iceb_u_strlen(osnov) > 45)
       iceb_u_bstab(strf,iceb_u_adrsimv(45,osnov),0,54,1);
      if(iceb_u_strlen(sumac) > 50)
        iceb_u_bstab(strf,iceb_u_adrsimv(50,sumac),58,112,1);
      break;     

    case 21:
      if(iceb_u_strlen(osnov) > 99)
       iceb_u_bstab(strf,iceb_u_adrsimv(99,osnov),0,54,1);
      break;     

    case 22:
      iceb_u_bstab(strf,sumac,5,54,1);
      break;     

    case 23:
      if(iceb_u_strlen(sumac) > 49)
       iceb_u_bstab(strf,iceb_u_adrsimv(49,sumac),0,54,1);
      break;     

    case 24:
      iceb_u_bstab(strf,dopol,9,54,1);
      break;     

    case 25:
      if(iceb_u_strlen(dopol) > 45)
        iceb_u_bstab(strf,iceb_u_adrsimv(45,dopol),0,54,1);
      break;     


    case 27:
      iceb_u_bstab(strf,glbuh_str,19,54,1);
      iceb_u_bstab(strf,glbuh_str,76,111,1);
      break;     

    case 29:
      iceb_u_bstab(strf,kassir_str,14,54,1);
      iceb_u_bstab(strf,kassir_str,63,111,1);
      break;     


      
   }
  fprintf(ff,"%s",strf);
  memset(strf,'\0',sizeof(strf));

 }


if(imaf[0] != '\0') 
 {
  fclose(ff);
  iceb_ustpeh(imaf,1,wpredok);
 }
}
