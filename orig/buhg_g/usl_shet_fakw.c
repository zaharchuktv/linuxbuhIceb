/*$Id: usl_shet_fakw.c,v 1.12 2011-02-21 07:35:58 sasa Exp $*/
/*14.12.2010	22.02.2005	Белых А.И.	usl_shet_fakw.c
Распечатка счёта фактуры для подсистемы "Учёт услуг"
Если вернули 0-сделали докумет
             1-нет
*/
#include <stdlib.h>
#include        <errno.h>
#include	"buhg_g.h"
#include <unistd.h>

int usl_shet_fak_sap(short dd,short md,short gd,const char*,const char *ras_shet,const char *mfo,const char *kod,const char *naim_kontr,const char *dover,const char *herez,FILE *ff,GtkWidget*);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern SQL_baza bd;

int usl_shet_fakw(short dg,short mg,short gg, //Дата документа
int podr, //Подразделение
const char *nomdok, //Номер документа
iceb_u_str *imafil, //имя файла с распечаткой
int tipz,  //1-приход 2-расход
GtkWidget *wpredok)
{
char            nmo[512],nmo1[512];  /*Наименование организации*/
char            gor[512],gor1[512]; /*Адрес*/
char            pkod[20],pkod1[20]; //Код ЕДРПУО
char            nmb[512],nmb1[512];  /*Наименование банка*/
char            mfo[32],mfo1[32];  /*МФО*/
char            nsh[32],nsh1[32];  /*Номер счета*/
char            inn[32],inn1[32];  /*Индивидуальный налоговый номер*/
char		grb[512],grb1[512];  /*Город банка*/
char		npnds[32],npnds1[32]; /*Номер плательщика НДС*/
char		rsnds[32],rsnds1[32]; //спец счёт для НДС
char		tel[112],tel1[112];    /*Номер телефона*/
char		rnhp[40];           //Регистрационный номер частного предпринимателя
char		strsql[1024];
SQL_str row;
SQLCURSOR cur;
int		kolstr;
char		kodop[32];
char		kontr[32];
char		datop[16];
double sumkor;
char		osnov[512];
iceb_u_str naimnal;
float		procent=0.;
char		kodkontr00[32];
char bros[512];
char		regnom[32];
short lnds=0; //Льготы по НДС
char dover[512];
char sherez[512];
short		mvnpp;
float pnds=0.;

/*Читаем шапку документа*/
sprintf(strsql,"select kontr,kodop,sumkor,datop,osnov,nds,dover,sherez,pn from Usldokum \
where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
gg,mg,dg,podr,nomdok,tipz);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;

  sprintf(strsql,"usl_shet_fakw-%s !",gettext("Не найден документ"));

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
  return(1);
 }
pnds=atof(row[8]);
strncpy(kontr,row[0],sizeof(kontr)-1);
strncpy(kodop,row[1],sizeof(kodop)-1);
sumkor=atof(row[2]);

memset(datop,'\0',sizeof(datop));
strncpy(datop,row[3],sizeof(datop)-1);

memset(osnov,'\0',sizeof(osnov));
strncpy(osnov,row[4],sizeof(osnov)-1);
lnds=atoi(row[5]);

strncpy(dover,row[6],sizeof(dover)-1);

strncpy(sherez,row[7],sizeof(sherez)-1);

naimnal.new_plus("");
usldopnalw(tipz,kodop,&procent,&naimnal);

int kol_str_ots=0;
//читаем количество сторок отступа для логотипа

if(iceb_poldan("Количество строк отступа для счёта с логотипом",bros,"uslnast.alx",wpredok ) == 0)
  kol_str_ots=atoi(bros);
  
/*Читаем наименование операции*/
if(tipz == 1)
 strcpy(bros,"Usloper1");
if(tipz == 2)
 strcpy(bros,"Usloper2");

mvnpp=0;

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros,kodop);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код операции"),kodop);
  iceb_menu_soob(strsql,wpredok);
 }
else
 {

  mvnpp=atoi(row[1]);
  
 }

