/*$Id: iceb_nalnake.c,v 1.2 2011-02-21 07:36:07 sasa Exp $*/
/*02.02.2011	12.01.2011	Белых А.И.	iceb_nalnake.c
Распечатка концовки налоговой накладной приказ от 21.12.2010г N 969
*/

#include        <errno.h>
#include "iceb_libbuh.h"
extern SQL_baza bd;

void iceb_nalnake(int tipz,short tipnn,
double itogo,
double sumkor,
const char *naimnal,
float procent, //Процент налога на рекламные услуги
float pnds,
const char *imaf_nastr,
const char *kodop,
double okrg1, /*Округление*/
double suma_voz_tar,
FILE *ff,
GtkWidget *wpredok)
{
char stroka[1024];
int nomer_st=0;
class SQLCURSOR cur_alx;
SQL_str row_alx;
int kolstr=0;
char strsql[512];
class iceb_u_str fio("");

sprintf(strsql,"select str from Alx where fil='nalnake.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"nalnake.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }



fprintf(ff,"\x1B\x33%c",20);
char bros[512];
//пропускаем одну строку

double sumanr=0.;
if(procent != 0)
 {
  sumanr=itogo*procent/100.;
  sumanr=iceb_u_okrug(sumanr,okrg1);
 }
sumanr+=suma_voz_tar;

double bb1=0.;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
     continue;
  nomer_st++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka));
  switch (nomer_st)
   {  

    case 2:
      //Всього по розділу І

      sprintf(bros,"%10s",iceb_prcn(itogo));

      if(tipnn == 0)
        iceb_u_bstab(stroka,bros,87,98,1);
      if(tipnn == 1)
       {
        sprintf(bros,"%9s",iceb_prcn(itogo));
        iceb_u_bstab(stroka,bros,98,108,1);
       }
      if(tipnn == 2)
       {
        sprintf(bros,"%9s",iceb_prcn(itogo));
        iceb_u_bstab(stroka,bros,108,118,1);
       }
      if(tipnn == 3)
       {
        sprintf(bros,"%9s",iceb_prcn(itogo));
        iceb_u_bstab(stroka,bros,118,128,1);
       }
      sprintf(bros,"%11s",iceb_prcn(itogo));
      iceb_u_bstab(stroka,bros,128,140,1);
      break;  


    case 4:
      /*Зворотна (заставна) тара*/


      if(sumanr != 0.) //Процент налога на рекламные услуги
       {

        sprintf(bros,"%11s",iceb_prcn(sumanr));
        iceb_u_bstab(stroka,bros,128,140,1);
        if(procent != 0.)
         {
          sprintf(bros,"%-*s",iceb_u_kolbait(50,naimnal),naimnal); //чтобы наверняка забить текущий текст
          iceb_u_bstab(stroka,bros,8,56,1);
         }
       }
      
    
      break;



    case 6: /*Податок на додану вартість*/
      bb1=0.;
      if(tipnn == 0) //20% НДС
       {
        bb1=(itogo+sumkor)*pnds/100.;
        bb1=iceb_u_okrug(bb1,okrg1);
        sprintf(bros,"%10s",iceb_prcn(bb1));
        iceb_u_bstab(stroka,bros,87,98,1);

        sprintf(bros,"%11s",iceb_prcn(bb1));
        iceb_u_bstab(stroka,bros,128,140,1);
       }

      if(tipnn == 3) //Освобождение от НДС
       {
        char punkt_zak[512]; //Пункт закона на основании которого есть освобождение от НДС
        memset(punkt_zak,'\0',sizeof(punkt_zak));
        char naim_nast[512];
        sprintf(naim_nast,"Пункт закона освобождения от НДС для операции %s",kodop);

        iceb_poldan(naim_nast,punkt_zak,imaf_nastr,wpredok);

        if(punkt_zak[0] != '\0')
         {        

          iceb_u_bstab(stroka,punkt_zak,118,127,1);
          fprintf(ff,"%s",stroka);

          for(int ii=9; iceb_u_strlen(punkt_zak) > ii ; ii+=9)
           {
            fprintf(ff,"\
|    |                                                   |       |         |          |          |         |         |%-*.*s|           |\n",
            
            iceb_u_kolbait(9,iceb_u_adrsimv(ii,punkt_zak)),
            iceb_u_kolbait(9,iceb_u_adrsimv(ii,punkt_zak)),
            iceb_u_adrsimv(ii,punkt_zak));
           }
          continue;
         }
       }
      break;
     


    case 8: /*Загальна сума з ПДВ*/
      sprintf(bros,"%10.2f",itogo+sumkor+bb1+sumanr);
      if(tipnn == 0)
        iceb_u_bstab(stroka,bros,87,98,1);
      if(tipnn == 1)
       {
        sprintf(bros,"%9.2f",itogo+sumkor+bb1+sumanr);
        iceb_u_bstab(stroka,bros,98,108,1);
       }
      if(tipnn == 2)
       {
        sprintf(bros,"%9.2f",itogo+sumkor+bb1+sumanr);
        iceb_u_bstab(stroka,bros,108,118,1);
       }
      if(tipnn == 3)
       {
        sprintf(bros,"%9.2f",itogo+sumkor+bb1+sumanr);
        iceb_u_bstab(stroka,bros,118,128,1);
       }
      sprintf(bros,"%11.2f",itogo+sumkor+bb1+sumanr);
      iceb_u_bstab(stroka,bros,128,140,1);
      break;

    case 13: /*Фамилия исполнителя*/
      if(tipz == 1)
       break;
      fio.new_plus("");
      iceb_poldan("Исполнитель",&fio,"matnast.alx",wpredok);
      if(fio.getdlinna() <= 1)
       {
        fio.new_plus(iceb_getfioop(wpredok));
       }

      iceb_u_bstab(stroka,fio.ravno(),60,100,1);
      break;

    case 17: /*пункти налогового кодекса которыми пердусмотрено освобождение от НДС*/
/************************
      if(tipnn == 3) //Освобождение от НДС
       {
        char punkt_zak[512]; //Пункт закона на основании которого есть освобождение от НДС
        memset(punkt_zak,'\0',sizeof(punkt_zak));
        char naim_nast[512];
        sprintf(naim_nast,"Пункт закона освобождения от НДС для операции %s",kodop);

        iceb_poldan(naim_nast,punkt_zak,imaf_nastr,wpredok);

        if(punkt_zak[0] != '\0')
         {        

          iceb_u_bstab(stroka,punkt_zak,3,140,1);
         }      
       } 
*************************/
       break;
   }
 fprintf(ff,"%s",stroka);
}

iceb_podpis(ff,wpredok);

fclose(ff);

}
