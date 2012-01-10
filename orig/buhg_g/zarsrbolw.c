/*$Id: zarsrbolw.c,v 1.18 2011-04-14 16:09:36 sasa Exp $*/
/*20.11.2009	20.11.2001	Белых А.И.	zarsrbolw.c
Расчет средней зароботной платы для больничного
*/
#include <stdlib.h>
#include        <errno.h>
#include        <math.h>
#include        <time.h>
#include        "buhg_g.h"
#include <unistd.h>
#include "zar_sprav.h"

void zarsrbol_rn(char *kodotp,char *kodgos,char *kodtabotrv,char *kodtabotp,int *metkaknvr,char*,class iceb_u_str*,GtkWidget*);

extern char     *organ;
extern char	*shetb; /*Бюджетные счета начислений*/
extern short    *kodbl;  /*Код начисления больничного*/
extern SQL_baza bd;

void zarsrbolw(class zar_sprav_rek *data,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
char		strsql[512];
SQL_str		row,row1;
int		kolstr=0;
int		kolmes=0; //Количество месяцев в периоде
double		suma=0.;
short		kkk;
char		imaf[40],imafprot[40];
FILE		*ff,*ffprot;
char		kodotp[312]; //Коды отпускных
char		kodgos[312]; //Коды гос.обязанности
char		kodtabotrv[312]; //Коды табеля отработанного времени
char		kodtabotp[312]; //Коды табеля отпускных
char kod_ed_viplat[312];
short		knah=0;
short		dr,mr,gr;
short		metkab=0; //0-не бюджет 1 бюджет

short mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,data->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->datak.ravno());
short dn=1;
kolmes=iceb_u_period(1,mn,gn,1,mk,gk,1);
if(kolmes == 0)
 {
  iceb_menu_soob(gettext("Количество месяцев в периоде равно нолю !"),wpredok);
  return;
 }


sprintf(strsql,"%s %d.%d %s %d.%d %s %s\n",
gettext("Период с"),mn,gn,
gettext("до"),mk,gk,data->tabnom.ravno(),data->fio.ravno());

iceb_printw(iceb_u_toutf(strsql),buffer,view);

sprintf(strsql,"%s:%d\n",gettext("Количество месяцев"),kolmes);

iceb_printw(iceb_u_toutf(strsql),buffer,view);

double nash[kolmes]; //Всего начислено
double otpusk[kolmes]; //Отпускные
double gosob[kolmes]; //Оплата гособязанности
int    kolrd[kolmes]; //Количество отработанных дней
double kolrh[kolmes]; //Количество отработанных часов
int    kolotp[kolmes]; //Количество отпускных
int kolddpev[kolmes];

double sum_ed_viplat[kolmes]; //Суммы единоразовых выплат
double sum_ed_viplat_b[kolmes]; //Суммы единоразовых выплат - бюджет
double sum_ed_viplat_h[kolmes]; //Суммы единоразовых выплат - хозрасчёт

float kolrd_mes[kolmes];  //Количество рабочих дней в месяце по календарю
float kolrh_mes[kolmes]; //Количесво рабочих часов в месяце

double bnash[kolmes]; //Всего начислено (бюджет)
double botpusk[kolmes]; //Отпускные  (бюджет)
double bgosob[kolmes]; //Оплата гособязанности  (бюджет)

double nbnash[kolmes]; //Всего начислено  (небюджет)
double nbotpusk[kolmes]; //Отпускные  (небюджет)
double nbgosob[kolmes]; //Оплата гособязанности  (небюджет)

double metka_bol[kolmes]; //Метка больничного общая
double metka_bol_hoz[kolmes]; //Метка больничного для хозрасчёта
double metka_bol_bd[kolmes]; //Метка больничного для бюджета

memset(metka_bol,'\0',sizeof(metka_bol));
memset(metka_bol_hoz,'\0',sizeof(metka_bol_hoz));
memset(metka_bol_bd,'\0',sizeof(metka_bol_bd));

memset(kolrd_mes,'\0',sizeof(kolrd_mes));
memset(kolrh_mes,'\0',sizeof(kolrh_mes));
memset(kolddpev,'\0',sizeof(kolddpev));

