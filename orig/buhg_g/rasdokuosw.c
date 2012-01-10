/* $Id: rasdokuosw.c,v 1.10 2011-08-29 07:13:43 sasa Exp $ */
/*09.08.2011	10.02.2001	Белых А.И.	rasdokuosw.c
Распечатка накладной для учета основных средств
*/
#include <stdlib.h>
#include        <errno.h>
#include	"buhg_g.h"
#include <unistd.h>

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern SQL_baza bd;

void rasdokuosw(const char *data_dok, //Дата документа
int podr,
const char *nomdok, //Номер документа
int tipz,  //1-приход 2-расход
GtkWidget *wpredok)
{
short dg,mg,gg;
char            nmo[512],nmo1[512];  /*Наименование организации*/
char            gor[512],gor1[512]; /*Адрес*/
char            pkod[32],pkod1[32]; /*Почтовый код*/
char            nmb[512],nmb1[512];  /*Наименование банка*/
char            mfo[32],mfo1[32];  /*МФО*/
char            nsh[32],nsh1[32];  /*Номер счета*/
char            inn[112],inn1[112];  /*Индивидуальный налоговый номер*/
char		grb[512],grb1[512];  /*Город банка*/
char		npnds[112],npnds1[112]; /*Номер плательщика НДС*/
char		tel[512],tel1[512];    /*Номер телефона*/
char		strsql[512];
SQL_str         row,row1;
SQLCURSOR curr;
int		kolstr;
char		bros[512],bros1[512];
char		kontr[32];
char		kodop[32];
char		osnov[512];
char		regnom[112];
int		mvnpp;
char		naimpr[512];
int		podr1;
char		naipod[512];
double		bs;
char		naim[512];
FILE 		*ff;
char		imaf[32];
double		cena=0.;
double		itogo=0.;
double		pnds=0.;
char		dover[512];
char		sherez[512];
short		dvd=0,mvd=0,gvd=0;
double		sumands=0.;
short		lnds=0;
short		kodot1=0;
short		kodot2=0;

iceb_u_rsdat(&dg,&mg,&gg,data_dok,1);

/*Читаем шапку документа*/
sprintf(strsql,"select kontr,kodop,osnov,dover,datvd,sherez,nds,\
kodol,kodolv,podrv,pn from Uosdok where datd='%04d-%02d-%02d' and nomd='%s' \
and tipz=%d",gg,mg,dg,nomdok,tipz);

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {

  sprintf(strsql,"Ненайден документ! Дата:%d.%d.%d Документ:%s",
  dg,mg,gg,nomdok);
  iceb_menu_soob(strsql,wpredok);
  return;
 }

strncpy(kontr,row[0],sizeof(kontr)-1);
strncpy(kodop,row[1],sizeof(kodop)-1);
memset(osnov,'\0',sizeof(osnov));

strncpy(osnov,row[2],sizeof(osnov)-1);
strncpy(dover,row[3],sizeof(dover)-1);
iceb_u_rsdat(&dvd,&mvd,&gvd,row[4],2);
strncpy(sherez,row[5],sizeof(sherez)-1);
lnds=atoi(row[6]);
kodot1=atoi(row[7]);
kodot2=atoi(row[8]);
podr1=atoi(row[9]);

pnds=atof(row[10]);

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

sprintf(bros,"select * from Kontragent where kodkon='00'");
if(iceb_sql_readkey(bros,&row,&curr,wpredok) != 1)
 {
  iceb_menu_soob(gettext("Не найден код контрагента 00!"),wpredok);
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
if(iceb_u_SRAV("00-",kontr,1) != 0)
 {

  sprintf(bros,"select * from Kontragent where kodkon='%s'",kontr);
  if(iceb_sql_readkey(bros,&row,&curr,wpredok) != 1)
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

sprintf(strsql,"select innom,bs,bsby,cena from Uosdok1 \
where datd='%04d-%02d-%02d' and nomd='%s' and tipz=%d",
gg,mg,dg,nomdok,tipz);
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 { 
  return;
 } 
sprintf(imaf,"uos%s.lst",nomdok);
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
iceb_u_startfil(ff);

fprintf(ff,"\x1b\x6C%c",10); /*Установка левого поля*/
fprintf(ff,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/
fprintf(ff,"\x12"); /*Отмена ужатого режима*/
fprintf(ff,"\x1B\x4D"); /*12-знаков*/


if(tipz == 1)
 fprintf(ff,gettext("\
  Кому                                 От кого\n"));
if(tipz == 2)
 fprintf(ff,gettext("\
   От кого                             Кому\n"));

fprintf(ff,"%-*s \x1B\x45N%s %s\x1B\x46\n",iceb_u_kolbait(40,nmo),nmo,kontr,nmo1);

memset(bros,'\0',sizeof(bros));
memset(bros1,'\0',sizeof(bros1));

sprintf(bros,"%s %s",gettext("Адрес"),gor);
sprintf(bros1,"%s %s",gettext("Адрес"),gor1);

//fprintf(ff,"%-40s %s\n",bros,bros1);

fprintf(ff,"%-*.*s %-*.*s\n",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros,iceb_u_kolbait(40,bros1),iceb_u_kolbait(40,bros1),bros1);

if(iceb_u_strlen(bros) > 40 || iceb_u_strlen(bros1) > 40)
 fprintf(ff,"%-*.*s %-*.*s\n",
 iceb_u_kolbait(40,iceb_u_adrsimv(40,bros)),
 iceb_u_kolbait(40,iceb_u_adrsimv(40,bros)),
 iceb_u_adrsimv(40,bros),
 iceb_u_kolbait(40,iceb_u_adrsimv(40,bros1)),
 iceb_u_kolbait(40,iceb_u_adrsimv(40,bros1)),
 iceb_u_adrsimv(40,bros1));

sprintf(bros,"%s %s",gettext("Код ОКПО"),pkod);
sprintf(bros1,"%s %s",gettext("Код ОКПО"),pkod1);
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s %s %s",gettext("Р/С"),nsh,
gettext("МФО"),mfo);

sprintf(bros1,"%s %s %s %s",gettext("Р/С"),nsh1,
gettext("МФО"),mfo1);

fprintf(ff,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s %s %s",gettext("в"),nmb,
gettext("гор."),grb);
sprintf(bros1,"%s %s %s %s",gettext("в"),nmb1,
gettext("гор."),grb1);
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s",
gettext("Налоговый номер"),inn);

sprintf(bros1,"%s %s",
gettext("Налоговый номер"),inn1);

fprintf(ff,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds);
sprintf(bros1,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds1);
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s",gettext("Телефон"),tel);
sprintf(bros1,"%s %s",gettext("Телефон"),tel1);
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

fprintf(ff,"\x1B\x50"); /*10-знаков*/

/*Читаем наименование операции*/
if(tipz == 1)
 strcpy(bros,"Uospri");
if(tipz == 2)
 strcpy(bros,"Uosras");

mvnpp=0;
memset(naimpr,'\0',sizeof(naimpr));

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros,kodop);
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),kodop);
  iceb_menu_soob(strsql,wpredok);
 }