strcpy(kodkontr00,"00");
if(tipz == 2)
 {
  memset(bros,'\0',sizeof(bros));
  if(iceb_poldan(kodop,bros,"uslnast.alx",wpredok) == 0)
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
  strncpy(rnhp,row[15],sizeof(rnhp)-1);
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
if(iceb_u_atof(kontr) != 0. || kontr[0] != '0')
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
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
  return(1);

sprintf(strsql,"shfk%d.lst",getpid());
imafil->new_plus(strsql);
FILE *ff;

if((ff = fopen(imafil->ravno(),"w")) == NULL)
 {
  iceb_er_op_fil(imafil->ravno(),"",errno,wpredok);
  return(1);
 }
iceb_u_startfil(ff);

for(int i=0; i < kol_str_ots; i++)
 fprintf(ff,"\n");

fprintf(ff,"\x1b\x6C%c",10); /*Установка левого поля*/

if(tipz == 1)
if(usl_shet_fak_sap(dg,mg,gg,nomdok,nsh1,mfo1,pkod1,nmo,dover,sherez,ff,wpredok) != 0)
 {
  fclose(ff);
  unlink(imafil->ravno());
  return(1);
 }

if(tipz == 2)
if(usl_shet_fak_sap(dg,mg,gg,nomdok,nsh,mfo,pkod,nmo1,dover,sherez,ff,wpredok) != 0)
 {
  fclose(ff);
  unlink(imafil->ravno());
  return(1);
 }


SQLCURSOR curr;
SQL_str row1;
int metka;
int kodzap;
double kolih;
double cena;
double cenasnd;
double itogo=0.;
double suma;
char naim[512];
int nom_str=0;
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
  fprintf(ff," %3d %-*.*s %6.6g %-*.*s %8.8g %10.2f\n",
  ++nom_str,
  iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,
  kolih,
  iceb_u_kolbait(6,row[4]),iceb_u_kolbait(6,row[4]),row[4],
  cena,suma);

  if(iceb_u_strlen(naim) > 30)
     fprintf(ff,"%3s %s\n"," ",iceb_u_adrsimv(30,naim));
 }
fprintf(ff,"\
---------------------------------------------------------------------\n");
int dlina=58;
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(dlina,gettext("Итого")),gettext("Итого"),itogo);

if(mvnpp == 0) /*Внешнее перемещение*/
 {


  double bb=0.;

  if(sumkor != 0)
   {
    bb=sumkor*100./itogo;
    bb=iceb_u_okrug(bb,0.01);
    if(sumkor < 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Снижка"),bb*(-1));
    if(sumkor > 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Надбавка"),bb);

    fprintf(ff,"%*s%10.2f/З %s %.2f\n",
    iceb_u_kolbait(dlina+1,strsql),strsql,sumkor,gettext("НДС"),sumkor+sumkor*pnds/100.);

    if(sumkor < 0)
      sprintf(strsql,gettext("Итого со снижкой :"));
    if(sumkor > 0)
      sprintf(strsql,gettext("Итого с надбавкой :"));

    fprintf(ff,"%*s%10.2f\n",iceb_u_kolbait(dlina+1,strsql),strsql,sumkor+itogo);

   }

  double bbproc=0.;
  if(procent != 0.)
   {
    sprintf(strsql,"%s %.2f%% :",naimnal.ravno(),procent);
    bbproc=(itogo+sumkor)*procent/100.;
    bbproc=iceb_u_okrug(bbproc,0.01);
    fprintf(ff,"%*s%10.2f\n",iceb_u_kolbait(dlina+1,strsql),strsql,bbproc);

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
    fprintf(ff,"%*s%10.2f\n",iceb_u_kolbait(dlina+1,strsql),strsql,bb);
   }

  double itogonds=(itogo+sumkor)+bb+ bbproc;
  strcpy(bros,gettext("Итого к оплате:"));
  if(tipz == 1)
    strcpy(bros,gettext("Итого"));
  fprintf(ff,"%*s%10.2f\n",iceb_u_kolbait(dlina+1,bros),bros,itogonds);
 }

fprintf(ff,"\n%*s______________________\n\n",iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"));
fprintf(ff,"%*s______________________\n",iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"));


iceb_insfil("usl_shet_fak_end.alx",ff,wpredok);


iceb_podpis(ff,wpredok);
fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/


fclose(ff);
return(0);

}
/*******************************/
/*Формирование шапки документа*/
/*******************************/

int usl_shet_fak_sap(short dd,short md,short gd,
const char *nomdok,
const char *ras_shet,
const char *mfo,
const char *kod,
const char *naim_kontr,
const char *dover,
const char *herez,
FILE *ff,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"usl_shet_fak.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
char stroka[1024];
char bros[108];
int nomer_str=0;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  nomer_str++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch(nomer_str)
   {
    case 2:
     iceb_u_bstab(stroka,ras_shet,15,68,1);
     break;

    case 3:
     iceb_u_bstab(stroka,mfo,15,68,1);
     break;

    case 4:
     iceb_u_bstab(stroka,kod,15,68,1);
     break;

    case 6:
     iceb_u_bstab(stroka,nomdok,21,40,1);
     sprintf(bros,"%02d.%02d.%d",dd,md,gd);     
     iceb_u_bstab(stroka,bros,58,70,1);
     break;

    case 8:
     iceb_u_bstab(stroka,naim_kontr,12,68,1);
     break;

    case 11:
     iceb_u_bstab(stroka,dover,16,28,1);
     iceb_u_bstab(stroka,herez,34,68,1);
     break;
     
   }
  fprintf(ff,"%s",stroka);
 }
return(0);
}
