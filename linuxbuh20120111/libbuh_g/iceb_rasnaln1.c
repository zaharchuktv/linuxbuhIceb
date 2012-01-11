/*$Id: iceb_rasnaln1.c,v 1.11 2011-02-21 07:36:07 sasa Exp $*/
/*11.09.2010	08.11.2005	Белых А.И.	iceb_rasnaln1.c
Формирование шапки налоговой накладной
*/
#include <stdlib.h>
#include        <errno.h>
#include "iceb_libbuh.h"

extern SQL_baza bd;

void iceb_rasnaln1(const char *imaf,
int *nomer_str,
int *kor,
short dd,short md,short gd, //дата документа
short dnn,short mnn,short gnn, //Дата налоговой накладной
const char *nomnn, //Номер налоговой накладной
const char *osnov, //Основание
const char *uspr,  //Условие продажи
const char *frop,//Код формы оплаты
const char *kpos,
int tipz,
FILE **f1,FILE **f2,
GtkWidget *wpredok)
{
char strsql[1024];
char strsql1[1024];
SQL_str row;
SQLCURSOR cur;
char            nmo[512],nmo1[512];  /*Наименование организации*/
char            gor[512],gor1[512]; /*Адрес*/
char            inn[24],inn1[24];  /*Индивидуальный налоговый номер*/
char            nspdv[24],nspdv1[24]; /*Номер свидетельства плательщика НДС*/
char            telef[40],telef1[40]; /*Номер телефона*/
short		ots; /*Отступ от левого края*/
char nai_for_opl[512];

memset(nai_for_opl,'\0',sizeof(nai_for_opl));


*kor=0;

memset(strsql,'\0',sizeof(strsql)); 
iceb_poldan("Корректор межстрочного растояния",strsql,"matnast.alx",wpredok);
*kor=atoi(strsql);

ots=10;
memset(strsql,'\0',sizeof(strsql)); 
if(iceb_poldan("Отступ налоговой накладной",strsql,"matnast.alx",wpredok) == 0)
    ots=(short)atoi(strsql);

if(frop[0] != '\0')
 {
  /*Читаем форму оплаты*/
  sprintf(strsql,"select naik from Foroplat \
where kod='%s'",frop);

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"Не знайдена форма оплаты %s !",frop);  
    iceb_menu_soob(strsql,wpredok);
   }
  else
    strncpy(nai_for_opl,row[0],sizeof(nai_for_opl)-1);  
 }

/*Читаем реквизиты организации свои */

memset(nmo,'\0',sizeof(nmo));
memset(gor,'\0',sizeof(gor));
memset(inn,'\0',sizeof(inn));
memset(telef,'\0',sizeof(telef));
memset(nspdv,'\0',sizeof(nspdv));

memset(nmo1,'\0',sizeof(nmo1));
memset(gor1,'\0',sizeof(gor1));
memset(inn1,'\0',sizeof(inn1));
memset(nspdv1,'\0',sizeof(nspdv1));
memset(telef1,'\0',sizeof(telef1));

sprintf(strsql,"select * from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),"00");
  iceb_menu_soob(strsql,wpredok);
 }
else
 {
  if(tipz == 2)
   {
    if(row[16][0] == '\0')
      strncpy(nmo,row[1],sizeof(nmo)-1);
    else
      strncpy(nmo,row[16],sizeof(nmo)-1);

    strncpy(gor,row[3],sizeof(gor)-1);
    strncpy(inn,row[8],sizeof(inn)-1);
    strncpy(nspdv,row[9],sizeof(nspdv)-1);
    strncpy(telef,row[10],sizeof(telef)-1);
   }
  if(tipz == 1)
   {
    if(row[16][0] == '\0')
      strncpy(nmo1,row[1],sizeof(nmo1)-1);
    else
      strncpy(nmo1,row[16],sizeof(nmo1)-1);

    strncpy(gor1,row[3],sizeof(gor1)-1);
    strncpy(inn1,row[8],sizeof(inn1)-1);
    strncpy(nspdv1,row[9],sizeof(nspdv1)-1);
    strncpy(telef1,row[10],sizeof(telef1)-1);
   }
 } 


