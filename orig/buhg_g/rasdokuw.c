/* $Id: rasdokuw.c,v 1.17 2011-02-21 07:35:56 sasa Exp $ */
/*23.03.2010	21.04.2000	Белых А.И.	rasdokuw.c
Распечтка акта выполненых работ, счета на оплату
*/
#include <stdlib.h>
#include        <errno.h>
#include	"buhg_g.h"

void saldo_kontr(char *sh, //Счет
char *kor,  //Код контрагента
double *db,  //Дебет
double *kr,  //Дебет
short dn,short mn,short gn, //Дата начала
short dk,short mk,short gk, //Дата конца
GtkWidget *wpredok);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern SQL_baza bd;

void            rasdokuw(short dg,short mg,short gg, //Дата документа
int podr, //Подразделение
const char *nomdok, //Номер документа
short lnds, //Льготы по НДС
short vidr, //0-акт вып. работ 1-счет
FILE *ff,  //Указатель на файл
int tipz,  //1-приход 2-расход
int saldo, //0-без учета сальдо 1-с учетом
GtkWidget *wpredok)
{
char            nmo[512],nmo1[512];  /*Наименование организации*/
char            gor[512],gor1[512]; /*Адрес*/
char            pkod[32],pkod1[32]; /*Почтовый код*/
char            nmb[512],nmb1[512];  /*Наименование банка*/
char            mfo[32],mfo1[32];  /*МФО*/
char            nsh[32],nsh1[32];  /*Номер счета*/
char            inn[112],inn1[24];  /*Индивидуальный налоговый номер*/
char		grb[512],grb1[512];  /*Город банка*/
char		npnds[112],npnds1[112]; /*Номер плательщика НДС*/
char		rsnds[32],rsnds1[32]; //спец счёт для НДС
char		tel[512],tel1[512];    /*Номер телефона*/
char		rnhp[112];           //Регистрационный номер частного предпринимателя
char		strsql[512];
SQL_str         row,row1;
char		bros[512],bros1[512];
char		regnom[112];
char		kontr[56];
int		metka,kodzap;
int		kolstr;
double		kolih,cena,cenasnd;
char		naim[512];
double		suma;
double		itogo=0.,itogonds=0.;
char		naipod[512];
int		podr1;
short		mvnpp;
char		naimpr[512];
char		kodop[56];
double		sumkor,bb;
char		datop[16];
short		d,m,g;
short		ds,ms,gs; //Дата сальдо
short		dlina;
char		osnov[512];
double		deb,kre;
char		shetuslug[56];
char		kodkaz[56];
char		rrkaz[56];
iceb_u_str	naimnal;
float		procent=0.;
char		kodkontr00[56];
class iceb_u_str dover("");
class iceb_u_str datdov("");
class iceb_u_str sherez("");
float pnds=0.;

SQLCURSOR curr;
/*
printw("\nrasdoku-Дата:%d.%d.%d Подр.:%d %s %d %d tipz=%d\n",
dg,mg,gg,podr,nomdok,lnds,vidr,tipz);
OSTANOV();
*/

/*Читаем шапку документа*/
sprintf(strsql,"select kontr,kodop,sumkor,datop,osnov,pn,dover,datdov,sherez from Usldokum \
where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
gg,mg,dg,podr,nomdok,tipz);

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  iceb_u_str repl;

  sprintf(strsql,"rasdoku-%s !",gettext("Не найден документ"));

  repl.plus_ps(strsql);

  sprintf(strsql,"%s:%d.%d.%d %s:%s %s:%d",
  gettext("Дата"),
  dg,mg,gg,
  gettext("Документ"),
  nomdok,
  gettext("Подразделение"),
  podr);

  repl.ps_plus(strsql);

  iceb_menu_soob(&repl,wpredok);
  return;
 }

pnds=atof(row[5]);
strncpy(kontr,row[0],sizeof(kontr)-1);
strncpy(kodop,row[1],sizeof(kodop)-1);
sumkor=atof(row[2]);