memset(sum_ed_viplat,'\0',sizeof(sum_ed_viplat));
memset(sum_ed_viplat_b,'\0',sizeof(sum_ed_viplat_b));
memset(sum_ed_viplat_h,'\0',sizeof(sum_ed_viplat_h));

memset(nash,'\0',sizeof(nash));
memset(otpusk,'\0',sizeof(otpusk));
memset(gosob,'\0',sizeof(gosob));
memset(kolrd,'\0',sizeof(kolrd));
memset(kolrh,'\0',sizeof(kolrh));
memset(kolotp,'\0',sizeof(kolotp));
memset(kod_ed_viplat,'\0',sizeof(kod_ed_viplat));

memset(bnash,'\0',sizeof(bnash));
memset(botpusk,'\0',sizeof(botpusk));
memset(bgosob,'\0',sizeof(bgosob));

memset(nbnash,'\0',sizeof(nbnash));
memset(nbotpusk,'\0',sizeof(nbotpusk));
memset(nbgosob,'\0',sizeof(nbgosob));

memset(kodgos,'\0',sizeof(kodgos));
memset(kodotp,'\0',sizeof(kodotp));
memset(kodtabotrv,'\0',sizeof(kodtabotrv));
memset(kodtabotp,'\0',sizeof(kodtabotp));

int metkaknvr=0;
class iceb_u_str kodtdpev("");
//Читаем настройки
zarsrbol_rn(kodotp,kodgos,kodtabotrv,kodtabotp,&metkaknvr,kod_ed_viplat,&kodtdpev,wpredok);

//Читаем календарь за период и заполняем массивы количества рабочих часов и дней
short d=1,m=mn,g=gn;
//short kol_kd=0; //количество календарных дней
for(int shmes=0; shmes < kolmes ; shmes++)
 {
  redkalw(m,g,&kolrd_mes[shmes],&kolrh_mes[shmes],wpredok);
//  printw("%f %f\n",kolrd_mes[shmes],kolrh_mes[shmes]);
  iceb_u_dpm(&d,&m,&g,3);
 }

