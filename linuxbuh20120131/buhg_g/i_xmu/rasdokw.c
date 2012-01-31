/* $Id: rasdokw.c,v 1.31 2011-08-29 07:13:43 sasa Exp $ */
/*23.03.2010    15.04.1997      Белых А.И.      rasdokw.c
Распечатка накладных, счета, Акта приемки
*/
#define		KLST1 50 /*для графической и терминальной версии значения разные*/
#define		KLST2 67
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <errno.h>
#include        <pwd.h>
#include        <math.h>
#include	"../headers/buhg_g.h"
#include        <unistd.h>

void	        vztr(short,short,short,int,const char*,double*,FILE*,short,GtkWidget*);
void 		sapnak(short,FILE*);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern short    srtnk; /*0-не включена 1-включена сортировка записей в накладной*/
extern short	vtara; /*Код группы возвратная тара*/
extern short    obzap; /*0-не объединять записи 1-обединять*/
extern SQL_baza  bd;

void rasdokw(short dg,short mg,short gg, //дата документа
int skl,  //Склад
const char *nomdok, //Номер документа
const char *imaf,   // имя файла
short lnds,  //Льготы по НДС*
short vr,  //0-накладная 1-счет 2-акт приемки 3-акт списания*/
FILE *f1,double ves, //Вес
GtkWidget *wpredok)
{
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
char            rsnds[112],rsnds1[112]; //счет для НДС
char		regnom[16]; /*Регистрационный номер частного предпринимателя*/
FILE		*ff;
char            bros1[512];
int             i,i1;
char            nomn[512];  /*Номенклатурный номер*/
double          itogo;
double          itogo1;
double          bb=0.,bb1=0.,bb2=0.,bb4=0.;
short           d,m,g;
short           dd,md,gd;
short           nnds=0; //0-накладная 1-накладная с ценой НДС 2-накладная с номером заказа 
short		mnt; /*Метка наличия тары*/
double		sumt; /*Сумма по таре*/
char		kdtr[112]; /*Коды тары*/
short		mtpr; /*Метка первого и второго прохода*/
short		mtar=0; /*Метка наличия oбычной тары в накладной*/
short		mppu; /*Метка первого прохода услуг*/
double		mest;
char		naimpr[512];
int		skl1;
short		kor,ots;
short           mvnpp; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
char		imaf1[32];
int             kodm,kodmz,nk;
double		kol,sum;
double		kolt;
double		kratn,kratz,cenan,cenaz;
char		ein[56],eiz[56];
char		naim[512],naimz[512];
short		klst,klli;
double		sumkor;
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
char		kpos[56];
char		kop[56]; /*Код операции*/
int		tipz=0;
char		naiskl[512];
char		nomnn[56]; /*Номер счет-фактуры*/
char		nn1[128]; /*Номер встречного документа*/
char		bros[512];
char		str[1024];
short		vtar; /*Метка возвратной тары*/
short		kgrm; /*код группы материалла*/
char		shu[56]; /*счет учета материалла*/
double		kolih,cena;
char		fmoll[512]; //Фамилия материально-ответственного
char		nomz[56];
char		nomzz[56];
short		metkadov=0; //0- доверенность печатается только если реквизит введен 1- всегда
char		mesqc[32];
char		kodkontr00[32];
short           metka_close=0;
SQLCURSOR cur,cur1;
printf("rasdokw-vr=%d imaf=%s\n",vr,imaf);
mest=mnt=nnds=0;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gg,mg,dg,skl,nomdok);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"rasdokw-%s",gettext("Не найден документ !")),
  repl.plus(strsql);
  repl.ps_plus(strsql);
  
  sprintf(strsql,"%s %d.%d.%d %s %s %s %d",
  gettext("Дата"),dg,mg,gg,
  gettext("Документ"),nomdok,
  gettext("Слад"),skl);
  
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  
  return;
 }
float pnds=atof(row[13]);
dd=dg;
md=mg;
gd=gg;
strcpy(kpos,row[3]);
strcpy(kop,row[6]);
tipz=atoi(row[0]);
nomnn[0]='\0';
strncpy(nomnn,row[5],sizeof(nomnn)-1);
nn1[0]='\0';
strncpy(nn1,row[11],sizeof(nn1)-1);

/*Читаем наименование операции*/
if(tipz == 1)
 strcpy(bros,"Prihod");
if(tipz == 2)
 strcpy(bros,"Rashod");

mvnpp=0;
memset(naimpr,'\0',sizeof(naimpr));

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros,kop);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код операции"));
  repl.plus(" ");
  repl.plus(kop);
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);  
 }
else
 {

  mvnpp=atoi(row[1]);
  strncpy(naimpr,row[0],sizeof(naimpr)-1);
  
 }

kor=0;
memset(bros,'\0',sizeof(bros)); 
iceb_poldan("Корректор межстрочного растояния",bros,"matnast.alx",wpredok);
kor=(short)iceb_u_atof(bros);

iceb_poldan("Печатать строку для ввода доверенности",bros,"matnast.alx",wpredok);
if(iceb_u_SRAV(bros,"Вкл",1) == 0)
  metkadov=1;

char nadpis[500];
memset(nadpis,'\0',sizeof(nadpis));
iceb_poldan("Надпись на расходной накладной",nadpis,"matnast.alx",wpredok);
  
ots=0;
memset(bros,'\0',sizeof(bros)); 
iceb_poldan("Отступ от левого края",bros,"matnast.alx",wpredok);
ots=(short)iceb_u_atof(bros);

nnds=0;
memset(bros,'\0',sizeof(bros));
iceb_poldan("Код операции торговли",bros,"matnast.alx",wpredok);
if(iceb_u_proverka(bros,kop,0,1) == 0)
 nnds=1;
else
 {
  if(tipz == 2)
   {
    memset(bros,'\0',sizeof(bros));
    iceb_poldan("Форма документов с номером заказа",bros,"matnast.alx",wpredok);
    if(iceb_u_SRAV(bros,"Вкл",1) == 0)
     nnds=2;
   }
 }

memset(kdtr,'\0',sizeof(kdtr));
iceb_poldan("Код тары",kdtr,"matnast.alx",wpredok);