memset(datop,'\0',sizeof(datop));
strncpy(datop,row[3],sizeof(datop)-1);

memset(osnov,'\0',sizeof(osnov));
strncpy(osnov,row[4],sizeof(osnov)-1);
dover.new_plus(row[6]);
datdov.new_plus(row[7]);
sherez.new_plus(row[8]);

usldopnalw(tipz,kodop,&procent,&naimnal);

class iceb_u_str imaf_nast("uslnast.alx");

strcpy(kodkontr00,"00");
if(tipz == 2)
 {
  memset(bros,'\0',sizeof(bros));
  if(iceb_poldan(kodop,bros,imaf_nast.ravno(),wpredok) == 0)
   {
    memset(kodkontr00,'\0',sizeof(kodkontr00));
    strncpy(kodkontr00,bros,sizeof(kodkontr00)-1);
   }
 }
//printw("\nkodkontr00=%s\n",kodkontr00);
//OSTANOV();

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
memset(rnhp,'\0',sizeof(rnhp));
memset(rsnds,'\0',sizeof(rsnds));

sprintf(bros,"select * from Kontragent where kodkon='%s'",kodkontr00);
if(sql_readkey(&bd,bros,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kodkontr00);
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
  strncpy(rnhp,row[15],sizeof(rnhp)-1);
  strncpy(rsnds,row[17],sizeof(rsnds)-1);
 } 

//Чтение настроек для бюджетных организаций

memset(bros,'\0',sizeof(bros));
memset(kodkaz,'\0',sizeof(kodkaz));
if(iceb_poldan("Код казначейства",bros,imaf_nast.ravno(),wpredok) == 0)
 if(bros[0] != '\0')
  {
   strncpy(kodkaz,bros,sizeof(kodkaz)-1);  
   iceb_poldan("Расчетный счет казначейства",rrkaz,imaf_nast.ravno(),wpredok);
   iceb_poldan("Регистрационный счет в казначействе",nsh,imaf_nast.ravno(),wpredok);
   iceb_poldan("МФО казначейства",mfo,imaf_nast.ravno(),wpredok);
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
memset(rsnds1,'\0',sizeof(rsnds1));

/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/
if(iceb_u_atof(kontr) != 0. || kontr[0] != '0')
 {

  sprintf(bros,"select * from Kontragent where kodkon='%s'",kontr);
  if(sql_readkey(&bd,bros,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    if(row[16][0] == '\0')
      strncpy(nmo1,row[1],sizeof(nmo1)-1);
    else
      strncpy(nmo1,row[16],sizeof(nmo1)-1);
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
    strncpy(rsnds1,row[17],sizeof(rsnds1)-1);
    
   }
 }

sprintf(strsql,"select metka,kodzap,kolih,cena,ei,dnaim from Usldokum1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d order by metka,kodzap asc",
gg,mg,dg,podr,nomdok,tipz);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;

int ots=4;
memset(bros,'\0',sizeof(bros)); 
if(iceb_poldan("Отступ от левого края",bros,imaf_nast.ravno(),wpredok) == 0)
  ots=(short)iceb_u_atof(bros);

fprintf(ff,"\x1b\x6C%c",ots); /*Установка левого поля*/
fprintf(ff,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/
fprintf(ff,"\x12"); /*Отмена ужатого режима*/
fprintf(ff,"\x1B\x4D"); /*12-знаков*/
if(vidr == 1)
  fprintf(ff,"\x1B\x45"); /*Включение режима выделенной печати*/


if(vidr == 0)
 {
  if(tipz == 1)
   fprintf(ff,gettext("\
    Получатель                             Поставщик\n"));
  if(tipz == 2)
   fprintf(ff,gettext("\
     Поставщик                             Плательщик\n"));
  fprintf(ff,"%-*.*s \x1B\x45N%s %-*.*s\x1B\x46\n",
  iceb_u_kolbait(40,nmo),iceb_u_kolbait(40,nmo),nmo,
  kontr,
  iceb_u_kolbait(40,nmo1),iceb_u_kolbait(40,nmo1),nmo1);

  if(iceb_u_strlen(nmo) > 40 || iceb_u_strlen(nmo1) > 40)
    fprintf(ff,"%-*.*s \x1B\x45%-*.*s\x1B\x46\n",
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo)),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo)),
    iceb_u_adrsimv(40,nmo),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo1)),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo1)),
    iceb_u_adrsimv(40,nmo1));
 }