/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/
if(atoi(kpos) != 0. || kpos[0] != '0')
 {
  sprintf(strsql,"select * from Kontragent where kodkon='%s'",kpos);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kpos);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    if(tipz == 2)
     {
      if(row[16][0] == '\0')
        strncpy(nmo1,row[1],sizeof(nmo1)-1);
      else
        strncpy(nmo1,row[16],sizeof(nmo1)-1);

      strncpy(gor1,row[3],sizeof(gor1)-1);
      strncpy(inn1,row[8],sizeof(inn1)-1);
      strncpy(nspdv1,row[9],sizeof(nspdv1)-1);
      strncpy(telef1,row[10],sizeof(telef1)-1);
     }
    if(tipz == 1)
     {
      if(row[16][0] == '\0')
        strncpy(nmo,row[1],sizeof(nmo)-1);
      else
        strncpy(nmo,row[16],sizeof(nmo)-1);

      strncpy(gor,row[3],sizeof(gor)-1);
      strncpy(inn,row[8],sizeof(inn)-1);
      strncpy(nspdv,row[9],sizeof(nspdv)-1);
      strncpy(telef,row[10],sizeof(telef)-1);
     }
   }
  iceb_u_str repl;
  int i=0;  
  if(inn1[0] == '\0')
   {
    i++;
    sprintf(strsql,gettext("Для контрагента %s не введен индивидуальный налоговый номер !"),kpos);
    repl.plus(strsql);
   }
  if(nspdv1[0] == '\0')
   {
    i++;
    sprintf(strsql,gettext("Для контрагента %s не введен номер свидетельства плательщика НДС !"),kpos);
    if(i == 1)
     repl.plus(strsql);
    else
     repl.ps_plus(strsql);
   }
  if(i != 0)
   iceb_menu_soob(&repl,wpredok);
 }

iceb_u_str imaf_for("nakl.alx");
if(iceb_alxout(imaf_for.ravno(),wpredok) != 0)
 return;
if((*f2 = fopen(imaf_for.ravno(),"r")) == NULL)
 {
  if(errno != ENOENT)
   {
    iceb_er_op_fil(imaf_for.ravno(),"",errno,wpredok);
    return;
   }

  if(errno == ENOENT)
   {
    iceb_menu_soob("Не найден файл заготовки накладной nakl.alx !",wpredok);
    return;
   }
 }
if((*f1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
iceb_u_startfil(*f1);
fprintf(*f1,"\x1b\x6C%c",ots); /*Установка левого поля*/
fprintf(*f1,"\x1B\x33%c",20-*kor); /*Уменьшение межстрочного интервала*/
fprintf(*f1,"\x1B\x4D"); /*12 знаков на дюйм*/
fprintf(*f1,"\x0F");     /*Ужатый режим*/
memset(strsql,'\0',sizeof(strsql));
if(dnn != 0)
 {
  iceb_mesc(mnn,1,strsql);
  sprintf(strsql1,"%d %s %d%s",dnn,strsql,gnn,gettext("г."));
 }
else
 {
  iceb_mesc(md,1,strsql);
  sprintf(strsql1,"%d %s %d%s",dd,strsql,gd,gettext("г."));
 }
char str[1024];

for(*nomer_str=1; *nomer_str <= 47; *nomer_str+=1)
 {
  memset(str,'\0',sizeof(str));
  fgets(str,sizeof(str),*f2);
  if(*nomer_str == 11)
   {
    iceb_u_bstab(str,strsql1,34,60,1);
    iceb_u_bstab(str,nomnn,73,83,1);
   }

  if(*nomer_str == 12)
   iceb_u_bstab(str,kpos,106,126,1);

  if(*nomer_str == 14)
   {
    iceb_u_bstab(str,nmo,20,60,1);
    iceb_u_bstab(str,nmo1,95,135,1);
   }
  if(*nomer_str == 15) //Вторая строка длинного наименования контрагента
   {
    if(iceb_u_strlen(nmo) > 40)
      iceb_u_bstab(str,iceb_u_adrsimv(40,nmo),20,60,1);
    if(iceb_u_strlen(nmo1))
      iceb_u_bstab(str,iceb_u_adrsimv(40,nmo1),95,135,1);
   }
  if(*nomer_str == 19)
   {
    iceb_u_bstab(str,inn,6,29,2);
    iceb_u_bstab(str,inn1,103,126,2);
   }
  if(*nomer_str == 23)
   {
    iceb_u_bstab(str,gor,13,13+50,1);
    iceb_u_bstab(str,gor1,87,87+50,1);
   }
  if(*nomer_str == 24)
   {
    if(iceb_u_strlen(gor) > 50)
      iceb_u_bstab(str,iceb_u_adrsimv(50,gor),13,50,1);
    if(iceb_u_strlen(gor1) > 50)
      iceb_u_bstab(str,iceb_u_adrsimv(50,gor1),87,87+50,1);
   }
  if(*nomer_str == 25)
   {
    iceb_u_bstab(str,telef,15,15+sizeof(telef),1);
    iceb_u_bstab(str,telef1,91,91+sizeof(telef1),1);
   }
  if(*nomer_str == 28)
   {
    iceb_u_bstab(str,nspdv,32,32+sizeof(nspdv),1);
    iceb_u_bstab(str,nspdv1,108,108+sizeof(nspdv1),1);
   }
  if(*nomer_str == 30)
   {
    memset(strsql,'\0',sizeof(strsql));
    sprintf(strsql,"%s %s",osnov,uspr);
    iceb_u_bstab(str,strsql,20,30+iceb_u_strlen(strsql),1);
   }
  if(*nomer_str == 33)
   {
    iceb_u_bstab(str,nai_for_opl,30,30+sizeof(nai_for_opl),1);
   }
  fprintf(*f1,"%s",str);
 }

fprintf(*f1,"\x1B\x33%c",24-*kor);

}