strcpy(kodkontr00,"00");
if(tipz == 2)
 {
  memset(bros,'\0',sizeof(bros));
  if(iceb_poldan(kop,bros,"matnast.alx",wpredok) == 0)
   {
    memset(kodkontr00,'\0',sizeof(kodkontr00));
    strncpy(kodkontr00,bros,sizeof(kodkontr00)-1);
   }
 }

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
memset(rsnds,'\0',sizeof(rsnds));

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
  strncpy(rsnds,row[17],sizeof(rsnds)-1);
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
if(iceb_u_atof(kpos) != 0. || kpos[0] != '0')
 {

  sprintf(bros,"select * from Kontragent where kodkon='%s'",kpos);
  if(sql_readkey(&bd,bros,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",
    gettext("Не найден код контрагента"),kpos);
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
    strncpy(rsnds1,row[17],sizeof(rsnds1)-1);
    
   }
 }

sortnakw(dg,mg,gg,skl,nomdok,wpredok);

sprintf(imaf1,"sn%d.tmp",getpid());
if((ff = fopen(imaf1,"r")) == NULL)
if(errno != ENOENT)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }
if(f1 == NULL)
 {
  printf("rasdokw-открываем файл\n");
  if((f1 = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return;
   }
  metka_close=1;
 }

if(ots != 0)
 fprintf(f1,"\x1b\x6C%c",ots); /*Установка левого поля*/
fprintf(f1,"\x1B\x33%c",30-kor); /*Уменьшаем межстрочный интервал*/
fprintf(f1,"\x12"); /*Отмена ужатого режима*/
fprintf(f1,"\x1B\x4D"); /*12-знаков*/
if(vr == 1)
  fprintf(f1,"\x1B\x45"); /*Включение режима выделенной печати*/


if(vr == 1)
 {
  if(nadpis[0] != '\0' && tipz == 2 && mvnpp == 0)
   fprintf(f1,"%s\n",nadpis);
  if(tipz == 2)
    fprintf(f1,gettext("\
    Поставщик                             Плательщик\n"));
  if(tipz == 1)
    fprintf(f1,gettext("\
    Плательщик                            Поставщик\n"));

  fprintf(f1,"%-*.*s N%s %-*.*s\n",
  iceb_u_kolbait(40,nmo),iceb_u_kolbait(40,nmo),nmo,
  kpos,
  iceb_u_kolbait(40,nmo1),iceb_u_kolbait(40,nmo1),nmo1);

  if(iceb_u_strlen(nmo) > 40 || iceb_u_strlen(nmo1) > 40)
    fprintf(f1,"%-*.*s %-*.*s\n",
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo)),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo)),
    iceb_u_adrsimv(40,nmo),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo1)),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo1)),
    iceb_u_adrsimv(40,nmo1));
   
 }
if(vr == 0 || vr == 2)
 {

  if(nadpis[0] != '\0' && tipz == 2 && mvnpp == 0)
   fprintf(f1,"%s\n",nadpis);
  if(tipz == 1)
   fprintf(f1,gettext("\
    Кому                                 От кого\n"));
  if(tipz == 2)
   fprintf(f1,gettext("\
     Поставщик                           Плательщик\n"));

  fprintf(f1,"%-*.*s ",iceb_u_kolbait(40,nmo),iceb_u_kolbait(40,nmo),nmo);
  fprintf(f1,"\x1B\x45N%s %-*.*s\x1B\x46\n",kpos,iceb_u_kolbait(40,nmo1),iceb_u_kolbait(40,nmo1),nmo1);

  if(iceb_u_strlen(nmo) > 40)
    fprintf(f1,"%-*.*s ",
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo)),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo)),
    iceb_u_adrsimv(40,nmo));

  if(iceb_u_strlen(nmo1) > 40)
    fprintf(f1,"\x1B\x45%-*.*s\x1B\x46\n",
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo1)),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,nmo1)),
    iceb_u_adrsimv(40,nmo1));
 }
if(vr == 3)
 {
  fprintf(f1,"%s\n",nmo);
 }

memset(bros,'\0',sizeof(bros));
memset(bros1,'\0',sizeof(bros1));
sprintf(bros,"%s %s",gettext("Адрес"),gor);
sprintf(bros1,"%s %s",gettext("Адрес"),gor1);

if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
 {
  fprintf(f1,"%-*.*s %-*.*s\n",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros,iceb_u_kolbait(40,bros1),iceb_u_kolbait(40,bros1),bros1);

  if(iceb_u_strlen(bros) > 40 || iceb_u_strlen(bros1) > 40)
   fprintf(f1,"%-*.*s %-*.*s\n",
   iceb_u_kolbait(40,iceb_u_adrsimv(40,bros)),
   iceb_u_kolbait(40,iceb_u_adrsimv(40,bros)),
   iceb_u_adrsimv(40,bros),
   iceb_u_kolbait(40,iceb_u_adrsimv(40,bros1)),
   iceb_u_kolbait(40,iceb_u_adrsimv(40,bros1)),
   iceb_u_adrsimv(40,bros1));
  
 }
sprintf(bros,"%s %s",gettext("Код ОКПО"),pkod);
sprintf(bros1,"%s %s",gettext("Код ОКПО"),pkod1);
if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
 fprintf(f1,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s %s %s",gettext("Р/С"),nsh,
gettext("МФО"),mfo);

sprintf(bros1,"%s %s %s %s",gettext("Р/С"),nsh1,
gettext("МФО"),mfo1);