if(vidr == 1)
 {
  fprintf(ff,gettext("\
    Получатель                            Плательщик\n"));

  fprintf(ff,"%-*.*s N%s %-*.*s\n",
  iceb_u_kolbait(40,nmo),iceb_u_kolbait(40,nmo),nmo,
  kontr,
  iceb_u_kolbait(40,nmo1),iceb_u_kolbait(40,nmo1),nmo1);
  
  if(iceb_u_strlen(nmo) > 40 || iceb_u_strlen(nmo1) > 40)
    fprintf(ff,"%-*.*s %-*.*s\n",
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo)),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo)),
    iceb_u_adrsimv(40,nmo),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo1)),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo1)),
    iceb_u_adrsimv(40,nmo1));
 }

memset(bros,'\0',sizeof(bros));
memset(bros1,'\0',sizeof(bros1));
sprintf(bros,"%s %s",gettext("Адрес"),gor);
sprintf(bros1,"%s %s",gettext("Адрес"),gor1);

fprintf(ff,"%-*.*s %-*.*s\n",
iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros,
iceb_u_kolbait(40,bros1),iceb_u_kolbait(40,bros1),bros1);

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
if(rsnds[0] != '\0' || rsnds1[0] != '\0')
 {
  sprintf(bros,"%s %s","Спец счёт для НДС",rsnds);
  sprintf(bros1,"%s %s","Спец счёт для НДС",rsnds1);

  fprintf(ff,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
 }

sprintf(bros,"%s %s %s %s",gettext("в"),nmb,
gettext("гор."),grb);
sprintf(bros1,"%s %s %s %s",gettext("в"),nmb1,
gettext("гор."),grb1);

fprintf(ff,"%-*.*s %s\n",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros,bros1);
if(iceb_u_strlen(bros) > 40)
 fprintf(ff,"%s\n",iceb_u_adrsimv(40,bros));
if(kodkaz[0] != '\0')
 {

  fprintf(ff,"%s %s %s\n",
  gettext("Код"),
  gettext("казначейства"),kodkaz);
  

  fprintf(ff,"%s %s %s\n",gettext("Р/С"),
  gettext("казначейства"),rrkaz);
  
 }
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
 strcpy(bros,"Usloper1");
if(tipz == 2)
 strcpy(bros,"Usloper2");

mvnpp=0;
memset(naimpr,'\0',sizeof(naimpr));

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros,kodop);
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код операции"),kodop);
  iceb_menu_soob(strsql,wpredok);
 }
else
 {

  mvnpp=atoi(row[1]);
  strncpy(naimpr,row[0],sizeof(naimpr)-1);
  
 }

/*Читаем наименование подразделения*/
sprintf(strsql,"select naik from Uslpodr where kod=%d",
podr);
naipod[0]='\0';

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),podr);
  iceb_menu_soob(strsql,wpredok);
 }
else
  strncpy(naipod,row[0],sizeof(naipod)-1);

if(vidr == 0 || vidr == 2)
  fprintf(ff,"\n%s",naipod);

