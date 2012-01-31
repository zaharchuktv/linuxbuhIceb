/*$Id: iceb_nalnaks.c,v 1.6 2011-08-29 07:13:46 sasa Exp $*/
/*10.08.2011	12.01.2011	Белых А.И.	iceb_nalnaks.c
Печать шапки счет-фактуры
Если вернули 0 - шапку сформировали
             1 - нет
             
*/
#include <errno.h>
#include "iceb_libbuh.h"

int iceb_nalnaks_v(class iceb_u_str *vid_dog,class iceb_u_str *dat_dog,class iceb_u_str *nom_dog,class iceb_u_str *vid_pokup,class iceb_u_str *vkl_erpn,class iceb_u_str *zal_prod,class iceb_u_str *tip_prih,class iceb_u_str *kopiq,GtkWidget *wpredok);

extern SQL_baza bd;


int iceb_nalnaks(const char *imaf,
int *nomer_st, /*возвращяем количество строк в заголовке*/
short dd,short md,short gd, //дата документа
short dnn,short mnn,short gnn, //Дата счет-фактуры
const char *nomnn, //Номер счет-фактуры
const char *osnov, //Основание
const char *uspr,  //Условие продажи
const char *frop,//Код формы оплаты
const char *kpos,
int tipz,  /*1-приход 2-расход*/
int metka_kop, /*Метка копии 0 - не ставить "Х" 1- копия получателя 2-копия выдавателя*/
FILE **ff,
GtkWidget *wpredok)
{
char stroka[1024];
char strsql[512];
char data_vipiski[512];
SQL_str row;
SQLCURSOR cur;
char            nmo[512],nmo1[512];  /*Наименование организации*/
char            gor[512],gor1[512]; /*Адрес*/
char            inn[32],inn1[32];  /*Индивидуальный налоговый номер*/
char            nspdv[32],nspdv1[32]; /*Номер свидетельства плательщика НДС*/
char            telef[112],telef1[112]; /*Номер телефона*/
char nai_for_opl[512];
class SQLCURSOR cur_alx;
SQL_str row_alx;
int kolstr=0;
short ddog=0,mdog=0,gdog=0;/*дата договора*/
class iceb_u_str nomer_dog("");
class iceb_u_str vid_dog("");
class iceb_u_str data_dog("");

static class iceb_u_str vid_pokup("");
static class iceb_u_str vkl_erpn("");
static class iceb_u_str zal_prod("");
static class iceb_u_str tip_prih("");
static class iceb_u_str kopiq("");

*nomer_st=0;

if(iceb_u_polen(uspr,&vid_dog,1,':') != 0)
 vid_dog.new_plus(uspr);
iceb_u_polen(uspr,&data_dog,2,':');
iceb_u_polen(uspr,&nomer_dog,3,':');


if(iceb_nalnaks_v(&vid_dog,&data_dog,&nomer_dog,&vid_pokup,&vkl_erpn,&zal_prod,&tip_prih,&kopiq,wpredok) != 0)
 return(1);

memset(nai_for_opl,'\0',sizeof(nai_for_opl));


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
if(iceb_u_atof(kpos) != 0. || kpos[0] != '0')
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
  class iceb_u_spisok repl;
//  VVOD SOOB(1);
  int i=0;  
  if(inn1[0] == '\0')
   {
    i++;
    sprintf(strsql,gettext("Для контрагента %s не введен индивидуальный налоговый номер !"),kpos);
//    SOOB.VVOD_SPISOK_add_MD(strsql);
    repl.plus(strsql);
   }
  if(nspdv1[0] == '\0')
   {
    i++;
    sprintf(strsql,gettext("Для контрагента %s не введен номер свидетельства плательщика НДС !"),kpos);
    repl.plus(strsql);
   }
  if(i != 0)
    iceb_menu_soob(&repl,wpredok);
 }

sprintf(strsql,"select str from Alx where fil='nalnaks1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"nalnaks1.alx");
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }


if((*ff = fopen(imaf,"w")) == NULL)
 {
   iceb_er_op_fil(imaf,"",errno,wpredok);
//    error_op_nfil(imaf,errno,"");
    return(1);
 }
fprintf(*ff,"\x1B\x33%c",24);
//fprintf(*ff,"\x1B\x33%c",20); /*Уменьшение межстрочного интервала*/
memset(strsql,'\0',sizeof(strsql));
if(dnn != 0)
 {
  iceb_mesc(mnn,1,strsql);
 // sprintf(data_vipiski,"%d %s %d%s",dnn,strsql,gnn,gettext("г."));
  sprintf(data_vipiski,"%02d%02d%04d",dnn,mnn,gnn);
 }