if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
 fprintf(f1,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

if(rsnds[0] != '\0' || rsnds1[0] != '\0')
 {
  sprintf(bros,"%s %s","Спец счёт для НДС",rsnds);
  sprintf(bros1,"%s %s","Спец счёт для НДС",rsnds1);

  if(vr == 3)
    fprintf(f1,"%s\n",bros);
  else  
   fprintf(f1,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
 }
 
sprintf(bros,"%s %s %s %s",gettext("в"),nmb,
gettext("гор."),grb);
sprintf(bros1,"%s %s %s %s",gettext("в"),nmb1,
gettext("гор."),grb1);
if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
 {
  fprintf(f1,"%-*.*s %s\n",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros,bros1);
  if(iceb_u_strlen(bros) > 40)
   fprintf(f1,"%s\n",iceb_u_adrsimv(40,bros));
 }
sprintf(bros,"%s %s",
gettext("Налоговый номер"),inn);

sprintf(bros1,"%s %s",
gettext("Налоговый номер"),inn1);

if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
   fprintf(f1,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds);
sprintf(bros1,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds1);
if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
   fprintf(f1,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s",gettext("Телефон"),tel);
sprintf(bros1,"%s %s",gettext("Телефон"),tel1);
if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
   fprintf(f1,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

if(nnds == 2)
  fprintf(f1,"\
──────────────────────────────────────────────────────────────────────────────────\n");

fprintf(f1,"\x1B\x50"); /*10-знаков*/



/*Читаем наименование склада*/
sprintf(strsql,"select naik,fmol from Sklad where kod=%d",
skl);
memset(naiskl,'\0',sizeof(naiskl));
memset(fmoll,'\0',sizeof(fmoll));

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код склада"));
  repl.plus(" ");
  repl.plus((int)skl);
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);  
 }
else
 {
  strncpy(naiskl,row[0],sizeof(naiskl)-1);
  strncpy(fmoll,row[1],sizeof(fmoll)-1);
 }
if(vr == 0 || vr == 2 || vr == 3)
  fprintf(f1,"\n%s",naiskl);

if(mvnpp == 1) /*Внутреннее перемещение*/
 {
  if(iceb_u_atof(kpos) == 0. && iceb_u_pole(kpos,bros,2,'-') == 0 && kpos[0] == '0')
   {
    iceb_u_pole(kpos,bros,2,'-');
    skl1=(int)iceb_u_atof(bros);
    /*Читаем наименование склада*/
    sprintf(strsql,"select naik,fmol from Sklad where kod=%d",
    skl1);
    naiskl[0]='\0';
    char fmol[40];
    memset(fmol,'\0',sizeof(fmol));
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не найден код склада"));
      repl.plus(" ");
      repl.plus((int)skl1);
      repl.plus(" !");
      iceb_menu_soob(&repl,wpredok);  
     }
    else
     {
      strncpy(naiskl,row[0],sizeof(naiskl)-1);
      strncpy(fmol,row[1],sizeof(fmol)-1);
     }
    if(tipz == 1)
       fprintf(f1,"\n\
%s: %s %d <= %s %d %s\n\
%22s %s <= %s\n",
       naimpr,gettext("Склад"),skl,
       gettext("Склад"),skl1,naiskl," ",fmoll,fmol);
    if(tipz == 2)
       fprintf(f1,"\n\
%s: %s %d => %s %d %s\n\
%22s %s => %s\n",
       naimpr,gettext("Склад"),skl,
       gettext("Склад"),skl1,naiskl," ",fmoll,fmol);
   }
  else
    fprintf(f1,"\n%s",naimpr);
 }
else
 { 
  bros[0]='\0';

  if(iceb_poldan("Наименование операции в накладной",bros,"matnast.alx",wpredok) == 0)
   if(iceb_u_SRAV(bros,"Вкл",1) == 0)
    fprintf(f1,"\n%s",naimpr);
   
 }

if(nnds != 2)
  fprintf(f1,"\n");

sprintf(bros,"N%s",nomdok);  
if(tipz == 2 && iceb_poldan("Перенос символа N",bros,"matnast.alx",wpredok) == 0)
 {
  if(iceb_u_SRAV(bros,"Вкл",1) == 0)
   {
    memset(bros,'\0',sizeof(bros));
    iceb_u_pole(nomdok,bros,1,'-');
    iceb_u_pole(nomdok,bros1,2,'-');
    strcat(bros,"-N");  
    strcat(bros,bros1);  
    
   }       
  else
    sprintf(bros,"N%s",nomdok);  

 } 
memset(mesqc,'\0',sizeof(mesqc));
iceb_mesc(md,1,mesqc);

if(vr == 0)
 {
  if(tipz == 1)
    fprintf(f1,"\n\
                     %s  %s\n",
    gettext("ПРИХОДНАЯ НАКЛАДНАЯ"),bros);
  if(tipz == 2)
    fprintf(f1,"\n\
                     %s  %s\n",
    gettext("ТОВАРНАЯ НАКЛАДНАЯ"),bros);
 }

if(vr == 1)
 {
  if(nnds == 2)
    fprintf(f1,"\n\
                 %s %s %s %d %s %d%s\n",
    gettext("С ч е т"),bros,gettext("от"),
    dd,mesqc,gd,gettext("г."));
  else
    fprintf(f1,"\n\
                     %s %s\n",gettext("Счёт"),bros);
  }

if(vr == 2)
  fprintf(f1,"\n\
                     %s  %s\n",
  gettext("АКТ ПРИЁМКИ"),bros);
if(vr == 3)
  fprintf(f1,"\n\
                     %s  %s\n",
  gettext("АКТ СПИСАНИЯ"),bros);


if(nnds != 2 || vr != 1)
  fprintf(f1,"\
                      %d %s %d%s\n",dd,mesqc,gd,gettext("г."));

/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(f1,gettext("Доверенность"));
  fprintf(f1," N");
  iceb_u_pole(row[0],bros,1,'#');
  fprintf(f1," %s",bros);
  iceb_u_pole(row[0],bros,2,'#');
  fprintf(f1," %s %s",gettext("от"),bros);
 }
else
 if(metkadov == 1 && vr == 0)
  fprintf(f1,"%s N_____________________________________________",gettext("Доверенность"));

fprintf(f1,"\n");