if(mvnpp == 1) /*Внутреннее перемещение*/
 {
  if(iceb_u_atof(kontr) == 0. && iceb_u_pole(kontr,bros,2,'-') == 0 && kontr[0] == '0')
   {
    iceb_u_pole(kontr,bros,2,'-');
    podr1=(int)iceb_u_atof(bros);
    /*Читаем наименование склада*/
    sprintf(strsql,"select naik from Uslpodr where kod=%d",
    podr1);
    naipod[0]='\0';

    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),podr1);
      iceb_menu_soob(strsql,wpredok);
     }
    else
      strncpy(naipod,row[0],sizeof(naipod)-1);

    if(tipz == 1)
       fprintf(ff,"\n%s: %s %d <= %s %d %s",
       naimpr,gettext("Подразделение"),podr,
       gettext("Подразделение"),podr1,naipod);
    if(tipz == 2)
       fprintf(ff,"\n%s: %s %d => %s %d %s",
       naimpr,gettext("Подразделение"),podr,
       gettext("Подразделение"),podr1,naipod);
   }
 }

fprintf(ff,"\n%s",naimpr);
fprintf(ff,"\n");

sprintf(bros,"N%s",nomdok);  

if(vidr == 0)
 {
    fprintf(ff,"\n\
                %s  %s\n",gettext("АКТ ВЫПОЛНЕНЫХ РАБОТ"),bros);
 }
if(vidr == 1)
  fprintf(ff,"\n\
                %s %s\n",
  gettext("Счет"),bros);

memset(bros,'\0',sizeof(bros));
iceb_mesc(mg,1,bros);
fprintf(ff,"\
                  %d %s %d%s\n",
dg,bros,gg,gettext("г."));