sprintf(imaf,"sz%s_%d.lst",data->tabnom.ravno(),getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

sprintf(imafprot,"szp%s_%d.lst",data->tabnom.ravno(),getpid());
if((ffprot = fopen(imafprot,"w")) == NULL)
 {
  iceb_er_op_fil(imafprot,"",errno,wpredok);
  return;
 }
 
iceb_u_zagolov(gettext("Расчет средней зарплаты для начисления больничного"),dn,mn,gn,d,mk,gk,organ,ff);
fprintf(ff,"%s %s\n",data->tabnom.ravno(),data->fio.ravno());
fprintf(ff,"%s:%d\n",gettext("Количество месяцев"),kolmes);

if(kodgos[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Коды начислений не вошедшие в расчет"),kodgos);
  int klst=0;
  if(metkaknvr == 1)
   iceb_printcod(ff,"Nash","kod","naik",0,kodgos,&klst);
 }
if(kod_ed_viplat[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Коды единоразовых выплат"),kod_ed_viplat);
if(kodtdpev.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Коды табеля для пересчёта одноразовых выплат"),kodtdpev.ravno());

fprintf(ffprot,"\x1b\x6C%c",5); /*Установка левого поля*/
iceb_u_zagolov(gettext("Расчет средней зарплаты для начисления больничного"),dn,mn,gn,d,mk,gk,organ,ffprot);
fprintf(ffprot,"%s %s\n",data->tabnom.ravno(),data->fio.ravno());
fprintf(ffprot,"%s:%d\n",gettext("Количество месяцев"),kolmes);

SQLCURSOR cur;
SQLCURSOR curr;

dr=1; mr=mn; gr=gn;
for(kkk=0; kkk < kolmes; kkk++)
 {
  fprintf(ffprot,"\n%d.%d\n",mr,gr);
  sprintf(strsql,"select knah,suma,shet from Zarp where tabn=%s and \
datz >= '%d-%d-01' and datz <= '%d-%d-31' and prn='1' and suma <> 0.",
  data->tabnom.ravno(),gr,mr,gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }
  if(kolstr != 0)
   {
    while(cur.read_cursor(&row) != 0)
     {
      knah=atoi(row[0]);
      suma=atof(row[1]);    


      sprintf(strsql,"select naik from Nash where kod=%d",knah);
      sql_readkey(&bd,strsql,&row1,&curr);
      
      fprintf(ffprot,"%2d %-40s %8.2f\n",knah,row1[0],suma);
             

      metkab=0;
      if(iceb_u_proverka(shetb,row[2],0,1) == 0)
        metkab=1;

      if(iceb_u_proverka(kodotp,row[0],0,1) == 0)
       {
        fprintf(ffprot,"%s !\n",gettext("Отпускные"));      
        otpusk[kkk]+=suma;
        if(metkab == 1)
          botpusk[kkk]+=suma;
        else        
          nbotpusk[kkk]+=suma;
       }
      if(iceb_u_proverka(kod_ed_viplat,row[0],0,1) == 0) //коды единоразовых выплат
       {
        sum_ed_viplat[kkk]+=suma;
        if(metkab == 1)
         sum_ed_viplat_b[kkk]+=suma;
        else
         sum_ed_viplat_h[kkk]+=suma;
       }

      if(iceb_u_proverka(kodgos,row[0],0,1) == 0) //коды не входящие в расчёт
       {
        gosob[kkk]+=suma;
        if(metkab == 1)
         bgosob[kkk]+=suma;
        else
         nbgosob[kkk]+=suma;
       }

      nash[kkk]+=suma;    
      if(metkab == 1)
       {
        bnash[kkk]+=suma;    
       }
      else      
       {
        nbnash[kkk]+=suma;    
       }
     }

    fprintf(ffprot,"%*s:%8.2f\n",iceb_u_kolbait(33,gettext("Итого")),gettext("Итого"),nash[kkk]);
   }  
  //Узнаем количество отработанных дней и количество дней в отпуске
  sprintf(strsql,"select kodt,dnei,kdnei,has from Ztab where \
tabn=%s and god=%d and mes=%d",data->tabnom.ravno(),gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   { 
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }
  while(cur.read_cursor(&row) != 0)
   {
    sprintf(strsql,"select naik from Tabel where kod=%s",row[0]);
    sql_readkey(&bd,strsql,&row1,&curr);
    fprintf(ffprot,"%-2s %-*s %2s %2s\n",row[0],iceb_u_kolbait(30,row1[0]),row1[0],row[1],row[2]);

    if(iceb_u_proverka(kodtabotrv,row[0],0,1) == 0)
     {
      kolrd[kkk]+=atoi(row[1]);
      kolrh[kkk]+=atof(row[3]);
     }
    if(iceb_u_proverka(kodtabotp,row[0],0,1) == 0)
      kolotp[kkk]+=atoi(row[2]);
    if(iceb_u_proverka(kodtdpev.ravno(),row[0],0,1) == 0)
      kolddpev[kkk]+=atoi(row[1]);
   }  
  iceb_u_dpm(&dr,&mr,&gr,3);
 }

/*смотрим есть ли больничные в счёт месяцев расчёта*/
fprintf(ffprot,"Определяем больничные в счёт месяцев расчёта\n");
dr=1; mr=mn; gr=gn;
for(kkk=0; kkk < kolmes; kkk++)
 {
  fprintf(ffprot,"\n%d.%d\n",mr,gr);
  sprintf(strsql,"select knah,suma,shet,datz from Zarp where tabn=%s and \
godn=%d and mesn = %d and prn='1' and suma <> 0.",data->tabnom.ravno(),gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }
  if(kolstr != 0)
   {
    while(cur.read_cursor(&row) != 0)
     {
      if(provkodw(kodbl,row[0]) < 0)
        continue;
      fprintf(ffprot,"%s %2s %10s %*s\n",row[3],row[0],row[1],iceb_u_kolbait(6,row[2]),row[2]);
      metka_bol[kkk]=1;

      metkab=0;
      if(iceb_u_proverka(shetb,row[2],0,1) == 0)
        metkab=1;

      if(metkab == 1)
       {
        metka_bol_bd[kkk]=metka_bol[kkk];
       }
      else      
       {
        metka_bol_hoz[kkk]=metka_bol[kkk];
       }

     }
   }
  iceb_u_dpm(&dr,&mr,&gr,3);
 }


//Распечатываем результат


fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Дата  |      Количество дней      |Начисленная заработная  плата|Суммы ед|В расчёт|Берем в |\n\
       | Отраб.   |Отпуск.|В месяце| Всего    |Отпускные|Невошло |ино.раз.|с ед.раз| расчет |\n\
       |          |       |        |          |         |        |выплат  |выплат  |        |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");

int iotr=0;
int iotr_mes=0;
double ioth=0;
double ioth_mes=0;
int iotp=0;
double isumn=0.;
double iotpn=0.;
double igos=0.;
double ivras=0.;
double vras=0;
double ivras_h=0.;
double vras_h=0;
dr=1; mr=mn; gr=gn;
char metka_rd[5];
char metka_rh[5];
double suma_ed_vras;
double isuma_ed_vras=0;
double isum_ed_viplat=0;
short dnr=1,mnr=mn,gnr=gn;
char boll[64];
class zar_read_tn1h nastr;

for(kkk=0; kkk < kolmes; kkk++)
 {
  suma_ed_vras=sum_ed_viplat[kkk];
  if(sum_ed_viplat[kkk] != 0.)
   {
    fprintf(ffprot,"Есть сумма единоразовой выплаты %.2f в %d месяце\n\
Количество рабочих дней в месяце:%f\n",sum_ed_viplat[kkk],kkk,kolrd_mes[kkk]);
  /*Если нет больничного берётся вся единорaзовая выплата*/

    if(kolrd_mes[kkk] != kolddpev[kkk])
     {
      float kold=kolddpev[kkk];
      if(kolddpev[kkk] > kolrd_mes[kkk])
          kold=kolrd_mes[kkk];
      suma_ed_vras=sum_ed_viplat[kkk]/kolrd_mes[kkk]*kold;
      fprintf(ffprot,"Пересчитываем единоразовую выплату=%.2f/%f*%f=%.2f\n",
      sum_ed_viplat[kkk],kolrd_mes[kkk],kold,suma_ed_vras);
     }
   }
  vras_h=vras=nash[kkk]-otpusk[kkk]-gosob[kkk]-sum_ed_viplat[kkk]+suma_ed_vras;
  if(vras < 0.)
   vras_h=vras=0.;
   
  //Читаем текущие настройки
  zar_read_tn1w(1,mr,gr,&nastr,NULL,wpredok);
  iceb_u_dpm(&dnr,&mnr,&gnr,3);
  
  if(vras > nastr.max_sum_for_soc)
    vras_h=vras=nastr.max_sum_for_soc;
  
  memset(metka_rd,'\0',sizeof(metka_rd));
  memset(metka_rh,'\0',sizeof(metka_rh));
  metka_rh[0]=' ';    
  metka_rd[0]=' ';    
  if(kolrd[kkk] < kolrd_mes[kkk])
    metka_rd[0]='*';
    
  if(kolrh[kkk] < kolrh_mes[kkk])
    metka_rh[0]='*';
  
  memset(boll,'\0',sizeof(boll));
  if(metka_bol[kkk] == 1)
   strcpy(boll,gettext("Больничный")); 
  
  fprintf(ff,"%02d.%d %-3d%s/%4.f%s %-7d %-3.f/%4.f %10.2f %9.2f %8.2f %8.2f %8.2f %8.2f %s\n",
  mr,gr,kolrd[kkk],metka_rd,kolrh[kkk],metka_rh,kolotp[kkk],kolrd_mes[kkk],kolrh_mes[kkk],nash[kkk],otpusk[kkk],gosob[kkk],
  sum_ed_viplat[kkk],suma_ed_vras,
  vras,boll);
  
  iotr+=kolrd[kkk];
  ioth+=kolrh[kkk];
  iotr_mes+=(int)kolrd_mes[kkk];
  ioth_mes+=kolrh_mes[kkk];
  iotp+=kolotp[kkk];
  isumn+=nash[kkk];
  iotpn+=otpusk[kkk];
  igos+=gosob[kkk];
  ivras+=vras;
  ivras_h+=vras_h;
  isuma_ed_vras+=suma_ed_vras;
  isum_ed_viplat+=sum_ed_viplat[kkk];
  
  iceb_u_dpm(&dr,&mr,&gr,3);
 }

if(iotr != 0)
 {
  sprintf(strsql,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),ivras,iotr,ivras/iotr);
  iceb_printw(iceb_u_toutf(strsql),buffer,view);
 }
else
 {
  sprintf(strsql,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),ivras,iotr,0.);
  iceb_printw(iceb_u_toutf(strsql),buffer,view);
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");

fprintf(ff,"%-*.*s %-3d /%4.f  %-7d %-3d/%4.f %10.2f %9.2f %8.2f %8.2f %8.2f %8.2f\n",
iceb_u_kolbait(7,gettext("Итого")),iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),
iotr,ioth,iotp,iotr_mes,ioth_mes,isumn,iotpn,igos,
isum_ed_viplat,isuma_ed_vras,
ivras);

if(iotr != 0)
 fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
 ivras,iotr,ivras/iotr);
else
 fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
 ivras,iotr,0.);

if(ioth != 0)
 fprintf(ff,"%s: %.2f/%.1f=%.2f\n",gettext("Средняя зарплата"),
 ivras_h,ioth,ivras/ioth);
else
 fprintf(ff,"%s: %.2f/%.1f=%.2f\n",gettext("Средняя зарплата"),
 ivras_h,ioth,0.);

if(shetb != NULL)
 {
  isum_ed_viplat=0;
  isuma_ed_vras=0;
  iotr=0;
  iotp=0;
  isumn=0.;
  iotpn=0.;
  igos=0.;
  ivras=0.;
  vras=0;
  
  sprintf(strsql,"%s\n",gettext("Бюджет"));
  iceb_printw(iceb_u_toutf(strsql),buffer,view);
  
  fprintf(ff,"\n%s\n",gettext("Бюджет"));

fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Дата  |      Количество дней      |Начисленная заработная  плата|Суммы ед|В расчёт|Берем в |\n\
       | Отраб.   |Отпуск.|В месяце| Всего    |Отпускные|Невошло |ино.раз.|с ед.раз| расчет |\n\
       |          |       |        |          |         |        |выплат  |выплат  |        |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");

  dr=1; mr=mn; gr=gn;
  for(kkk=0; kkk < kolmes; kkk++)
   {
    suma_ed_vras=sum_ed_viplat_b[kkk];
    if(suma_ed_vras != 0.)
    if(kolrd_mes[kkk] != kolddpev[kkk])
     {
      float kold=kolddpev[kkk];
      if(kolddpev[kkk] > kolrd_mes[kkk])
        kold=kolrd_mes[kkk];
      suma_ed_vras=sum_ed_viplat_b[kkk]/kolrd_mes[kkk]*kold;
     }


    vras_h=vras=bnash[kkk]-botpusk[kkk]-bgosob[kkk]-sum_ed_viplat_b[kkk]+suma_ed_vras;
    if(vras < 0.)
     vras_h=vras=0.;

    zar_read_tn1w(1,mr,gr,&nastr,NULL,wpredok);
    if(vras > nastr.max_sum_for_soc)
      vras_h=vras=nastr.max_sum_for_soc;

    memset(metka_rd,'\0',sizeof(metka_rd));
    memset(metka_rh,'\0',sizeof(metka_rh));
      
    metka_rd[0]=' ';    
    metka_rh[0]=' ';    
    if(kolrd[kkk] < kolrd_mes[kkk])
      metka_rd[0]='*';
      
    if(kolrh[kkk] < kolrh_mes[kkk])
      metka_rh[0]='*';

    memset(boll,'\0',sizeof(boll));
    if(metka_bol[kkk] == 1)
     strcpy(boll,gettext("Больничный")); 
      
    fprintf(ff,"%02d.%d %-8d%s %-7d %-3.f/%4.f %10.2f %9.2f %8.2f %8.2f %8.2f %8.2f %s\n",
    mr,gr,kolrd[kkk],metka_rd,kolotp[kkk],kolrd_mes[kkk],kolrh_mes[kkk],bnash[kkk],botpusk[kkk],bgosob[kkk],
    sum_ed_viplat[kkk],suma_ed_vras,
    vras,boll);

    iotr+=kolrd[kkk];
    ioth+=kolrh[kkk];
    iotp+=kolotp[kkk];
    isumn+=bnash[kkk];
    iotpn+=botpusk[kkk];
    igos+=bgosob[kkk];
    ivras+=vras;
    isuma_ed_vras+=suma_ed_vras;
    isum_ed_viplat+=sum_ed_viplat_b[kkk];
    iceb_u_dpm(&dr,&mr,&gr,3);
   }

  if(iotr != 0)
   {
    sprintf(strsql,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
    ivras,iotr,ivras/iotr);
    iceb_printw(iceb_u_toutf(strsql),buffer,view);
   }
  else
   {
    sprintf(strsql,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
    ivras,iotr,0.);
    iceb_printw(iceb_u_toutf(strsql),buffer,view);
   }

fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");

  fprintf(ff,"%-*.*s %-3d /%4.f  %-7d %-3d/%4.f %10.2f %9.2f %8.2f %8.2f %8.2f %8.2f\n",
  iceb_u_kolbait(7,gettext("Итого")),iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),
  iotr,ioth,iotp,iotr_mes,ioth_mes,isumn,iotpn,igos,
  isum_ed_viplat,isuma_ed_vras,
  ivras);


  if(iotr != 0)
   fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
   ivras,iotr,ivras/iotr);
  else
   fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
   ivras,iotr,0.);


  isum_ed_viplat=0;
  isuma_ed_vras=0;
  iotr=0;
  iotp=0;
  isumn=0.;
  iotpn=0.;
  igos=0.;
  ivras=0.;
  vras=0;

  sprintf(strsql,"%s\n",gettext("Хозрасчёт"));
  iceb_printw(iceb_u_toutf(strsql),buffer,view);

  fprintf(ff,"\n%s\n",gettext("Хозрасчёт"));

fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Дата  |      Количество дней      |Начисленная заработная  плата|Суммы ед|В расчёт|Берем в |\n\
       | Отраб.   |Отпуск.|В месяце| Всего    |Отпускные|Невошло |ино.раз.|с ед.раз| расчет |\n\
       |          |       |        |          |         |        |выплат  |выплат  |        |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");

  dr=1; mr=mn; gr=gn;
  for(kkk=0; kkk < kolmes; kkk++)
   {
    suma_ed_vras=sum_ed_viplat_h[kkk];
    if(suma_ed_vras != 0.)
    if(kolrd_mes[kkk] != kolddpev[kkk])
     {
      float kold=kolddpev[kkk];
      if(kolddpev[kkk] > kolrd_mes[kkk])
        kold=kolrd_mes[kkk];
      suma_ed_vras=sum_ed_viplat_h[kkk]/kolrd_mes[kkk]*kold;
     }



    vras_h=vras=nbnash[kkk]-nbotpusk[kkk]-nbgosob[kkk]-sum_ed_viplat_h[kkk]+suma_ed_vras;
    if(vras < 0.)
     vras_h=vras=0.;

    zar_read_tn1w(1,mr,gr,&nastr,NULL,wpredok);
    if(vras > nastr.max_sum_for_soc)
      vras_h=vras=nastr.max_sum_for_soc;
      
    memset(metka_rd,'\0',sizeof(metka_rd));
    memset(metka_rh,'\0',sizeof(metka_rh));
      
    metka_rd[0]=' ';    
    metka_rh[0]=' ';    
    if(kolrd[kkk] < kolrd_mes[kkk])
      metka_rd[0]='*';
      
    if(kolrh[kkk] < kolrh_mes[kkk])
      metka_rh[0]='*';

    memset(boll,'\0',sizeof(boll));
    if(metka_bol[kkk] == 1)
     strcpy(boll,gettext("Больничный")); 

    fprintf(ff,"%02d.%d %-8d%s %-7d %-3.f/%4.f %10.2f %9.2f %8.2f %8.2f %8.2f %8.2f %s\n",
    mr,gr,kolrd[kkk],metka_rd,kolotp[kkk],kolrd_mes[kkk],kolrh_mes[kkk],nbnash[kkk],nbotpusk[kkk],nbgosob[kkk],
    sum_ed_viplat[kkk],suma_ed_vras,
    vras,boll);

    iotr+=kolrd[kkk];
    ioth+=kolrh[kkk];
    iotp+=kolotp[kkk];
    isumn+=nbnash[kkk];
    iotpn+=nbotpusk[kkk];
    igos+=nbgosob[kkk];
    ivras+=vras;
    isuma_ed_vras+=suma_ed_vras;
    isum_ed_viplat+=sum_ed_viplat_h[kkk];
    iceb_u_dpm(&dr,&mr,&gr,3);
   }

  if(iotr != 0)
   {
    sprintf(strsql,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),ivras,iotr,ivras/iotr);
    iceb_printw(iceb_u_toutf(strsql),buffer,view);
   }
  else
   {
    sprintf(strsql,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),ivras,iotr,0.);
    iceb_printw(iceb_u_toutf(strsql),buffer,view);
   }

fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");

  fprintf(ff,"%-*.*s %-3d /%4.f  %-7d %-3d/%4.f %10.2f %9.2f %8.2f %8.2f %8.2f %8.2f\n",
  iceb_u_kolbait(7,gettext("Итого")),iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),
  iotr,ioth,iotp,iotr_mes,ioth_mes,isumn,iotpn,igos,
  isum_ed_viplat,isuma_ed_vras,
  ivras);



  if(iotr != 0)
   fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
   ivras,iotr,ivras/iotr);
  else
   fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
   ivras,iotr,0.);
 }
iceb_podpis(ff,wpredok);
iceb_podpis(ffprot,wpredok);


fclose(ff);
fclose(ffprot);

data->imaf.plus(imaf);
data->imaf.plus(imafprot);
data->naimf.plus(gettext("Расчет средней зарплаты для начисления больничного"));
data->naimf.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < data->imaf.kolih(); nom++)
 iceb_ustpeh(data->imaf.ravno(nom),3,wpredok);
 
}
/************************/
/*Чтение настроек*/
/*************************/
void zarsrbol_rn(char *kodotp,char *kodgos,char *kodtabotrv,char *kodtabotp,int *metkaknvr,char *kod_ed_viplat,
class iceb_u_str *kodtdpev, /*Коды табеля для перерасчёта единоразовых выплат*/
GtkWidget *wpredok)
{
char stroka1[1024];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zarsrbol.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  iceb_u_polen(row_alx[0],stroka1,sizeof(stroka1),1,'|');

  if(iceb_u_SRAV(stroka1,"Коды начисления отпускных",0) == 0)
   {
    iceb_u_pole(row_alx[0],kodotp,2,'|');
    continue;
   }
  if(iceb_u_SRAV(stroka1,"Коды не входящие в расчет средней зарплаты для начисления больничного",0) == 0)
   {
    iceb_u_pole(row_alx[0],kodgos,2,'|');
    continue;
   }
  if(iceb_u_SRAV(stroka1,"Коды видов табеля отработанного времени",0) == 0)
   {
    iceb_u_pole(row_alx[0],kodtabotrv,2,'|');
    continue;
   }
  if(iceb_u_SRAV(stroka1,"Коды видов табеля отпускных",0) == 0)
   {
    iceb_u_pole(row_alx[0],kodtabotp,2,'|');
    continue;
   }
  if(iceb_u_SRAV(stroka1,"Коды табеля для перерасчёта одноразовых выплат",0) == 0)
   {
    iceb_u_pole(row_alx[0],kodtdpev,2,'|');
    continue;
   }
  if(iceb_u_SRAV(stroka1,"Коды единоразовых выплат",0) == 0)
   {
    iceb_u_pole(row_alx[0],kod_ed_viplat,2,'|');
    continue;
   }
  if(iceb_u_SRAV(stroka1,"Печатать коды невходящие в расчет",0) == 0)
   {
    iceb_u_pole(row_alx[0],stroka1,2,'|');
    if(iceb_u_SRAV(stroka1,"Вкл",1) == 0)
      *metkaknvr=1; 
    continue;
   }

 }
}