/*Читаем через кого*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,2);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   fprintf(f1,"%s: %s\n",gettext("Через кого"),row[0]);


if(tipz == 2)
 {
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,3);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    fprintf(f1,"%s: %s\n",gettext("Основание"),row[0]);

  if(regnom[0] != '\0')
    fprintf(f1,"%s: %s\n",gettext("Регистрационный номер"),regnom);
  
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,9);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    iceb_u_rsdat(&d,&m,&g,row[0],0);
//    if(d == 0 || SRAV1(g,m,d,gd,md,dd) <= 0)
    if(d == 0 || iceb_u_sravmydat(d,m,g,dd,md,gd) >= 0)
     {
      if(vr == 0)
       fprintf(f1,"\x1B\x45%s: %s\x1B\x45\x1b\x46\n",
       gettext("Конечная дата расчета"),row[0]);
      if(vr == 1)
       fprintf(f1,"\x1B\x45%s %s\x1B\x45\x1b\x46\n",
       gettext("Счет действителен до"),row[0]);

     }
   }

  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,10);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)

  if(nomnn[0] != '\0')
    fprintf(f1,"%s: %s\n",
    gettext("Номер счет-фактуры"),nomnn);
    
 }

if(nn1[0] != '\0' && tipz == 1)
 fprintf(f1,"%s: %s\n",
 gettext("Номер встречной накладной"),nn1);
if(nn1[0] != '\0' && tipz == 2 && mvnpp > 0 )
 fprintf(f1,"%s: %s\n",
 gettext("Номер встречной накладной"),nn1);

if(vr == 1)
 {
  fprintf(f1,"\x1B\x46"); /*Выключение режима выделенной печати*/
  fprintf(f1,"\x1B\x47"); /*Включение режима двойного удара*/
 }

if(nnds == 1)
  fprintf(f1,"\x1B\x4D"); //12-знаков
if(nnds == 0)
  fprintf(f1,"\x0F"); /*Включение ужатого режима печати*/
if(nnds == 2)
  fprintf(f1,"\x0F"); /*Включение ужатого режима печати*/

klst=0;
klli=1;

sapnak(nnds,f1);

itogo=itogo1=0.;
mtpr=0;

naz:;
mnt=0;
i=0;
kol=kolt=sum=0.;
memset(eiz,'\0',sizeof(eiz));
memset(naimz,'\0',sizeof(naimz));
memset(nomn,'\0',sizeof(nomn));
kratn=cenan=kratz=cenaz=kodmz=0;
if(ff != NULL)
while(fgets(str,sizeof(str),ff) != NULL)
 {
/*  printw("%s",str);*/
  
  iceb_u_pole(str,bros,1,'|');
  kgrm=atoi(bros);
  
  iceb_u_polen(str,naim,sizeof(naim),2,'|');

  iceb_u_pole(str,bros,3,'|');
  kodm=atoi(bros);

  iceb_u_pole(str,bros,4,'|');
  nk=atoi(bros);

  iceb_u_pole(str,bros,5,'|');
  cenan=atof(bros);
  cenan=iceb_u_okrug(cenan,okrcn);
  
  iceb_u_pole(str,bros,6,'|');
  kratn=atof(bros);

  iceb_u_pole(str,ein,7,'|');

  iceb_u_pole(str,bros,8,'|');
  vtar=atoi(bros);  

  iceb_u_pole(str,shu,9,'|');

  iceb_u_pole(str,bros,10,'|');
  kolih=atof(bros);

  iceb_u_polen(str,nomz,sizeof(nomz),11,'|');
   
  if(vtar == 1)
   {
    mnt++;
    continue;
   } 

  mest=0;

  if(vtara != 0 && kgrm == vtara)
   {
    mnt++;
    continue;
   } 

  memset(bros,'\0',sizeof(bros));
  sprintf(bros,"%d",kodm);
  if(kdtr[0] != '\0' && mtpr == 0 )
    if(iceb_u_pole1(kdtr,bros,',',0,&i1) == 0 || iceb_u_SRAV(kdtr,bros,0) == 0)
     {  
      mtar++;
      continue;
     }

  if(kdtr[0] != '\0' && mtpr == 1)
    if(iceb_u_pole1(kdtr,bros,',',0,&i1) != 0)
     if(iceb_u_SRAV(kdtr,bros,0) != 0)
       continue;
   
  if(obzap == 1)
   {
    if((kodmz != 0 && kodmz != kodm) || (kratz != 0 && kratz != kratn) ||
    (cenaz != 0. && cenaz != cenan) || (eiz[0] != '\0' && iceb_u_SRAV(eiz,ein,0) != 0)\
     || (naimz[0] != '\0' && iceb_u_SRAV(naimz,naim,0) != 0))
     {
      if(nnds == 0)
       { 
        i++;
        klst++;
        fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
        i,
        iceb_u_kolbait(15,nomn),nomn,
        iceb_u_kolbait(46,naimz),iceb_u_kolbait(46,naimz),naimz,
        iceb_u_kolbait(4,eiz),iceb_u_kolbait(4,eiz),eiz,
        kol,iceb_prcn(cenaz));
        fprintf(f1," %10s",iceb_prcn(sum));
        rasdokkr(kol,kolt,kratz,f1);

        if(iceb_u_strlen(naimz) > 46)
         {
          klst++;
          fprintf(f1,"%3s %15s %s\n"," "," ",iceb_u_adrsimv(46,naimz));
         }
       }
      if(nnds == 1)
       {
        i++;
        klst++;

        fprintf(f1,"%3d %-*.*s %-*.*s %6.6g %7.2f %11s",
        i,
        iceb_u_kolbait(33,naimz),iceb_u_kolbait(33,naimz),naimz,
        iceb_u_kolbait(4,eiz),iceb_u_kolbait(4,eiz),eiz,
        kol,bb1,iceb_prcn(cenaz));

        fprintf(f1," %8s",iceb_prcn(sum));
        rasdokkr(kol,kolt,kratz,f1);

        if(iceb_u_strlen(naimz) > 33)
         {
          klst++;
          fprintf(f1,"%3s %s\n"," ",iceb_u_adrsimv(33,naimz));
         }
       }
      if(nnds == 2)
       { 
        i++;
        klst++;
        fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %5s %11s",
        i,
        iceb_u_kolbait(20,nomzz),nomzz,
        iceb_u_kolbait(46,naimz),iceb_u_kolbait(46,naimz),naimz,
        iceb_u_kolbait(4,eiz),iceb_u_kolbait(4,eiz),eiz,
        kol," ",iceb_prcn(cenaz));
        fprintf(f1," %10s\n",iceb_prcn(sum));

        if(iceb_u_strlen(naimz) > 46)
         {
          klst++;
          fprintf(f1,"%3s %20s %s\n"," "," ",iceb_u_adrsimv(46,naimz));
         }
       }
     }
    if(kodmz != kodm || kratz != kratn || cenaz != cenan || 
     iceb_u_SRAV(eiz,ein,0) != 0 || iceb_u_SRAV(naimz,naim,0) != 0)
     {
      kol=kolt=sum=0.;
      kodmz=kodm;
      cenaz=cenan;
      kratz=kratn;
      strcpy(eiz,ein);
      strcpy(naimz,naim);

     }
   }
  
  sprintf(nomn,"%d.%s.%d.%d",skl,shu,kodm,nk);

  if(obzap == 1 && kol != 0.)
     sprintf(nomn,"%d.%s.%d.***",skl,shu,kodm);
  strcpy(nomzz,nomz);
  bb=cenan*kolih;
  bb=iceb_u_okrug(bb,okrg1);
  bb1=cenan+(cenan*pnds/100.);
  bb1=iceb_u_okrug(bb1,okrg1);

  mest=0;
  if(mtpr == 0 && kratn != 0.)
   {
    mest=kolih/kratn;
    mest=iceb_u_okrug(mest,0.1);
   }
  if(obzap == 0)
   {
    strcpy(naimz,naim);
    if(nnds == 1)
     {
      i++;
      klst++;
      fprintf(f1,"%3d %-*.*s %-*.*s %6.6g %7.2f %11s",
      i,
      iceb_u_kolbait(33,naim),iceb_u_kolbait(33,naim),naim,
      iceb_u_kolbait(4,ein),iceb_u_kolbait(4,ein),ein,
      kolih,bb1,iceb_prcn(cenan));
      fprintf(f1," %8s",iceb_prcn(bb));
      rasdokkr(kol,kolt,kratz,f1);

      if(iceb_u_strlen(naim) > 33)
       {
        klst++;
        fprintf(f1,"%3s %s\n"," ",iceb_u_adrsimv(33,naim));
       }
     }

    if(nnds == 0)
     { 
      i++;
      klst++;
      fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
      i,
      iceb_u_kolbait(15,nomn),nomn,
      iceb_u_kolbait(46,naim),iceb_u_kolbait(46,naim),naim,
      iceb_u_kolbait(4,ein),iceb_u_kolbait(4,ein),ein,
      kolih,iceb_prcn(cenan));
      fprintf(f1," %10s",iceb_prcn(bb));
      rasdokkr(kol,kolt,kratz,f1);

      if(iceb_u_strlen(naim) > 46)
       {
        klst++;
        fprintf(f1,"%3s %15s %s\n"," "," ",iceb_u_adrsimv(46,naim));
       }
     }

    if(nnds == 2)
     { 
      i++;
      klst++;
      fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %5s %11s",
      i,
      iceb_u_kolbait(20,nomz),nomz,
      iceb_u_kolbait(46,naim),iceb_u_kolbait(46,naim),naim,
      iceb_u_kolbait(4,ein),iceb_u_kolbait(4,ein),ein,
      kolih," ",iceb_prcn(cenan));
      fprintf(f1," %10s\n",iceb_prcn(bb));

      if(iceb_u_strlen(naim) > 46)
       {
        klst++;
        fprintf(f1,"%3s %20s %s\n"," "," ",iceb_u_adrsimv(46,naim));
       }
     }
   }

  kol+=kolih;
  kolt+=mest;
  sum+=bb;

  itogo+=bb;

  if((klli == 1 && klst >= KLST1 ) || (klli != 1 && klst >= KLST2))
   {
    fprintf(f1,"\f");
    sapnak(nnds,f1);
    klst=0;
    klli++;
   }

 }