else
 {

  mvnpp=atoi(row[1]);
  strncpy(naimpr,row[0],sizeof(naimpr)-1);
  
 }

if(mvnpp == 1) /*Внутреннее перемещение*/
 {
  /*Читаем наименование склада*/
  sprintf(strsql,"select naik from Uospod where kod=%d",
  podr1);
  memset(naipod,'\0',sizeof(naipod));

  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),podr1);
      iceb_menu_soob(strsql,wpredok);
     }
  else
      strncpy(naipod,row[0],sizeof(naipod)-1);

  if(tipz == 1)
   {
    sprintf(strsql,"%s: %s %d <= %s %d %s",
    naimpr,gettext("Подразделение"),podr,
    gettext("Подразделение"),podr1,naipod);
    fprintf(ff,"\n%.*s\n",iceb_u_kolbait(70,strsql),strsql);
   }
  if(tipz == 2)
   {
    sprintf(strsql,"%s: %s %d => %s %d %s",
       naimpr,gettext("Подразделение"),podr,
       gettext("Подразделение"),podr1,naipod);
    fprintf(ff,"\n%.*s\n",iceb_u_kolbait(70,strsql),strsql);

   }
  //Читаем фамилию материально-ответственного лица

  memset(naipod,'\0',sizeof(naipod));
  sprintf(strsql,"select naik from Uosol where kod=%d",kodot1);
  memset(naipod,'\0',sizeof(naipod));

  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код мат.-ответственного"),kodot1);
    iceb_menu_soob(strsql,wpredok);
   }
  else
    strncpy(naipod,row[0],sizeof(naipod)-1);

  if(tipz == 1)
    sprintf(bros1,"%d %s <= ",kodot1,naipod);
  if(tipz == 2)
    sprintf(bros1,"%d %s => ",kodot1,naipod);
  
  //Читаем фамилию материально-ответственного лица

  memset(naipod,'\0',sizeof(naipod));
  sprintf(strsql,"select naik from Uosol where kod=%d",kodot2);
  memset(naipod,'\0',sizeof(naipod));

  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код мат.-ответственного"),kodot2);
    iceb_menu_soob(strsql,wpredok);
   }
  else
    strncpy(naipod,row[0],sizeof(naipod)-1);

  sprintf(bros,"%d %s\n",kodot2,naipod);
  strcat(bros1,bros);
  fprintf(ff,"%.*s\n",iceb_u_kolbait(70,bros1),bros1);
  
 }
