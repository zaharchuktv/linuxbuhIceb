/*$Id: iceb_rasnaln2.c,v 1.8 2011-02-21 07:36:07 sasa Exp $*/
/*16.10.2010	08.11.2005	Белых А.И.	iceb_rasnaln2.c
Формирование концовки счет-фактуры
*/
#include        <errno.h>
#include "iceb_libbuh.h"
#include <unistd.h>


void iceb_rasnaln2(int nomer_str,
int kor,
short tipnn,
double itogo,
double sumkor,
const char *naimnal,
float procent, //Процент налога на рекламные услуги
float pnds,
const char *imaf_nastr,
const char *kodop,
double okrg1, /*Округление*/
double suma_voz_tar,
FILE *f1,FILE *f2,
GtkWidget *wpredok)
{

fprintf(f1,"\x1B\x33%c",20-kor);
char bros[512];
char str[1024];
//пропускаем одну строку

fgets(str,sizeof(str),f2);
//nomer_str++;
double sumanr=0.;
if(procent != 0)
 {
  sumanr=itogo*procent/100.;
  sumanr=iceb_u_okrug(sumanr,okrg1);
 }
sumanr+=suma_voz_tar;

double bb3=0.;
double bb1=0.;
while(fgets(str,sizeof(str),f2) != NULL)
 {
  nomer_str++;

  if(nomer_str == 50)
   { 
  //Всього по розділу І

    sprintf(bros,"%9s",iceb_prcn(itogo));

    if(tipnn == 0)
      iceb_u_bstab(str,bros,87,96,1);
    if(tipnn == 1)
      iceb_u_bstab(str,bros,97,107,1);
    if(tipnn == 2)
      iceb_u_bstab(str,bros,107,117,1);
    if(tipnn == 3)
      iceb_u_bstab(str,bros,117,127,1);

    iceb_u_bstab(str,bros,127,140,1);
   }
  
  //Товарно-транспортні витрати
  if(nomer_str == 52)
   { 

    bb3=0.;

    if(bb3 != 0.)
     {
      memset(str,'\0',sizeof(str));
      fgets(str,sizeof(str),f2);
      sprintf(bros,"%9s",iceb_prcn(bb3));
      if(tipnn == 0)
        iceb_u_bstab(str,bros,87,96,1);
      if(tipnn == 1)
        iceb_u_bstab(str,bros,97,107,1);
      if(tipnn == 2)
        iceb_u_bstab(str,bros,107,117,1);
      if(tipnn == 3)
        iceb_u_bstab(str,bros,117,127,1);

      iceb_u_bstab(str,bros,127,140,1);
     }
   }

  if(nomer_str == 54)
   {
    //Зворотна (заставна) тара


    if(sumanr != 0.) //Процент налога на рекламные услуги
     {

      sprintf(bros,"%9s",iceb_prcn(sumanr));
      iceb_u_bstab(str,bros,127,140,1);
      if(procent != 0.)
       {
        sprintf(bros,"%-50s",naimnal); //чтобы наверняка забить текущий текст
        iceb_u_bstab(str,bros,8,56,1);
       }
     }
    
    
   }

  if(nomer_str == 56)
   {
    /*Надбавка*/
    if(sumkor > 0)
     {
      sprintf(bros,"%9.2f",sumkor);
      if(tipnn == 0)
        iceb_u_bstab(str,bros,87,96,1);
      if(tipnn == 1)
        iceb_u_bstab(str,bros,97,107,1);
      if(tipnn == 2)
        iceb_u_bstab(str,bros,107,117,1);
      if(tipnn == 3)
        iceb_u_bstab(str,bros,117,127,1);

      iceb_u_bstab(str,bros,127,140,1);
      
     }
   }

  if(nomer_str == 58)
   {
    /*Снижка*/
    if(sumkor < 0)
     {
      sprintf(bros,"%9.2f",sumkor);
      if(tipnn == 0)
        iceb_u_bstab(str,bros,87,96,1);
      if(tipnn == 1)
        iceb_u_bstab(str,bros,97,107,1);
      if(tipnn == 2)
        iceb_u_bstab(str,bros,107,117,1);
      if(tipnn == 3)
        iceb_u_bstab(str,bros,117,127,1);

      iceb_u_bstab(str,bros,127,140,1);
      
     }
   }
  if(nomer_str == 60)
   {

    sprintf(bros,"%9s",iceb_prcn(bb3+itogo+sumkor));

    if(tipnn == 0)
        iceb_u_bstab(str,bros,87,96,1);
    if(tipnn == 1)
        iceb_u_bstab(str,bros,97,107,1);
    if(tipnn == 2)
        iceb_u_bstab(str,bros,107,117,1);
    if(tipnn == 3)
        iceb_u_bstab(str,bros,117,127,1);
    iceb_u_bstab(str,bros,127,140,1);

    
   }
  if(nomer_str == 62)
   {

    bb1=0.;
    if(tipnn == 0) //20% НДС
     {
      bb1=(bb3+itogo+sumkor)*pnds/100.;
      bb1=iceb_u_okrug(bb1,okrg1);
      sprintf(bros,"%9s",iceb_prcn(bb1));
      iceb_u_bstab(str,bros,87,96,1);

      iceb_u_bstab(str,bros,127,140,1);
     }

    if(tipnn == 3) //Освобождение от НДС
     {
      char punkt_zak[100]; //Пункт закона на основании которого есть освобождение от НДС
      memset(punkt_zak,'\0',sizeof(punkt_zak));
      //Смотрим пунк закона освобождения от НДС
      char naim_nast[500];
      sprintf(naim_nast,"Пункт закона освобождения от НДС для операции %s",kodop);

      iceb_poldan(naim_nast,punkt_zak,imaf_nastr,wpredok);
      if(punkt_zak[0] != '\0')
       {        

        iceb_u_bstab(str,punkt_zak,117,126,1);
        fprintf(f1,"%s",str);

        for(int ii=9; iceb_u_strlen(punkt_zak) > ii ; ii+=9)
         {
          fprintf(f1,"\
|    |                                                   |       |         |          |         |         |         |%-*.*s|           |\n",
          iceb_u_kolbait(9,iceb_u_adrsimv(ii,punkt_zak)),iceb_u_kolbait(9,iceb_u_adrsimv(ii,punkt_zak)),iceb_u_adrsimv(ii,punkt_zak));
         }
        continue;
       }
     }
     
   }

  if(nomer_str == 64)
   {

    sprintf(bros,"%9.2f",bb3+itogo+sumkor+bb1+sumanr);
    if(tipnn == 0)
      iceb_u_bstab(str,bros,87,96,1);
    if(tipnn == 1)
      iceb_u_bstab(str,bros,97,106,1);
    if(tipnn == 2)
      iceb_u_bstab(str,bros,107,117,1);
    if(tipnn == 3)
      iceb_u_bstab(str,bros,117,207,1);
    iceb_u_bstab(str,bros,127,136,1);
   }

  if(nomer_str == 72)
   {
    class iceb_u_str fio("");
    iceb_poldan("Исполнитель",bros,"matnast.alx",wpredok);
    if(fio.getdlinna() <= 1)
      fio.new_plus(iceb_getfioop(NULL));

    iceb_u_bstab(str,fio.ravno(),60,100,1);
   }
 fprintf(f1,"%s",str);
}

iceb_podpis(f1,wpredok);
fprintf(f1,"\x1b\x6C%c",1); /*Установка левого поля*/
fprintf(f1,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
fprintf(f1,"\x1B\x50"); /*10 знаков на дюйм*/
fprintf(f1,"\x12"); /*Нормальный режим*/

fclose(f1);
fclose(f2);
unlink("nakl.alx");
}