if(obzap == 1)
 {
  i++;
  klst++;
  if(nnds == 1)
   {
    fprintf(f1,"%3d %-*.*s %-*.*s %6.6g %7.2f %11s",
    i,
    iceb_u_kolbait(33,naimz),iceb_u_kolbait(33,naimz),naimz,
    iceb_u_kolbait(4,eiz),iceb_u_kolbait(4,eiz),eiz,
    kol,bb1,iceb_prcn(cenaz));
    fprintf(f1," %8s",iceb_prcn(sum));
    rasdokkr(kol,kolt,kratz,f1);

    if(iceb_u_strlen(naimz) > 33)
     {
      klst++;
      fprintf(f1,"%3s %s\n"," ",iceb_u_adrsimv(33,naimz));
     }
   }        
  if(nnds == 0)
   { 
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
    i,
    iceb_u_kolbait(15,nomn),nomn,
    iceb_u_kolbait(46,naimz),iceb_u_kolbait(46,naimz),naimz,
    iceb_u_kolbait(4,eiz),iceb_u_kolbait(4,eiz),eiz,
    kol,iceb_prcn(cenaz));
    fprintf(f1," %10s",iceb_prcn(sum));
    rasdokkr(kol,kolt,kratz,f1);
    if(iceb_u_strlen(naimz) > 46)
     {
      klst++;
      fprintf(f1,"%3s %15s %s\n"," "," ",iceb_u_adrsimv(46,naimz));
     }
   }
  if(nnds == 2)
   { 
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %5s %11s",
    i,
    iceb_u_kolbait(20,nomz),nomz,
    iceb_u_kolbait(46,naimz),iceb_u_kolbait(46,naimz),naimz,
    iceb_u_kolbait(4,eiz),iceb_u_kolbait(4,eiz),eiz,
    kol," ",iceb_prcn(cenaz));
    fprintf(f1," %10s\n",iceb_prcn(sum));

    if(iceb_u_strlen(naimz) > 46)
     {
      klst++;
      fprintf(f1,"%3s %20s %s\n"," "," ",iceb_u_adrsimv(46,naimz));
     }
   }
 }