if(osnov[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Основание"),osnov);

if(dover.getdlinna() > 1)
  fprintf(ff,"%s: %s %s %s\n",
  gettext("Доверенность"),
  dover.ravno(),
  gettext("от"),
  datdov.ravno());

if(sherez.getdlinna() > 1)
  fprintf(ff,"%s: %s\n",gettext("Через кого"),sherez.ravno());

if(datop[0] != '0')
 {
//  printw("datop=%s\n",datop);
//  OSTANOV();
  iceb_u_rsdat(&d,&m,&g,datop,2);
  if(d == 0 || iceb_u_sravmydat(g,m,d,gg,mg,dg) >= 0)
   {
    if(vidr == 0)
     fprintf(ff,"\x1B\x45%s: %d.%d.%d%s\x1B\x45\x1b\x46\n",
     gettext("Конечная дата расчета"),d,m,g,
     gettext("г."));
    if(vidr == 1)
     fprintf(ff,"\x1B\x45%s %d.%d.%d%s\x1B\x45\x1b\x46\n",
     gettext("Счет действителен до"),d,m,g,
     gettext("г."));
    
   }
 }
if(vidr == 1)
 {
  fprintf(ff,"\x1B\x46"); /*Выключение режима выделенной печати*/
  fprintf(ff,"\x1B\x47"); /*Включение режима двойного удара*/
 }

fprintf(ff,"\x1B\x4D"); /*12-знаков*/
//fprintf(ff,"\x0F"); /*Включение ужатого режима печати*/
 
if(lnds == 0 )
 {
  fprintf(ff,"\
------------------------------------------------------------------------------------------\n");
  fprintf(ff,gettext("\
 Код |           Наименование услуг            |Един|Количе-|Цена с | Цена без |Сумма без|\n\
     |                                         |изм.|  ство |  НДС  |   НДС    |  НДС    |\n"));
  fprintf(ff,"\
------------------------------------------------------------------------------------------\n");
      
 }
else
 {
  fprintf(ff,"\
------------------------------------------------------------------------------------------\n");
  fprintf(ff,gettext("\
 Код |               Наименование услуг                |Един|Количе-| Цена без |Сумма без|\n\
     |                                                 |изм.|  ство |   НДС    |  НДС    |\n"));
  fprintf(ff,"\
------------------------------------------------------------------------------------------\n");
      
 }
itogo=0;

if(saldo == 1)
 {
  deb=kre=0.;
  memset(naim,'\0',sizeof(naim));
  ds=1; ms=mg; gs=gg;
  iceb_u_dpm(&ds,&ms,&gs,2);
    
  memset(shetuslug,'\0',sizeof(shetuslug));

  //Узнаем счет учета операции
  sprintf(bros,"Usloper%d",tipz);
  sprintf(strsql,"select shet from %s where kod='%s'",bros,kodop);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(shetuslug,row[0],sizeof(shetuslug)-1);

  if(shetuslug[0] != '\0')
   {
//    printw("\n%s 1.%d.%d %s:%s\n",gettext("Сальдо на"),
//    mg,gg,gettext("Счет"),shetuslug);
//    OSTANOV();
    sprintf(naim,"%s 1.%d.%d",gettext("Сальдо на"),mg,gg);

//    salork(shetuslug,kontr,&deb,&kre,0,"\0","\0","\0",1,1,gs,ds,ms,gs,"\0",0);

    saldo_kontr(shetuslug,kontr,&deb,&kre,1,1,gs,ds,ms,gs,wpredok);

    //Смотрим сумму оплат за месяц если она была
  //  printw("deb=%f kre=%f\n",deb,kre);
  //  OSTANOV();
    deb-=kre;
   }
  else
    sprintf(naim,"%s",gettext("Не введен счет услуг"));
   
  bb=deb-deb*pnds/(100+pnds);
  suma=iceb_u_okrug(bb,okrg1);
  cena=iceb_u_okrug(bb,okrcn);


  if(lnds == 0)
   fprintf(ff,"%5s %-*.*s %-4s %7.7g %7.2f %10.10g %9.2f\n",
   " ",
   iceb_u_kolbait(41,naim),iceb_u_kolbait(41,naim),naim,
   " ",1.,deb,cena,suma);
  else  
   fprintf(ff,"%5s %-*.*s %-4s %7.7g %10.10g %9.2f\n",
   " ",
   iceb_u_kolbait(49,naim),iceb_u_kolbait(49,naim),naim,
   " ",1.,cena,suma);
  itogo+=suma;

  suma=kre=0.;

  memset(naim,'\0',sizeof(naim));
  if(shetuslug[0] != '\0')
   {
//    sprintf(naim,"%s %d.%d %s:%s",gettext("Оплата в"),
//    mg,gg,gettext("Счет"),shetuslug);
    sprintf(naim,"%s %d.%d",gettext("Оплата в"),mg,gg);

    //Смотрим сумму оплат за месяц если она была
    sprintf(strsql,"select kre from Prov where datp>='%d-%d-01' and \
datp<='%d-%d-31' and kodkon='%s' and sh='%s' and kre <> 0.",
    gg,mg,gg,mg,kontr,shetuslug);
    SQLCURSOR cur1;    
    if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    while(cur1.read_cursor(&row1) != 0)
     kre+=atof(row1[0]);

   }
  bb=kre-kre*pnds/(100+pnds);
  suma=iceb_u_okrug(bb,okrg1)*-1;
  cena=iceb_u_okrug(bb,okrcn)*-1;


  if(lnds == 0)
   {
    fprintf(ff,"%5s %-*.*s %-4s %7.7g %7.2f %10.10g %9.2f\n",
    " ",iceb_u_kolbait(41,naim),iceb_u_kolbait(41,naim),naim," ",1.,kre*-1,cena,suma);
    if(iceb_u_strlen(naim) > 41)
      fprintf(ff,"%5s %s\n",
      " ",iceb_u_adrsimv(41,naim));
   }
  else  
   {
    fprintf(ff,"%5s %-*.*s %-4s %7.7g %10.10g %9.2f\n",
    " ",iceb_u_kolbait(49,naim),iceb_u_kolbait(49,naim),naim," ",1.,cena,suma);
    if(iceb_u_strlen(naim) > 49)
      fprintf(ff,"%5s %s\n",
      " ",iceb_u_adrsimv(49,naim));
    
   }
  itogo+=suma;

 }

while(cur.read_cursor(&row) != 0)
 {

  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kolih=atof(row[2]);
  cena=atof(row[3]);
  cena=iceb_u_okrug(cena,okrcn);
  suma=cena*kolih;
  suma=iceb_u_okrug(suma,okrg1);
  itogo+=suma;
  cenasnd=cena+cena*pnds/100.;
  cenasnd=iceb_u_okrug(cenasnd,okrg1);
  
  /*Читаем наименование услуги*/
  memset(naim,'\0',sizeof(naim));

  if(metka == 0)
   sprintf(strsql,"select naimat from Material where kodm=%d",
   kodzap);

  if(metka == 1)
   sprintf(strsql,"select naius from Uslugi where kodus=%d",
   kodzap);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    if(metka == 0)
     sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodzap);
    if(metka == 1)
     sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),kodzap);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    strncpy(naim,row1[0],sizeof(naim)-1);
    if(row[5][0] != '\0')
     {
      strcat(naim," ");
      strcat(naim,row[5]);
     }    
   }
  if(lnds == 0)
   {
    fprintf(ff,"%5d %-*.*s %-*s %7.7g %7.2f %10.10g %9.2f\n",
    kodzap,
    iceb_u_kolbait(41,naim),iceb_u_kolbait(41,naim),naim,
    iceb_u_kolbait(4,row[4]),row[4],kolih,cenasnd,cena,suma);
    if(iceb_u_strlen(naim) > 41)
     fprintf(ff,"%5s %s\n"," ",iceb_u_adrsimv(41,naim));
   }
  else  
   {
    fprintf(ff,"%5d %-*.*s %-*s %7.7g %10.10g %9.2f\n",
    kodzap,iceb_u_kolbait(49,naim),iceb_u_kolbait(49,naim),naim,iceb_u_kolbait(4,row[4]),row[4],kolih,cena,suma);
    if(iceb_u_strlen(naim) > 49)
     fprintf(ff,"%5s %s\n"," ",iceb_u_adrsimv(49,naim));
   }
}

