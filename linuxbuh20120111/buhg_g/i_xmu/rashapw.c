/*$Id: rashapw.c,v 1.6 2011-08-29 07:13:43 sasa Exp $*/
/*11.03.2010	02.02.2007	Белых А.И.	rashapw.c
Распечатка реквизитов контрагентов в заголовке дукумента
*/
#include <stdlib.h>
#include "../headers/buhg_g.h"

extern SQL_baza bd;

void rashapw(int tipz,int *kolstrok,const char *kodkontr00,const char *kontr,int ots,int kor,FILE *ffdok,
GtkWidget *wpredok)
{
char            nmo[512],nmo1[512];  /*Наименование организации*/
char            gor[512],gor1[512]; /*Адрес*/
char            pkod[24],pkod1[24]; /*Почтовый код*/
char            nmb[256],nmb1[256];  /*Наименование банка*/
char            mfo[32],mfo1[32];  /*МФО*/
char            nsh[32],nsh1[32];  /*Номер счета*/
char            inn[32],inn1[32];  /*Индивидуальный налоговый номер*/
char		grb[512],grb1[512];  /*Город банка*/
char		npnds[32],npnds1[32]; /*Номер плательщика НДС*/
char		tel[64],tel1[64];    /*Номер телефона*/
char		regnom[32]; /*Регистрационный номер частного предпринимателя*/
char bros[512];
char bros1[512];
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
/*Читаем реквизиты организации свои */

memset(nmo,'\0',sizeof(nmo));
memset(gor,'\0',sizeof(gor));
memset(pkod,'\0',sizeof(pkod));
memset(nmb,'\0',sizeof(nmb));
memset(mfo,'\0',sizeof(mfo));
memset(nsh,'\0',sizeof(nsh));
memset(inn,'\0',sizeof(inn));
memset(grb,'\0',sizeof(grb));
memset(npnds,'\0',sizeof(npnds));
memset(tel,'\0',sizeof(tel));

sprintf(bros,"select * from Kontragent where kodkon='%s'",kodkontr00);
if(sql_readkey(&bd,bros,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",
  gettext("Не найден код контрагента"),kodkontr00);
  iceb_menu_soob(strsql,wpredok);
 }
else
 {
  strncpy(nmo,row[1],sizeof(nmo)-1);
  strncpy(gor,row[3],sizeof(gor)-1);
  strncpy(pkod,row[5],sizeof(pkod)-1);
  strncpy(nmb,row[2],sizeof(nmb)-1);
  strncpy(mfo,row[6],sizeof(mfo)-1);
  strncpy(nsh,row[7],sizeof(nsh)-1);
  strncpy(inn,row[8],sizeof(inn)-1);
  strncpy(grb,row[4],sizeof(grb)-1);
  strncpy(npnds,row[9],sizeof(npnds)-1);
  strncpy(tel,row[10],sizeof(tel)-1);
 } 

memset(nmo1,'\0',sizeof(nmo1));
memset(gor1,'\0',sizeof(gor1));
memset(pkod1,'\0',sizeof(pkod1));
memset(nmb1,'\0',sizeof(nmb1));
memset(mfo1,'\0',sizeof(mfo1));
memset(nsh1,'\0',sizeof(nsh1));
memset(inn1,'\0',sizeof(inn1));
memset(grb1,'\0',sizeof(grb1));
memset(npnds1,'\0',sizeof(npnds1));
memset(tel1,'\0',sizeof(tel1));
memset(regnom,'\0',sizeof(regnom));

/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/
if(atof(kontr) != 0. || kontr[0] != '0')
 {

  sprintf(bros,"select * from Kontragent where kodkon='%s'",kontr);
  if(sql_readkey(&bd,bros,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",
    gettext("Не найден код контрагента"),kontr);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    strncpy(nmo1,row[1],sizeof(nmo1)-1);
    strncpy(gor1,row[3],sizeof(gor1)-1);
    strncpy(pkod1,row[5],sizeof(pkod1)-1);
    strncpy(nmb1,row[2],sizeof(nmb1)-1);
    strncpy(mfo1,row[6],sizeof(mfo1)-1);
    strncpy(nsh1,row[7],sizeof(nsh1)-1);
    strncpy(inn1,row[8],sizeof(inn1)-1);
    strncpy(grb1,row[4],sizeof(grb1)-1);
    strncpy(npnds1,row[9],sizeof(npnds1)-1);
    strncpy(tel1,row[10],sizeof(tel1)-1);
    strncpy(regnom,row[15],sizeof(regnom)-1);
    
   }
 }

//if(ots != 0)
// fprintf(ffdok,"\x1b\x6C%c",ots); /*Установка левого поля*/
fprintf(ffdok,"\x1B\x33%c",30-kor); /*Уменьшаем межстрочный интервал*/
//fprintf(ffdok,"\x12"); /*Отмена ужатого режима*/
//fprintf(ffdok,"\x1B\x4D"); /*12-знаков*/

if(tipz == 1)
 fprintf(ffdok,gettext("\
  Кому                                 От кого\n"));
if(tipz == 2)
 fprintf(ffdok,gettext("\
   От кого                             Кому\n"));
*kolstrok+=1;

fprintf(ffdok,"%-*s ",iceb_u_kolbait(40,nmo),nmo);
if(iceb_u_strlen(nmo1) <= 35)
  fprintf(ffdok,"\x1B\x45N%s %s\x1B\x46\n",kontr,nmo1);
else
  fprintf(ffdok,"\x1B\x45\x0FN%s %s\x1B\x46\x12\n",kontr,nmo1);
*kolstrok+=1;

memset(bros,'\0',sizeof(bros));
sprintf(bros,"%s %s",gettext("Адрес"),gor);
sprintf(bros1,"%s %s",gettext("Адрес"),gor1);
fprintf(ffdok,"%-*.*s %s\n",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

sprintf(bros,"%s %s",gettext("Код ОКПО"),pkod);
sprintf(bros1,"%s %s",gettext("Код ОКПО"),pkod1);
fprintf(ffdok,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

sprintf(bros,"%s %s %s %s",gettext("Р/С"),nsh,
gettext("МФО"),mfo);

sprintf(bros1,"%s %s %s %s",gettext("Р/С"),nsh1,
gettext("МФО"),mfo1);
fprintf(ffdok,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

sprintf(bros,"%s %s %s %s",gettext("в"),nmb,
gettext("г."),grb);
sprintf(bros1,"%s %s %s %s",gettext("в"),nmb1,
gettext("г."),grb1);
fprintf(ffdok,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

sprintf(bros,"%s %s",
gettext("Налоговый номер"),inn);
sprintf(bros1,"%s %s",
gettext("Налоговый номер"),inn1);

fprintf(ffdok,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

sprintf(bros,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds);
sprintf(bros1,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds1);
fprintf(ffdok,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

if(tipz == 2)
if(regnom[0] != '\0')
 {
  fprintf(ffdok,"%s: %s\n",gettext("Регистрационный номер"),regnom);
  *kolstrok+=1;
 }  

//fprintf(ffdok,"\x1B\x50"); /*10-знаков*/


}