mtpr++;
/*
printw("mtpr=%d kdtr=%s mtar=%d\n",mtpr,kdtr,mtar);
OSTANOV();
*/
if(mtpr == 1 && mtar != 0) /*Распечатываем отдельно тару*/
 {
  klst++;

  if(nnds == 1)
    fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - %s - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n",
    gettext("Т а р а"));

  if(nnds == 0)
    fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - %s - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n",
    gettext("Т а р а"));

  if(nnds == 2)
    fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - %s - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n",
    gettext("Т а р а"));

  rewind(ff);
  goto naz;
 }
if(ff != NULL)
  fclose(ff);
unlink(imaf1);

/*Распечатываем услуги*/
sprintf(strsql,"select * from Dokummat3 where \
datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",gd,md,dd,skl,nomdok);
/*printw("\nstrsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

char naiusl[500];

mppu=0;/*метка первого прохода услуг*/
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=iceb_u_okrug(cena,okrcn);

  memset(naiusl,'\0',sizeof(naiusl));
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[10]);
    printf("strsql=%s\n",strsql);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
       strncpy(naiusl,row1[0],sizeof(naiusl)-1);
   }  

  if(naiusl[0] == '\0')
   strncpy(naiusl,row[7],sizeof(naiusl)-1);  
  else
   {
    strcat(naiusl," ");
    strcat(naiusl,row[7]);
   }
  
  sprintf(nomn,gettext("Услуги"));
  if(kolih > 0)
    bb=cena*kolih;
  else
    bb=cena;
  
  bb=iceb_u_okrug(bb,okrg1);
  bb1=cena+(cena*pnds/100.);
  bb1=iceb_u_okrug(bb1,okrg1);

  if(nnds == 1)
   {
/*
    if(mppu == 0)
     fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - У с л у г и - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
*/
    fprintf(f1,"%3d %-33.33s %-4.4s %6.6g %7.2f %11s",
    ++i,naiusl,row[3],kolih,bb1,iceb_prcn(cena));
    fprintf(f1," %8s\n",iceb_prcn(bb));
   }
  if(nnds == 0)
   { 
/*
    if(mppu == 0)
      fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - У с л у г и - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
*/
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
    ++i,
    iceb_u_kolbait(15,nomn),nomn,
    iceb_u_kolbait(46,naiusl),iceb_u_kolbait(46,naiusl),naiusl,
    iceb_u_kolbait(4,row[3]),iceb_u_kolbait(4,row[3]),row[3],
    kolih,iceb_prcn(cena));
    fprintf(f1," %10s\n",iceb_prcn(bb));
   }
  if(nnds == 2)
   { 
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %5s %11s",
    ++i,
    iceb_u_kolbait(10,nomn),nomn,
    iceb_u_kolbait(46,naiusl),iceb_u_kolbait(46,naiusl),naiusl,
    iceb_u_kolbait(4,row[3]),iceb_u_kolbait(4,row[3]),row[3],
    kolih," ",iceb_prcn(cena));
    fprintf(f1," %10s\n",iceb_prcn(bb));
   }
  
  mppu++;
  
  itogo+=bb;

 }
 
if(nnds == 1)
 {
  fprintf(f1,"\
------------------------------------------------------------------------------------------\n");
  fprintf(f1,"\
%*s: %10s\n",iceb_u_kolbait(66,gettext("Итого")),gettext("Итого"),iceb_prcn(itogo));
  }

if(nnds == 0)
  fprintf(f1,"\
---------------------------------------------------------------------------------------------------------------------\n\
%*s: %10s\n",iceb_u_kolbait(93,gettext("Итого")),gettext("Итого"),iceb_prcn(itogo));

if(nnds == 2)
  fprintf(f1,"\
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n\
%*s: %10s\n",iceb_u_kolbait(104,gettext("Итого")),gettext("Итого"),iceb_prcn(itogo));

itogo1=itogo;