dlina=80;
if(lnds != 0)
 dlina=80;

fprintf(ff,"\
------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%9.2f\n",
iceb_u_kolbait(dlina-1,gettext("Итого")),gettext("Итого"),
itogo);
 
if(mvnpp == 0) /*Внешнее перемещение*/
 {


  bb=0.;

  if(sumkor != 0)
   {
    bb=sumkor*100./itogo;
    bb=iceb_u_okrug(bb,0.01);
    if(sumkor < 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Снижка"),bb*(-1));
    if(sumkor > 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Надбавка"),bb);

    fprintf(ff,"%*s%9.2f/З %s %.2f\n",
    iceb_u_kolbait(dlina,strsql),strsql,
    sumkor,gettext("НДС"),sumkor+sumkor*pnds/100.);

    if(sumkor < 0)
      sprintf(strsql,gettext("Итого со снижкой :"));
    if(sumkor > 0)
      sprintf(strsql,gettext("Итого с надбавкой :"));

      fprintf(ff,"%*s%9.2f\n",
      iceb_u_kolbait(dlina,strsql),strsql,
      sumkor+itogo);

   }

  double bbproc=0.;
  if(procent != 0.)
   {
    sprintf(strsql,"%s %.2f%% :",naimnal.ravno(),procent);
    bbproc=(itogo+sumkor)*procent/100.;
    bbproc=iceb_u_okrug(bbproc,0.01);
    fprintf(ff,"%*s%9.2f\n",iceb_u_kolbait(dlina,strsql),strsql,bbproc);

   }

  bb=0.;

  if(lnds == 0 )
   {
    double sumandspr=0.;
    if(tipz == 1)
     {
      sprintf(strsql,"select sumnds from Usldokum where \
datd='%04d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
      gg,mg,dg,podr,nomdok,tipz);
      if(sql_readkey(&bd,strsql,&row,&curr) == 1)
        sumandspr=atof(row[0]);
     }

    sprintf(strsql,"%s %.2f%% :",gettext("НДС"),pnds);
    if(sumandspr != 0.)
      bb=sumandspr;
    else        
      bb=(itogo+sumkor)*pnds/100.;
    bb=iceb_u_okrug(bb,0.01);
    fprintf(ff,"%*s%9.2f\n",iceb_u_kolbait(dlina,strsql),strsql,bb);
   }
  itogonds=(itogo+sumkor)+bb+ bbproc;
  strcpy(bros,gettext("Итого к оплате:"));
  if(tipz == 1)
    sprintf(bros,"%s:",gettext("Итого"));
  fprintf(ff,"%*s%9.2f\n",iceb_u_kolbait(dlina,bros),bros,itogonds);
 }

memset(strsql,'\0',sizeof(strsql));
iceb_u_preobr(itogonds,strsql,0);

fprintf(ff,"%s:%s\n",gettext("Сумма прописью"),strsql);
if(lnds != 0 && mvnpp == 0)
 fprintf(ff,"%s\n",gettext("Без НДС"));

fprintf(ff,"\x1B\x50"); /*10-знаков*/
fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
fprintf(ff,"\x12"); /*Отмена ужатого режима печати*/
if(vidr == 1)
 {
  fprintf(ff,"\n\%*s_______________\n\n",iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"));
  if(rnhp[0] == '\0')
    fprintf(ff,"%*s________________\n",iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"));
 }

if(vidr == 0)
 {
  memset(bros,'\0',sizeof(bros));
  iceb_poldan("В акте выполненых работ подписи только исполнителя и заказчика",bros,imaf_nast.ravno(),wpredok);
  if(iceb_u_SRAV(bros,"Вкл",1) != 0)
   {  

    fprintf(ff,"\n\
%*s_______________ %*s_______________\n\n\
%*s_______________ %*s_______________\n\n",
    iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
    iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
    iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),
    iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"));
   }
  if(tipz == 1)
    fprintf(ff,"\
%*s_______________ %*s_______________\n",
    iceb_u_kolbait(20,gettext("Заказчик")),gettext("Заказчик"),
    iceb_u_kolbait(20,gettext("Исполнитель")),gettext("Исполнитель"));

  if(tipz == 2)
    fprintf(ff,"\
%*s_______________ %*s_______________\n",
    iceb_u_kolbait(20,gettext("Исполнитель")),gettext("Исполнитель"),
    iceb_u_kolbait(20,gettext("Заказчик")),gettext("Заказчик"));
 }
 

//Если есть приложение - распечатываем
sprintf(strsql,"select zapis from Usldokum3 where podr=%d and god=%d \
and tp=%d and nomd='%s'",podr,gg,tipz,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr != 0)
 fprintf(ff,"\n\n\n%s:\n",gettext("Приложение к документу"));

while(cur.read_cursor(&row) != 0)
  fprintf(ff,"%s\n",row[0]);

//if(vidr == 1)
 fprintf(ff,"\x1B\x48"); /*Выключение режима двойного удара*/


fprintf(ff,"\x12"); /*Отмена ужатого режима печати*/
if(mvnpp == 0 && tipz == 2) /*Внешнее перемещение*/
 {
  if(vidr != 0) //Для акта выполненых работ не надо
   if(iceb_nalndoh(wpredok) == 0)
    fprintf(ff,"%s\n",gettext("Предприятие является плательшиком налога на доход на общих основаниях"));

  fprintf(ff,"\n\x0E"); /*Включение режима удвоенной ширины*/
 // fprintf(ff,"\n%7s%s\n"," ",gettext("Благодарим за сотрудничество !"));
  fprintf(ff,gettext("Благодарим за сотрудничество !"));
  fprintf(ff,"\x14"); /*Выключение режима удвоенной ширины*/
 }
  
iceb_podpis(ff,wpredok);
fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/

}
