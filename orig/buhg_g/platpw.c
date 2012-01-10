/*$Id: platpw.c,v 1.17 2011-02-21 07:35:55 sasa Exp $*/
/*14.12.2010	29.09.2001	Белых А.И.	platpw.c
Печать платежного поручения в соответствии с постановлением
правления национального банка Украины от 29 марта 2001г. N135
Зарегистрировано в министерстве юстиции Украины 25 апреля 2001г
за N368/5559
Официальный весник Украины N18 часть 2 за 2001 год страница 848
В разделе 2 пункт 6 оговорена нумерация платежек

Если вернули 0 документ сформирован
             1 нет
*/
#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern short	kolstrdok; //Количество строк между документами
extern class REC rec;
extern short	ots; /*Отступ от левого края*/
extern short	kor; /*Корректор межстрочного растояния*/
extern short    rzk;       /*0-преобразование числа с тире 1-со вставкой запятой*/
extern short    mvs;       /*1-с разбивкой суммы по счетам 0-без*/
extern SQL_baza bd;

int platpw(char *imaf1,
short kp, //Количество копий
short nomk, //Номер печатаемой копии
FILE *ff,
int viddok, //0-Платежное поручение 1- платежное требование
GtkWidget *wpredok)
{
char bank_plat[512];
char bank_poluh[512];
char		stroka[1024];
char		stroka1[512];
char		stroka2[512];
char		imaf[512];
short		kolstrok=0;
char		bros[512];
char dt2[32]; //Дата платежа
double		bb=0.;
short dt,mt,gt;
int kol_ps=0;

iceb_u_poltekdat(&dt,&mt,&gt);

char mescc[32];
memset(mescc,'\0',sizeof(mescc));
iceb_mesc(rec.md,1,mescc);
sprintf(dt2,"%02d %s %d%s",rec.dd,mescc,rec.gd,gettext("г."));

memset(bank_plat,'\0',sizeof(bank_plat));
memset(bank_poluh,'\0',sizeof(bank_poluh));


if(iceb_u_sravmydat(3,7,2009,rec.ddi,rec.mdi,rec.gdi) <= 0)
 {
  if(viddok == 0)
    sprintf(imaf,"pp03072009.alx");
  if(viddok == 1)
    sprintf(imaf,"pt03072009.alx");
 }
else
 {
  if(viddok == 0)
    sprintf(imaf,"platp.alx");
  if(viddok == 1)
    sprintf(imaf,"platt.alx");
 }

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }


fprintf(ff,"\x12"); //Выключить уплотненный режим
fprintf(ff,"\x1b\x6C%c",ots); /*Установка левого поля*/
fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
fprintf(ff,"\x1B\x78%c",1); //Включить черновой режим печати

if(iceb_u_sravmydat(3,7,2009,rec.ddi,rec.mdi,rec.gdi) > 0)
 if(kp > 1)
  fprintf(ff,"%70s N%d\n",gettext("Экземпляр"),nomk+1);