if(mvnpp == 0) /*Внешнее перемещение*/
 {

  /*Читаем сумму корректировки*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,13);
  sumkor=0.;

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    sumkor=atof(row[0]);
    sumkor=iceb_u_okrug(sumkor,0.01);
   }

  bb=bb2=bb4=0.;

  if(sumkor != 0)
   {
    if(sumkor > 0.)
      bb=sumkor*100./itogo;

    if(sumkor < 0.)
      bb=100*sumkor/(itogo-sumkor*-1);
    bb=iceb_u_okrug(bb,0.1);
    if(sumkor < 0)
      sprintf(kop,"%s %.1f%% :",gettext("Снижка"),bb*(-1));
    if(sumkor > 0)
      sprintf(kop,"%s %.1f%% :",gettext("Надбавка"),bb);

    if( nnds == 1)
      fprintf(f1,"%*s %10.2f/З %s %.2f\n",
      iceb_u_kolbait(67,kop),kop,sumkor,gettext("НДС"),sumkor+sumkor*pnds/100.);
    if( nnds == 0)
      fprintf(f1,"%*s %10.2f/З %s %.2f\n",
      iceb_u_kolbait(94,kop),kop,sumkor,gettext("НДС"),sumkor+sumkor*pnds/100.);
    if( nnds == 2)
      fprintf(f1,"%*s %10.2f/З %s %.2f\n",
      iceb_u_kolbait(95,kop),kop,sumkor,gettext("НДС"),sumkor+sumkor*pnds/100.);

    if(sumkor < 0)
      sprintf(kop,gettext("Итого со снижкой :"));
    if(sumkor > 0)
      sprintf(kop,gettext("Итого с надбавкой :"));

    if( nnds == 1)
      fprintf(f1,"%*s %10.2f\n",iceb_u_kolbait(67,kop),kop,sumkor+itogo);
    if( nnds == 0)
      fprintf(f1,"%*s %10.2f\n",iceb_u_kolbait(94,kop),kop,sumkor+itogo);
    if( nnds == 2)
      fprintf(f1,"%*s %10.2f\n",iceb_u_kolbait(105,kop),kop,sumkor+itogo);

   }

  bb=0.;

  if(lnds == 0 )
   {
   
    sprintf(kop,"%s %.2f%% :",gettext("НДС"),pnds);
    bb=(itogo+sumkor)*pnds/100.;
    if(tipz == 1) //Читаем сумму НДС введенную вручную
     {
      sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d \
and nomd='%s' and nomerz=6",gg,skl,nomdok);
      if(sql_readkey(&bd,strsql,&row,&cur) == 1)
        bb=atof(row[0]);
     }

    bb=iceb_u_okrug(bb,0.01);
    if(nnds == 1)
	 fprintf(f1,"%*s %10.2f\n",iceb_u_kolbait(67,kop),kop,bb);
    if(nnds == 0)
	 fprintf(f1,"%*s %10.2f\n",iceb_u_kolbait(94,kop),kop,bb);
    if(nnds == 2)
	 fprintf(f1,"%*s %10.2f\n",iceb_u_kolbait(105,kop),kop,bb);
   }

  sumt=0.;

  if(mnt != 0)
     vztr(dd,md,gg,skl,nomdok,&sumt,f1,nnds,wpredok);
        
  itogo1=(itogo+sumkor)+bb+sumt;
  strcpy(bros,gettext("Итого к оплате:"));
  if(tipz == 1)
    strcpy(bros,gettext("Итого:"));
  if(nnds == 1)
    fprintf(f1,"%*s %10.2f\n",iceb_u_kolbait(67,bros),bros,itogo1);
  if(nnds == 0)
    fprintf(f1,"%*s %10.2f\n",iceb_u_kolbait(94,bros),bros,itogo1);
  if(nnds == 2)
    fprintf(f1,"%*s %10.2f\n",iceb_u_kolbait(105,bros),bros,itogo1);
 }
else
 { 
  sumt=0.;
  if(mnt != 0)
     vztr(dd,md,gd,skl,nomdok,&sumt,f1,nnds,wpredok);
  itogo1+=sumt;
 }

memset(str,'\0',sizeof(str));
iceb_u_preobr(itogo1,str,0);


fprintf(f1,"%s:%s\n",gettext("Сумма прописью"),str);
if(lnds != 0 && mvnpp == 0)
 fprintf(f1,"%s\n",gettext("Без НДС"));
 
fprintf(f1,"\x1B\x50"); /*10-знаков*/
fprintf(f1,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
if(vr == 0)
 {
  fprintf(f1,"\n\%s%s\n",gettext("\
Руководитель__________________        Главный  бухгалтер_______________\n\n"),
  gettext("\
Отпустил______________________        Принял__________________________"));

  if(mvnpp == 0)
   {
    iceb_insfil("matnakend.alx",f1,wpredok);
   }
 }
 
fprintf(f1,"\x12"); /*Отмена ужатого режима печати*/
if(vr == 1)
 {

  fprintf(f1,"\n%s__________________  %s_________________\n",
  gettext("Руководитель"),
  gettext("Главный бухгалтер"));
  

  iceb_insfil("matshetend.alx",f1,wpredok);

  fprintf(f1,"\x1B\x48"); /*Выключение режима двойного удара*/
 }

if(vr == 2)
 {
  fprintf(f1,gettext("\n\
Отпустил _____________________        Принял__________________________\n\n\
Комиссия _____________________\n\
         _____________________\n\
         _____________________\n"));
 }
if(vr == 3)
 {
  fprintf(f1,gettext("\n\
Руководитель _____________________\n\n\
    Комиссия _____________________\n\
             _____________________\n\
             _____________________\n"));
 }
 
if(mvnpp == 0 && tipz == 2 && vr != 3) /*Внешнее перемещение*/
 {
  if(iceb_nalndoh(wpredok) == 0)
    fprintf(f1,"%s\n",gettext("Предприятие является плательщиком налога на доход на общих основаниях"));
//  fprintf(f1,"\n");
  if(ves != 0)
    fprintf(f1,"%s: %.2f %s\n",
    gettext("Вес"),ves,gettext("Кг."));

  fprintf(f1,"\x0E"); /*Включение режима удвоенной ширины на одну строку*/
  fprintf(f1,gettext("Благодарим за сотрудничество !"));
  fprintf(f1,"\x14"); /*Выключение режима удвоенной ширины*/
 }


iceb_podpis(f1,wpredok);
//fprintf(f1,"\n\n");
if(ots != 0)
 fprintf(f1,"\x1b\x6C%c",1); /*Установка левого поля*/
if(metka_close == 1)
 fclose(f1);

}

/*****************/
/*Распечатка тары*/
/*****************/

void vztr(short dd,short md,short gd,int skl,const char *nomdok,
double *sumt,FILE *f1,short nnds,GtkWidget *wpredok)
{
double		itogo,bb;
char		nomn[30];
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
double		kolih,cena;
int		nk,kgrm;
char		naim[512];
char		shu[20];
short		vtr;
int		i;

sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  
if(kolstr == 0)
  return;

SQLCURSOR cur1;
itogo=0.;
i=0;
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s %s %s %s %s %s %s %s %s\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],
  row[10]);
*/
  
  /*Читаем наименование материалла*/
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден код материалла"));
    repl.plus(" ");
    repl.plus(row[4]);
    repl.plus(" !");
    iceb_menu_soob(&repl,wpredok);
   }
  else
   {
    strncpy(naim,row1[0],sizeof(naim)-1);
    if(row[16][0] != '\0')
     {
      strcat(naim," ");
      strcat(naim,row[16]);
     }
   }
  kgrm=atoi(row1[1]);
  vtr=atoi(row[8]);

  if(vtr == 0. && (vtara != 0 && kgrm != vtara))
    continue;
  if(vtr == 0 && vtara == 0 )
    continue;

  nk=atoi(row[3]);
  kolih=atof(row[5]);
  cena=atof(row[6]);
  cena=iceb_u_okrug(cena,okrcn);