else
 {
  iceb_mesc(md,1,strsql);
  sprintf(data_vipiski,"%02d%02d%04d",dd,md,gd);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
     continue;

  *nomer_st+=1;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  
  switch(*nomer_st)
   {  
    case 2: /*Выдаётся покупателю*/
      iceb_u_bstab(stroka,vid_pokup.ravno(),35,37,1);
      break;

    case 4: /*Включено до ЄРПН*/
      iceb_u_bstab(stroka,vkl_erpn.ravno(),35,37,1);
      break;

    case 6: /*Остаётся у продавца*/
      iceb_u_bstab(stroka,vkl_erpn.ravno(),35,37,1);
      iceb_u_bstab(stroka,zal_prod.ravno(),35,37,1);
      break;

    case 8: /*Тип причины*/
      iceb_u_bstab(stroka,tip_prih.ravno(),34,37,2);
      break;

    case 10: /*Копия (остаётся у продавца)*/
      iceb_u_bstab(stroka,kopiq.ravno(),35,37,1);
      break;

    case 14: /*Дата выписки - номер счет-фактуры*/
      iceb_u_bstab(stroka,data_vipiski,35,60,2);
      sprintf(strsql,"%*s",iceb_u_kolbait(7,nomnn),nomnn);
      iceb_u_bstab(stroka,strsql,100,130,2);
      break;

    case 17: /*код контрагента платника*/
//      iceb_u_bstab(stroka,kpos,106,126,1);
      break;

    case 19: /*первая строка ниаменования продвца и покупателя*/
      iceb_u_bstab(stroka,nmo,19,67,1);
      iceb_u_bstab(stroka,nmo1,88,138,1);
      break;
      
    case 20:  /*вторая строка ниаменования продвца и покупателя*/
      if(iceb_u_strlen(nmo) > 48)
        iceb_u_bstab(stroka,iceb_u_adrsimv(48,nmo),19,67,1);
      if(iceb_u_strlen(nmo1) > 50)
        iceb_u_bstab(stroka,iceb_u_adrsimv(50,nmo1),88,138,1);
      break;

    case 21:  /*третя строка ниаменования продвца и покупателя*/
      if(iceb_u_strlen(nmo) > 96)
        iceb_u_bstab(stroka,iceb_u_adrsimv(96,nmo),19,67,1);
      if(iceb_u_strlen(nmo1) > 100)
        iceb_u_bstab(stroka,iceb_u_adrsimv(100,nmo1),88,138,1);
      break;
      
    case 24:
      iceb_u_bstab(stroka,inn,6,29,2);
      sprintf(strsql,"%12s",inn1); /*Для печати со смещением вправо*/
      iceb_u_bstab(stroka,strsql,103,126,2);
      break;
      
    case 28:  /*первая строка адреса продавца и покупателя*/
      iceb_u_bstab(stroka,gor,9,9+66,1);
      iceb_u_bstab(stroka,gor1,84,84+56,1);
      break;

    case 29:  /*вторая строка адреса продавца и покупателя*/
      if(iceb_u_strlen(gor) > 66)
        iceb_u_bstab(stroka,iceb_u_adrsimv(66,gor),1,9+66,1);
      if(iceb_u_strlen(gor1) > 56)
        iceb_u_bstab(stroka,iceb_u_adrsimv(55,gor1),77,84+56,1);
      break;

    case 31:
      iceb_u_bstab(stroka,telef,16,16+20,2);
      iceb_u_bstab(stroka,telef1,92,92+20,2);
      break;

    case 34:
      sprintf(strsql,"%*s",iceb_u_kolbait(10,nspdv),nspdv); /*чтобы печатала со смещением вправо*/
      iceb_u_bstab(stroka,strsql,32,32+sizeof(nspdv),2);
      sprintf(strsql,"%*s",iceb_u_kolbait(10,nspdv1),nspdv1); /*чтобы печатала со смещением вправо*/
      iceb_u_bstab(stroka,strsql,106,128,2);
      break;

    case 37:
            
      iceb_u_bstab(stroka,vid_dog.ravno(),32,90,1);
      iceb_u_rsdat(&ddog,&mdog,&gdog,data_dog.ravno(),1);
      if(ddog != 0)
       {
        memset(strsql,'\0',sizeof(strsql));
        sprintf(strsql,"%02d%02d%04d",ddog,mdog,gdog);
        iceb_u_bstab(stroka,strsql,97,114,2);
       }
      sprintf(strsql,"%*s",iceb_u_kolbait(6,nomer_dog.ravno()),nomer_dog.ravno()); /*для печати со смещением вправо*/
      iceb_u_bstab(stroka,strsql,117,150,2);
      break;

    case 39:
      iceb_u_bstab(stroka,nai_for_opl,30,30+sizeof(nai_for_opl),1);

   }
  fprintf(*ff,"%s",stroka);
 }

//fprintf(*ff,"\x1B\x33%c",24);
return(0);

}
