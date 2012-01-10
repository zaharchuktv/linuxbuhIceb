/* $Id: avtprpldw.c,v 1.17 2011-02-21 07:35:51 sasa Exp $ */
/*10.12.2010    28.05.1998      Белых А.И.      avtprpldw.c
Автоматическое проведение проводок для платежних документов
*/
#include        <time.h>
#include        <errno.h>
#include        <math.h>
#include <unistd.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern struct REC rec;
extern SQL_baza bd;

void avtprpldw(const char *tabl,
const char *kop,  //Код операции
const char *npp,  //Номер документа
const char *kor,  //Код организации
const char *shet,
FILE *ff_prot,
GtkWidget *wpredok)
{
char		bros[512];
double          bb,deb,kre;
int             i,i1;
char		kodop[40];
double		sump; /*Сумма подтвержденная общая*/
double		sumpp; /*Сумма последнего подтверждения*/
double		sumpr; /*Сумма по проводкам*/
time_t		vrem;
char		tipd[8];
char		strsql[512];
class iceb_u_str kontr("");
class iceb_u_str kontr1("");
char		kor1[32],kor2[32];
short		metpr;
char		shet1[32],shet2[32],shet3[32];
char		koresp[512];
short		kp;
char		kom[512];
int		pod;
short		dp,mp,gp;
short		tp1,tp2;
struct OPSHET	shetv;
int		kolpr=2;
int kekv=0;
class iceb_u_spisok sp_prov;
class iceb_u_double sum_prov_dk;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

/********
printw("avtprpld-tabl-%s kop=%s npp-%s kor-%s shet-%s\n",
tabl,kop,npp,kor,shet);
OSTANOV();
**********/

int metka_vkontr=2; /*метка режима выполнения проводки для контрагента если его нет в списке счёта 0-запрос 2-автоматическая вставка*/
if(iceb_poldan("Автоматическая запись нового контрагента в список счёта",strsql,"nastdok.alx",wpredok) == 0)
 {
   if(iceb_u_SRAV("Вкл",strsql,1) == 0)
    metka_vkontr=2;
   else
    metka_vkontr=0;
 }
if(ff_prot != NULL)
 {
  if(metka_vkontr == 0)
   fprintf(ff_prot,"Ручная вставка кода контрагента в список счёта с развёрнутым сальдо если его там нет\n");
  if(metka_vkontr == 2)
   fprintf(ff_prot,"Автоматическая вставка кода контрагента в список счёта с развёрнутым сальдо если его там нет\n");
 }


if(shet[0] == '\0')
  return;
memset(kom,'\0',sizeof(kom));

/*Узнаем сумму подтвержденных записей*/
sump=sumpzpdw(tabl,&dp,&mp,&gp,&sumpp,wpredok);

if(iceb_u_SRAV(tabl,"Pltp",0) == 0)
 sprintf(tipd,gettext("ПЛТ"));
if(iceb_u_SRAV(tabl,"Pltt",0) == 0)
 sprintf(tipd,gettext("ТРЕ"));

/*Суммируем выполненные проводки*/

sumpr=0.;

sumpr=sumprpdw(rec.dd,rec.md,rec.gd,npp,0,tipd,shet,wpredok);
/*
printw("sumpr=%f sump=%f\n",sumpr,sump);
OSTANOV();
*/
/*Нужные проводки сделаны*/

if(sumpr == sump)
 {
  iceb_menu_soob(gettext("Все проводки сделаны !"),wpredok);
  return;
 }


memset(kor1,'\0',sizeof(kor1));
memset(kor2,'\0',sizeof(kor2));
kontr.new_plus(kor);

if(iceb_provsh(&kontr,shet,&shetv,metka_vkontr,0,wpredok) != 0)
   return;
tp1=shetv.saldo;
strncpy(kor1,kontr.ravno(),sizeof(kor1)-1);
strncpy(kor2,kontr.ravno(),sizeof(kor2)-1);

class iceb_u_str imaf_nast;
/*Открываем файл настроек*/
if(iceb_u_SRAV(tabl,"Pltp",0) == 0)
 imaf_nast.plus("avtprodok.alx");
if(iceb_u_SRAV(tabl,"Pltt",0) == 0)
 imaf_nast.plus("avtprodokt.alx");

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_nast.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки %s\n",imaf_nast.ravno());
  return;
 }

strcpy(shet3,shet);