/*
  printw("mk2.kodm=%d %.2f vt=%d %d\n",mk2.kodm,mk2.nds,vt,mk8.kgrm);
  refresh();  
*/
  strcpy(shu,"**");
  
  if(nk != 0)
   {
    sprintf(strsql,"select shetu from Kart where sklad=%s and nomk=%d",
    row[1],nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     { 
      strncpy(shu,row1[0],sizeof(shu)-1);
     }
   }

  sprintf(nomn,"%s.%s.%s.%d",row[1],shu,row[4],nk);

  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);
  i++;
  if(i == 1)
   fprintf(f1,"%s:\n",gettext("Оборотная тара"));
  if(nnds == 1)
   {
    fprintf(f1,"%3d %-*.*s %-*.*s %6.6g %7s %11s",
    i,
    iceb_u_kolbait(33,naim),iceb_u_kolbait(33,naim),naim,
    iceb_u_kolbait(4,row[7]),iceb_u_kolbait(4,row[7]),row[7],
    kolih," ",iceb_prcn(cena));
    fprintf(f1," %8s\n",iceb_prcn(bb));
   }
  if(nnds == 0)
   { 
  
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
    i,
    iceb_u_kolbait(15,nomn),nomn,
    iceb_u_kolbait(46,naim),iceb_u_kolbait(46,naim),naim,
    iceb_u_kolbait(4,row[7]),iceb_u_kolbait(4,row[7]),row[7],
    kolih,iceb_prcn(cena));
    fprintf(f1," %10s\n",iceb_prcn(bb));
   }

  itogo+=bb;

 }
*sumt=itogo;
}

/**********************/
/*Сортировка накладной*/
/**********************/
int sortnakw(short dd,short mm,short gg,int skl,const char *nomdok,GtkWidget *wpredok)
{
FILE		*ff=NULL;
char		imaf[32];
double		krat;
char		naimat[512];
int		kgrm;
char		ei[32];
int		nk;
double		cena;
char		shu[32];
short		vtar;
double		kolih;
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
short		kodm;
char		nomz[32];
char		artikul[512];

sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gg,mm,dd,skl,nomdok);

SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr == 0)
  return(1);

sprintf(imaf,"sn%d.tmp",getpid());
if((ff = fopen(imaf,"w")) == NULL)
  iceb_er_op_fil(imaf,"",errno,wpredok);
while(cur.read_cursor(&row) != 0)
 {

  /*Читаем наименование материалла*/
  memset(naimat,'\0',sizeof(naimat));
  memset(artikul,'\0',sizeof(artikul));
  sprintf(strsql,"select naimat,krat,kodgr,artikul from Material where kodm=%s",
  row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден код материалла"));
    repl.plus(" ");
    repl.plus(row[4]);
    repl.plus(" !");
    iceb_menu_soob(&repl,wpredok);
   }
  else
   {
    strncpy(naimat,row1[0],sizeof(naimat)-1);
    if(row[16][0] != '\0')
     {
      strcat(naimat," ");
      strcat(naimat,row[16]);
     }
    strncpy(artikul,row1[3],sizeof(artikul)-1);
   }
  krat=atof(row1[1]);
  kgrm=atoi(row1[2]);

  kodm=atoi(row[4]);
  nk=atoi(row[3]);
  kolih=atof(row[5]);
  cena=atof(row[6]);
  strncpy(ei,row[7],sizeof(ei)-1);
  vtar=atoi(row[8]);
  strcpy(nomz,row[17]);
  
  strcpy(shu,"**");  
  if(nk != 0)
   {
    sprintf(strsql,"select krat,shetu,innom,nomz from Kart where sklad=%d and nomk=%d",
    skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     { 
      krat=atof(row1[0]);
      strncpy(shu,row1[1],sizeof(shu)-1);
      if(row1[2][0] != '\0')
       {
        strcat(naimat," N");
        strcat(naimat,row1[2]);
       }
      strcpy(nomz,row1[3]);
     }
   }

  fprintf(ff,"%d|%s|%d|%d|%.10g|%.5g|%s|%d|%s|%.10g|%s|%s|\n",
  kgrm,naimat,kodm,nk,cena,krat,ei,vtar,shu,kolih,nomz,artikul);

 }

fclose(ff);

if(srtnk == 1)
 {
  /*Сортировка файла*/
  memset(strsql,'\0',sizeof(strsql));
//  sprintf(strsql,"sort -o %s -t\\| +0n +1b %s",imaf,imaf);
  sprintf(strsql,"sort -o %s -t\\| -k1,2n -k2,3b %s",imaf,imaf);
  system(strsql);
 }
return(0);
}

/************************/
/*Выдача шапки накладной*/
/************************/
void		sapnak(short nnds,FILE *f1)
{
if(nnds == 1)
 {
  fprintf(f1,"\
------------------------------------------------------------------------------------------\n");

  fprintf(f1,gettext(" N |    Наименование продукции       |Ед. |Колич-|Ц е н а|  Ц е н а  | Сумма  |Кол.|Крат-|\n"));
  fprintf(f1,gettext("   |           (товара)              |изм.|ество | с НДС |  без НДС  |без НДС |м-ст|ность|\n"));
  fprintf(f1,"\
------------------------------------------------------------------------------------------\n");
 }

if(nnds == 0)
 {

  fprintf(f1,"\
---------------------------------------------------------------------------------------------------------------------\n");

  fprintf(f1,gettext(" N | Номенклатурный|       Наименование продукции                 |Ед. |Количество|  Ц е н а  | С у м м а|Кол.|Крат-|\n"));
  fprintf(f1,gettext("   |    номер      |              (товара)                        |изм.|          |           |          |м-ст|ность|\n"));

  fprintf(f1,"\
---------------------------------------------------------------------------------------------------------------------\n");
 }

if(nnds == 2)
 {

  fprintf(f1,"\
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");

  fprintf(f1,gettext(" N |   Номер заказа     |       Наименование продукции                 |Ед. |Количество|Факт.|  Ц е н а  | С у м м а|\n"));
  fprintf(f1,gettext("   |                    |              (товару)                        |изм.|          |кол. |           |          |\n"));

  fprintf(f1,"\
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");
 }
}
/**********************************/
/*Вывод количество мест           */
/**********************************/
void	rasdokkr(double kol,double kolt,double kratz,FILE *ff)
{
int		dop=0;
double		ikolt=0.;
char		bros[512];
if(kolt != 0.)
 {
  modf(kolt,&ikolt);
  dop=(int)(kol-ikolt*kratz);
  if(dop != 0)
   sprintf(bros,"%.f/%d",ikolt,dop);
  else
   sprintf(bros,"%.f",ikolt);

  fprintf(ff," %-4s %4.4g\n",bros,kratz);
 }
else
  fprintf(ff,"\n");
  
}
