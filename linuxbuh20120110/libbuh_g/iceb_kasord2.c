/*$Id: iceb_kasord2.c,v 1.23 2011-02-21 07:36:07 sasa Exp $*/
/*10.12.2010	04.10.2000	Белых А.И.	iceb_kasord2.c
Распечатка расходного кассового ордера
*/
#include <stdlib.h>
#include        <errno.h>
#include <unistd.h>
#include	"iceb_libbuh.h"

extern SQL_baza	bd;

void iceb_kasord2_0(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,
double suma,const char *fio,const char *osnov,const char *dopol,const char *dokum,const char *hcn,FILE *ff,GtkWidget *wpredok);

void iceb_kasord2_1(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,
double suma,const char *fio,const char *osnov,const char *dopol,const char *dokum,const char *hcn,FILE *ff_f,GtkWidget *wpredok);


void iceb_kasord2(const char *imaf, //Имя файла
const char *nomd, //Номер документа
short dd,short md,short gd, //Дата
const char *shet, //Счет кореспондент
const char *shetkas, //Счет кассы ,Шифр аналитического учета
double suma, //Сумма
const char *fio, //Фамилия имя отчество
const char *osnov, //Основание
const char *dopol, //Дополнение
const char *dokum, //Реквизита документа по которому выданы деньги
const char *hcn, //Шифр целевого назначения
FILE *ff,
GtkWidget *wpredok)
{
if(iceb_u_sravmydat(16,6,2009,dd,md,gd) > 0)
  iceb_kasord2_0(imaf,nomd,dd,md,gd,shet,shetkas,suma,fio,osnov,dopol,dokum,hcn,ff,wpredok);
else
  iceb_kasord2_1(imaf,nomd,dd,md,gd,shet,shetkas,suma,fio,osnov,dopol,dokum,hcn,ff,wpredok);

}
/***********************************************/
/************************************************/
void	iceb_kasord2_0(const char *imaf, //Имя файла
const char *nomd, //Номер документа
short dd,short md,short gd, //Дата
const char *shet, //Счет кореспондент
const char *shetkas, //Счет кассы ,Шифр аналитического учета
double suma, //Сумма
const char *fio, //Фамилия имя отчество
const char *osnov, //Основание
const char *dopol, //Дополнение
const char *dokum, //Реквизита документа по которому выданы деньги
const char *hcn, //Шифр целевого назначения
FILE *ff,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
SQL_str		row;
SQLCURSOR       cur;
char		naim[512];
char		strf[1024];
short		kols;
char		sumac[200];
/*printf("iceb_kasord2_0\n");*/

memset(naim,'\0',sizeof(naim));
memset(sumac,'\0',sizeof(sumac));
iceb_u_preobr(suma,sumac,0);

/*Читаем реквизиты организации*/

sprintf(strsql,"select naikon from Kontragent where kodkon='00'");

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  strncpy(naim,row[0],sizeof(naim)-1);
 }