kp=0;
time(&vrem);
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
    continue;
    
  if(iceb_u_polen(row_alx[0],kodop,sizeof(kodop),2,'|') != 0)
   continue;
  
  if(iceb_u_SRAV(kodop,kop,0) != 0)
   continue;
  
  memset(bros,'\0',sizeof(bros));    
  iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|');
  i1=strlen(bros);
  metpr=0;
  if(iceb_u_strstrm(bros,"У") == 1)
      metpr=1;
  if(iceb_u_strstrm(bros,"С") == 1)
      metpr=2;

  if(metpr == 0)
   continue;

  deb=kre=0.;
  if(metpr == 1)
   {
    deb=iceb_u_okrug(rec.uslb,0.01);
    kre=0.;   
   }

  if(metpr == 2)
   {
    bb=sumpp-rec.uslb;
    deb=iceb_u_okrug(bb,0.01);
    kre=0.;
   }

  if(fabs(deb) < 0.01)
    continue;
       
  /*Берем счет*/
  memset(bros,'\0',sizeof(bros));
  iceb_u_polen(row_alx[0],bros,sizeof(bros),3,'|');

  memset(shet1,'\0',sizeof(shet1));
  if(iceb_u_polen(bros,shet1,sizeof(shet1),1,':') != 0)
   strncpy(shet1,bros,sizeof(shet1)-1);
  else
   iceb_u_polen(bros,&kontr1,2,':');
  
  /*Берем кореспонденцию*/
  memset(koresp,'\0',sizeof(koresp));
  iceb_u_polen(row_alx[0],koresp,sizeof(koresp),4,'|');

  i1=iceb_u_pole2(koresp,',');
  
  for(i=0; i <= i1; i++)
   {
    memset(shet2,'\0',sizeof(shet2));
    kontr.new_plus("");

    if(i1 > 0)
     iceb_u_pole(koresp,bros,i+1,',');
    else
     strcpy(bros,koresp);      

    if(i1 > 0 && i == 0)
     i1--;
     
    /*Проверяем есть ли код контрагента*/
    if(iceb_u_pole(bros,shet2,1,':') != 0)
     strcpy(shet2,bros);
    else
     iceb_u_pole(bros,&kontr,2,':');

    if(shet2[0] == '\0')
     continue;

    if(iceb_provsh(&kontr,shet2,&shetv,metka_vkontr,0,wpredok) != 0)
       continue;
    tp2=shetv.saldo;

    if(kontr.getdlinna() > 1)
      strcpy(kor1,kontr.ravno());
    if(kor2[0] == '\0' && kontr.ravno()[0] != '\0')
      strcpy(kor2,kontr.ravno());
    
    iceb_zapmpr(gp,mp,dp,shet2,shet3,kor1,kor2,deb,kre,kom,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);

    strcpy(shet3,shet2);
    memset(kor1,'\0',sizeof(kor1));
    memset(kor2,'\0',sizeof(kor2));
    strcpy(kor1,kontr.ravno());
    strcpy(kor2,kontr.ravno());
   }


  if(kontr1.getdlinna() <= 1)
     kontr1.new_plus(kor);


   if(iceb_provsh(&kontr1,shet1,&shetv,metka_vkontr,0,wpredok) != 0)
      continue;

 

   tp1=shetv.saldo;

   if(kontr1.getdlinna() > 1)
     strcpy(kor1,kontr1.ravno());
   if(kor2[0] == '\0' && kontr1.ravno()[0] != '\0')
     strcpy(kor2,kontr1.ravno());

//   printw("shet3=%s shet1=%s kor1=%s kor2=%s kontr1=%s deb=%.2f kre=%.2f\n",
//   shet3,shet1,kor1,kor2,kontr1,deb,kre);
//   OSTANOV();       

   iceb_zapmpr(gp,mp,dp,shet1,shet3,kor1,kor2,deb,kre,kom,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);

 }
/*Запись проводок из памяти в базу*/
pod=0;
time(&vrem);
int tipz=0;
if(iceb_u_SRAV(tabl,"Pltp",0) == 0)
 tipz=2;
if(iceb_u_SRAV(tabl,"Pltt",0) == 0)
 tipz=1;
iceb_zapmpr1(npp,kop,pod,vrem,tipd,rec.dd,rec.md,rec.gd,tipz,&sp_prov,&sum_prov_dk,ff_prot,wpredok);

}