else
 fprintf(ff,"\n%s",naimpr);
fprintf(ff,"\n");

sprintf(bros,"N%s",nomdok);  

if(tipz == 1)
  fprintf(ff,"\n\
                   %s  %s\n",
  gettext("ПРИХОДНАЯ НАКЛАДНАЯ"),bros);
if(tipz == 2)
  fprintf(ff,"\n\
                   %s  %s\n",
  gettext("РАСХОДНАЯ НАКЛАДНАЯ"),bros);

memset(bros,'\0',sizeof(bros));
iceb_mesc(mg,1,bros);
fprintf(ff,"\
                  %d %s %d%s\n",
dg,bros,gg,gettext("г."));

if(dover[0] != '\0')
  fprintf(ff,"%sN%s %s %d.%d.%d%s\n",gettext("Доверенность"),
  dover,gettext("от"),
  dvd,mvd,gvd,gettext("г."));

if(osnov[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Основание"),osnov);


if(sherez[0] != '\0')
   fprintf(ff,"%s: %s\n",gettext("Через кого"),sherez);

fprintf(ff,"\x0F"); /*Включение ужатого режима печати*/
 
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Инвентарный номер|                    Наименование                        |Кол|    Цена     |    Сумма    |\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------\n");
      
while(cur.read_cursor(&row) != 0)
 {
  bs=atof(row[2]);
  if(bs == 0.)
   bs=atof(row[1]);
  cena=atof(row[3]);
  /*Читаем наименование основного средства*/
  memset(naim,'\0',sizeof(naim));

  sprintf(strsql,"select naim from Uosin where innom=%s",
  row[0]);

  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
   {
    sprintf(strsql,"Не найден инвентарный номер %s !",row[0]);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   strncpy(naim,row1[0],sizeof(naim)-1);
  if(tipz == 1 || mvnpp == 1)
   {
    fprintf(ff,"%-17s %-*.*s %3d %13.2f %13.2f\n",
    row[0],
    iceb_u_kolbait(56,naim),iceb_u_kolbait(56,naim),naim,
    1,bs,bs);
    itogo+=bs;
   }
  else
  if(tipz == 2)
   {
    fprintf(ff,"%-17s %-*.*s %-3d %13.2f %13.2f\n",
    row[0],
    iceb_u_kolbait(56,naim),iceb_u_kolbait(56,naim),naim,
    1,cena,cena);
    itogo+=cena;
   }
}
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
%*s:%13.2f\n",iceb_u_kolbait(92,gettext("Итого")),gettext("Итого"),itogo);

if(mvnpp == 0) /*Внешнее перемещение*/
 {
  if(lnds == 0 )
   {
    sprintf(bros,"%s %.2f%% :",gettext("НДС"),pnds);
    sumands=itogo*pnds/100.;
    sumands=iceb_u_okrug(sumands,0.01);
    fprintf(ff,"%*s%13.2f\n",iceb_u_kolbait(93,bros),bros,sumands);
   }

  itogo=itogo+sumands;
  itogo=iceb_u_okrug(itogo,0.01);

  strcpy(bros,gettext("Итого к оплате:"));
  fprintf(ff,"%*s%13.2f\n",iceb_u_kolbait(93,bros),bros,itogo);
 } 

memset(strsql,'\0',sizeof(strsql));
iceb_u_preobr(itogo,strsql,0);
fprintf(ff,"%s:%s\n",gettext("Сумма прописью"),strsql);

fprintf(ff,"\x1B\x50"); /*10-знаков*/
fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/


fprintf(ff,"\n\%s%s\n",gettext("\
Руководитель__________________        Главный  бухгалтер_______________\n\n"),
  gettext("\
Отпустил______________________        Принял__________________________"));

fprintf(ff,"\x12"); /*Отмена ужатого режима печати*/

if(mvnpp == 0 && tipz == 2) /*Внешнее перемещение*/
 {
  fprintf(ff,"\n");
  fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины*/
  fprintf(ff,"%s",gettext("Благодарим за сотрудничество !"));
  fprintf(ff,"\x14"); /*Выключение режима удвоенной ширины*/
  fprintf(ff,"\n");
 }
iceb_podpis(ff,wpredok);
fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/
fclose(ff);


class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf);
naimf.plus(gettext("Распечатка накладной"));
iceb_rabfil(&imafil,&naimf,"",0,wpredok);
}