if(imaf[0] != '\0')
 {
  if((ff = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return;
   }
 }


sprintf(strsql,"select str from Alx where fil='kasorder2.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob("Не найдены настройки kasorder2.alx",wpredok);
  return;
 }

short metka_sum_prop=0;
if(iceb_poldan("Печатать сумму прописью в расходном документе",strf,"kasnast.alx",wpredok) == 0)
 if(iceb_u_SRAV("Вкл",strf,1) == 0)
  metka_sum_prop=1;


kols=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  strncpy(strf,row_alx[0],sizeof(strf)-1);
  kols++;
  switch (kols)
   {
    case 1:
      iceb_u_bstab(strf,naim,0,50,1);
      break;     

    case 3:
      iceb_u_bstab(strf,nomd,43,60,1);
      break;     

    case 7:
      if(nomd[0] != '\0')
       {
        sprintf(strsql,"%02d",dd);      
        iceb_u_bstab(strf,strsql,19,21,1);
        sprintf(strsql,"%02d",md);      
        iceb_u_bstab(strf,strsql,25,28,1);
       }

      sprintf(strsql,"%02d%s",gd,gettext("г."));      
      iceb_u_bstab(strf,strsql,32,42,1);

      break;     

    case 13:
      iceb_u_bstab(strf,shet,9,19,1);
      iceb_u_bstab(strf,shetkas,22,38,1);
      sprintf(strsql,"%15.2f",suma);
      iceb_u_bstab(strf,strsql,39,55,1);
      iceb_u_bstab(strf,hcn,56,69,1);
      break;     

    case 15:
      iceb_u_bstab(strf,fio,7,69,1);
      break;


    case 16:
      iceb_u_bstab(strf,osnov,9,70,1);
      break;     

    case 17:
      if(iceb_u_strlen(osnov) > 61)
         iceb_u_bstab(strf,iceb_u_adrsimv(61,osnov),0,70,1);
      break;     

    case 18:
      iceb_u_bstab(strf,sumac,0,69,1);
      break;     


    case 19:
      iceb_u_bstab(strf,dopol,9,69,1);
      break;     

    case 23:
      if(metka_sum_prop == 1)
        iceb_u_bstab(strf,sumac,8,69,1);
      break;     

    case 25:
      sprintf(strsql,"%02d%s",gd,gettext("г."));      
      iceb_u_bstab(strf,strsql,21,32,1);
      break;     

    case 27:
      iceb_u_bstab(strf,dokum,4,69,1);
      if(iceb_u_strlen(dokum) > 69-4)
       {
        fprintf(ff,"%s",strf);
        memset(strf,'\0',sizeof(strf));
        sprintf(strf,"%s\n",iceb_u_adrsimv(69-4,dokum));
       }
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
/******************************/
/*******************************/
void iceb_kasord2_1(const char *imaf, //Имя файла
const char *nomd, //Номер документа
short dd,short md,short gd, //Дата
const char *shet, //Счет кореспондент
const char *shetkas, //Счет кассы ,Шифр аналитического учета
double suma, //Сумма
const char *fio, //Фамилия имя отчество
const char *osnov, //Основание
const char *dopol, //Дополнение
const char *dokum, //Реквизита документа по которому выданы деньги
const char *hcn, //Шифр целевого назначения
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
char		strf[312];
short		kols;
char		sumac[200];
class iceb_fioruk_rk rukov;
class iceb_fioruk_rk glbuh;
class iceb_u_str kassir("");
char naim_mes[56];
FILE *ff=ff_f;
char rukov_str[512];
char glbuh_str[512];
char kassir_str[512];

memset(naim_mes,'\0',sizeof(naim_mes));
memset(kodedrp,'\0',sizeof(kodedrp));
memset(naim,'\0',sizeof(naim));
memset(sumac,'\0',sizeof(sumac));
iceb_u_preobr(suma,sumac,0);

iceb_fioruk(1,&rukov,wpredok); 
iceb_fioruk(2,&glbuh,wpredok); /*узнаём фамилию главного бухгалтера*/

/*записываем со смещением в право*/
memset(glbuh_str,'\0',sizeof(glbuh_str));
sprintf(glbuh_str,"%*.*s",
iceb_u_kolbait(23,glbuh.famil_inic.ravno()),
iceb_u_kolbait(23,glbuh.famil_inic.ravno()),
glbuh.famil_inic.ravno());
for(int nom=0; nom < (int)strlen(glbuh_str); nom++)
 if(glbuh_str[nom] == ' ')
  glbuh_str[nom]='_';

/*записываем со смещением в право*/
memset(rukov_str,'\0',sizeof(rukov_str));
sprintf(rukov_str,"%*.*s",
iceb_u_kolbait(27,rukov.famil_inic.ravno()),
iceb_u_kolbait(27,rukov.famil_inic.ravno()),
rukov.famil_inic.ravno());
for(int nom=0; nom < (int)strlen(rukov_str); nom++)
 if(rukov_str[nom] == ' ')
  rukov_str[nom]='_';

/*Чтобы узнать фамилию кассира нужно узнать номер кассы*/
memset(kassir_str,'\0',sizeof(kassir_str));
sprintf(strsql,"select kassa from Kasord where nomd='%s' and datd='%04d-%02d-%02d' and tp=2",
nomd,gd,md,dd);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  sprintf(strsql,"select fio from Kas where kod=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   sprintf(kassir_str,"%*.*s",iceb_u_kolbait(38,row[0]),iceb_u_kolbait(38,row[0]),row[0]); /*чтобы получить со смещением вправо*/
 }
for(int nom=0; nom < (int)strlen(kassir_str); nom++)
 if(kassir_str[nom] == ' ')
  kassir_str[nom]='_';

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
 
sprintf(strsql,"select str from Alx where fil='kasord2.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob("Не найдены настройки kasord2.alx",wpredok);
  return;
 }


short metka_sum_prop=0;
if(iceb_poldan("Печатать сумму прописью в расходном документе",strf,"kasnast.alx",wpredok) == 0)
 if(iceb_u_SRAV("Вкл",strf,1) == 0)
  metka_sum_prop=1;

iceb_mesc(md,1,naim_mes);

kols=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  strncpy(strf,row_alx[0],sizeof(strf)-1);
  kols++;
  switch (kols)
   {
    case 6:
      iceb_u_bstab(strf,naim,0,54,1);
      iceb_u_bstab(strf,kodedrp,55,74,1);
      break;     

    case 8:
      if(nomd[0] != '\0')
       {
        sprintf(strsql,"%02d",dd);
        iceb_u_bstab(strf,strsql,26,28,1);

        sprintf(strsql,"%s %d р.",naim_mes,gd);
        iceb_u_bstab(strf,strsql,30,80,1);
       }
      break;     

    case 16:
      iceb_u_bstab(strf,nomd,1,11,1);

      if(nomd[0] != '\0')
       {
        sprintf(strsql,"%02d.%02d.%d",dd,md,gd);
        iceb_u_bstab(strf,strsql,11,21,1);
       }
      iceb_u_bstab(strf,shet,24,38,1);
      iceb_u_bstab(strf,shetkas,39,49,1);
      sprintf(strsql,"%.2f",suma);
      iceb_u_bstab(strf,strsql,52,65,1);
      if(atoi(hcn) != 0)
        iceb_u_bstab(strf,hcn,65,76,1);
      break;     

    case 18:
      iceb_u_bstab(strf,fio,7,80,1);
      break;     

    case 19:
      iceb_u_bstab(strf,osnov,9,80,1);
      break;     

    case 20:
      if(iceb_u_strlen(osnov) > 71)
       iceb_u_bstab(strf,iceb_u_adrsimv(71,osnov),0,80,1);
      break;     

    case 21:
       iceb_u_bstab(strf,sumac,5,80,1);
       break;     

    case 22:
       iceb_u_bstab(strf,dopol,9,80,1);
       break;     

    case 23:
       if(iceb_u_strlen(dopol) > 71)
        iceb_u_bstab(strf,iceb_u_adrsimv(71,dopol),0,80,1);
       break;     

    case 25:
       iceb_u_bstab(strf,rukov_str,9,36,1);
       iceb_u_bstab(strf,glbuh_str,55,79,1);
       break;     

    case 27:
       if(metka_sum_prop == 0)
        break;
       iceb_u_bstab(strf,sumac,9,80,1);

       break;     

    case 29:
       iceb_u_bstab(strf,gd,26,30,1);
       break;     

    case 31:
       iceb_u_bstab(strf,dokum,3,80,1);
       break;     

    case 32:
       if(iceb_u_strlen(dokum) > 77)
         iceb_u_bstab(strf,iceb_u_adrsimv(77,dokum),0,80,1);
       break;     
    case 34:
       iceb_u_bstab(strf,kassir_str,12,50,1);
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