//fprintf(ff,"\x1B\x55\x31"); /*Включение однонаправленного режима*/
fprintf(ff,"\x1B\x33%c",20); /*Уменьшаем межстрочный интервал n/216 дюйма*/
//fprintf(ff,"\x1B\x41%c",7-kor); /*Уменьшаем межстрочный интервал n/72 дюйма*/
fprintf(ff,"\x1b\x45"); //Режим выделенного шрифта
kol_ps=iceb_u_pole2(rec.naz_plat.ravno(),'\n');
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  kolstrok++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch(kolstrok)
   {

    case 2: //Номер платежного поручения
     fprintf(ff,"\x1B\x33%c",16-kor); /*Уменьшаем межстрочный интервал n/216 дюйма*/
//     fprintf(ff,"\x1B\x41%c",5-kor); /*Уменьшаем межстрочный интервал n/72 дюйма*/
     iceb_u_bstab(stroka,rec.nomdk.ravno(),30,50,1);
     break;

    case 3: 
      fprintf(ff,"\x1B\x33%c",30-kor); /*Уменьшаем межстрочный интервал n/216 дюйма*/
//      fprintf(ff,"\x1B\x41%c",10-kor); /*Уменьшаем межстрочный интервал n/72 дюйма*/
      break;

    case 4: //Дата документа
     iceb_u_bstab(stroka,dt2,16,45,1);
     break;

    case 6: //Год
     sprintf(bros,"%d",gt);
     iceb_u_bstab(stroka,bros,64,69,1);
     break;

    case 7: //Плательщик
     if(viddok == 0)
       iceb_u_bstab(stroka,rec.naior.ravno(),8,51,1);
     if(viddok == 1)
       iceb_u_bstab(stroka,rec.naior1.ravno(),8,51,1);

     break;

    case 8: //Наименование плательщика, которое не поместилось в первой строке
     
     if(viddok == 0)
      if(strlen(rec.naior.ravno()) > 43)
       iceb_u_bstab(stroka,&rec.naior.ravno()[43],1,70,1);
     if(viddok == 1)
      if(strlen(rec.naior1.ravno()) > 43)
       iceb_u_bstab(stroka,&rec.naior1.ravno()[43],1,70,1);

/***************** дата валютирования теперь не печатаестся
     if(rec.dv != 0)
      {
       sprintf(bros,"%02d",rec.dv);
       iceb_u_bstab(stroka,bros,53,56,1);

       sprintf(bros,"%02d",rec.mv);
       iceb_u_bstab(stroka,bros,59,62,1);

       sprintf(bros,"%d",rec.gv);
       iceb_u_bstab(stroka,bros,64,69,1);
      }
     else
      {
       sprintf(bros,"%d",gt);
       iceb_u_bstab(stroka,bros,64,69,1);
      }    
********************/
     break;

    case 10: //Код СДЗПОУ
     if(viddok == 0)
       iceb_u_bstab(stroka,rec.kod.ravno(),11,26,1);
     if(viddok == 1)
       iceb_u_bstab(stroka,rec.kod1.ravno(),11,26,1);
     break;

    case 12: //Банк плательщика
      if(viddok == 0)
       sprintf(bank_plat,"%s в м.%s",rec.naiban.ravno(),rec.gorod.ravno());
      if(viddok == 1)
       sprintf(bank_plat,"%s в м.%s",rec.naiban1.ravno(),rec.gorod1.ravno());
      iceb_u_bstab(stroka,bank_plat,14,32,1);
     break;

    case 13: //Банк плательщика, код банка, счет дебета, сумма платежа
     if(strlen(bank_plat) > 18)
        iceb_u_bstab(stroka,&bank_plat[18],0,32,1);
     if(viddok == 0)
      {
/*********
       sprintf(bros,"%s в м.%s",rec.naiban.ravno(),rec.gorod.ravno());
       iceb_u_bstab(stroka,bros,0,32,1);
**********/
       iceb_u_bstab(stroka,rec.mfo.ravno(),37,46,1);
       iceb_u_bstab(stroka,rec.nsh.ravno(),57,74,1);
      }
     if(viddok == 1)
      {
/*********
       sprintf(bros,"%s в м.%s",rec.naiban1.ravno(),rec.gorod1.ravno());
       iceb_u_bstab(stroka,bros,0,32,1);
*************/
       iceb_u_bstab(stroka,rec.mfo1.ravno(),37,46,1);
       iceb_u_bstab(stroka,rec.nsh1.ravno(),57,74,1);
      }
     bb=rec.sumd+rec.uslb;

     if(rzk == 0)
        iceb_u_prnb(bb,bros);
     if(rzk == 1)
        iceb_u_prnb1(bb,bros);

     iceb_u_bstab(stroka,bros,79,88,1);

     break;

    case 15: //Получатель
     if(viddok == 0)
      iceb_u_bstab(stroka,rec.naior1.ravno(),10,62,1);
     if(viddok == 1)
      iceb_u_bstab(stroka,rec.naior.ravno(),10,62,1);
     break;

    case 17: //Код, номер счета за услуги, сумма за услуги
     iceb_u_bstab(stroka,rec.kod1.ravno(),11,26,1);
     break;

    case 18: //Кредит счет N
      bros[0]='\0';
      if(rec.uslb != 0. && mvs == 1)
       {
        bb=rec.sumd;
        if(rzk == 0)
         iceb_u_prnb(bb,bros);

        if(rzk == 1)
         iceb_u_prnb1(bb,bros);

       }
     if(viddok == 0)
        iceb_u_bstab(stroka,rec.nsh1.ravno(),53,70,1);
     if(viddok == 1)
        iceb_u_bstab(stroka,rec.nsh.ravno(),53,70,1);
      iceb_u_bstab(stroka,bros,75,84,1);
      break;

    case 19: /*Банк получателя*/
      if(viddok == 0)
       sprintf(bank_poluh,"%s в м.%s",rec.naiban1.ravno(),rec.gorod1.ravno());
      if(viddok == 1)
        sprintf(bank_poluh,"%s в м.%s",rec.naiban.ravno(),rec.gorod.ravno());
      iceb_u_bstab(stroka,bank_poluh,16,32,1);
      break;

    case 20: //Банк получателя, МФО, счет
      if(strlen(bank_poluh) > 16)
        iceb_u_bstab(stroka,&bank_poluh[16],0,32,1);
      if(viddok == 0)
       {
/*********
        sprintf(bros,"%s в м.%s",rec.naiban1.ravno(),rec.gorod1.ravno());
        iceb_u_bstab(stroka,bros,0,32,1);
*************/
        iceb_u_bstab(stroka,rec.mfo1.ravno(),37,46,1);
       }
      if(viddok == 1)
       {
/************
        sprintf(bros,"%s в м.%s",rec.naiban.ravno(),rec.gorod.ravno());
        iceb_u_bstab(stroka,bros,0,32,1);
***************/
        iceb_u_bstab(stroka,rec.mfo.ravno(),37,46,1);
       }
      iceb_u_bstab(stroka,rec.shbzu.ravno(),57,74,1);
      bros[0]='\0';
      if(rec.uslb != 0. && mvs == 1)
       {
        if(rzk == 0)
         iceb_u_prnb(rec.uslb,bros);

        if(rzk == 1)
         iceb_u_prnb1(rec.uslb,bros);

       }

      iceb_u_bstab(stroka,bros,79,88,1);
       
     break;

    case 23: //Сумма словами
      bb=rec.sumd+rec.uslb;

      memset(bros,'\0',sizeof(bros));
      iceb_u_preobr(bb,bros,0);
      iceb_u_bstab(stroka,bros,0,102,1);

      break;

    case 24: //Код нерезидента
      iceb_u_bstab(stroka,rec.kodnr.ravno(),63,71,1);
      break;

    case 26: //Назначение платежа
      if(kol_ps == 0)
       {
        //Включение/выключения режима 12 знаков на дюйм
        sprintf(stroka2,"\x1B\x4D%10s%s\x1B\x50","",rec.naz_plat.ravno());
        iceb_u_bstab(stroka,stroka2,0,300,1);
       }
      else
       {
        iceb_u_polen(rec.naz_plat.ravno(),stroka1,sizeof(stroka1),1,'\n');
        //Включение/выключения режима 12 знаков на дюйм
        sprintf(stroka2,"\x1B\x4D%10s%s\x1B\x50","",stroka1);
        iceb_u_bstab(stroka,stroka2,0,300,1);
       }       
      break;
    case 27: //Назначение платежа
      if(kol_ps >= 1)
       {
        iceb_u_polen(rec.naz_plat.ravno(),stroka1,sizeof(stroka1),2,'\n');
        //Включение/выключения режима 12 знаков на дюйм
        sprintf(stroka2,"\x1B\x4D%10s%s\x1B\x50","",stroka1);
        iceb_u_bstab(stroka,stroka2,0,300,1);
       }       
      break;

    case 28: //Назначение платежа
      if(kol_ps >= 2)
       {
        iceb_u_polen(rec.naz_plat.ravno(),stroka1,sizeof(stroka1),3,'\n');
        //Включение/выключения режима 12 знаков на дюйм
        sprintf(stroka2,"\x1B\x4D%10s%s\x1B\x50","",stroka1);
        iceb_u_bstab(stroka,stroka2,0,300,1);
       }       
      break;
    case 29: //Назначение платежа
//    case 30: //Назначение платежа
//      fprintf(ff,"\x1b\x46"); //Отмена режима выделенного шрифта
#if 0
###############################333    
      memset(stroka1,'\0',sizeof(stroka1));
      if(fgets(stroka1,sizeof(stroka1),ff1) != NULL)
       {
        iceb_u_ud_simv(stroka1,"\n");
        //Включение/выключения режима 12 знаков на дюйм
        sprintf(stroka2,"\x1B\x4D%10s%s\x1B\x50","",stroka1);
        iceb_u_bstab(stroka,stroka2,0,300,1);
       }
###################33333
#endif
      if(kol_ps >= 3)
       {
        iceb_u_polen(rec.naz_plat.ravno(),stroka1,sizeof(stroka1),4,'\n');
        //Включение/выключения режима 12 знаков на дюйм
        sprintf(stroka2,"\x1B\x4D%10s%s\x1B\x50","",stroka1);
        iceb_u_bstab(stroka,stroka2,0,300,1);
       }       
      break;

 
    default:
     break;
   }

  fprintf(ff,"%s",stroka);

 }

fprintf(ff,"\x1b\x46"); //Отмена режима выделенного шрифта
fprintf(ff,"\x12"); /*Выключение ужатого режима*/
fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля в ноль*/
//fprintf(ff,"\x1B\x55\x30"); /*Выключение однонаправленного режима*/
fprintf(ff,"\x1B\x32\n");/*Межсточный интервал 1/6 дюйма*/

if(nomk == 0 || nomk == 2)
if(kp > 1)
  for(int i=0; i< kolstrdok ; i++)
     fprintf(ff,"\n");

return(0);
}
