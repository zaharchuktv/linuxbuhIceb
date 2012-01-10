/* $Id: zaravpr_oldw.c,v 1.2 2011-04-04 08:10:14 sasa Exp $ */
/*12.03.2011	23.09.1998	Белых А.И.	zaravpr_oldw.c
Выполнение проводок для зароботной платы
*/
#include <stdlib.h>
#include        <time.h>
#include        <math.h>
#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"

void zaravprf_old(int,short,short,class SQLCURSOR *cur_alx,short*,class iceb_u_spisok*,class iceb_u_double*,FILE*,GtkWidget*);
void zaravprf(int,short,short,class SQLCURSOR *cur_alx,short*,class iceb_u_spisok*,class iceb_u_double*,FILE*,GtkWidget*);

extern short   *kodbl;  /*Код начисления больничного*/
extern short    *knnf; //Коды начислений недежными формами 
extern short    *kodmp;   /*Коды материальной помощи*/
extern short	avkvs; //0-Не вставлять автоматом код контрагента 1-вставлять
extern char	*shetb; /*Бюджетные счета*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern double   okrg; /*Округление*/
extern char	*mprov; /*Массив проводок*/
extern double	*mdk;   /*Массив дебетов и кредитов к масиву проводок*/
extern char	pktn[16]; /*Приставка к табельному номеру*/
extern short	kodpenf; /*Код пенсионного фонда*/
extern short	kodbezf;  /*Код фонда занятости*/
extern short	kodsoc;   /*Код фонда социального страхования*/
extern short    *knvr;/*Коды начислений не входящие в расчет подоходного налога*/
extern char	*shpn; /*Счет подоходного налога*/
extern char	shrpz[16]; /*Счет расчетов по зарплате*/
extern char	shrpzbt[16]; /*Счет расчетов по зарплате бюджет*/
extern class iceb_u_spisok dop_zar_sheta; //Дополнительные зарплатные счёта
extern class iceb_u_spisok dop_zar_sheta_spn; /*Списки начислений для каждого дополнительного зарплатного счёта*/

extern int kekv_at;  //Код экономической классификации для атестованых
extern int kekv_nat; //Код экономической классификации для не атестованых
extern int kodzv_nat;//Код звания неатестованный
extern int kekv_nfo; //Код экономической классификации для начислений на фонд оплаты


extern SQL_baza bd;

void zaravpr_oldw(int tbn,short mp,short gp,FILE *ff_prot,GtkWidget *wpredok)
{
char		nn[64];
short		kp;
short		d,m,g;
short		dd,md,gd;
char		kom[512];
double		deb,kre;
char		kor11[64];
class iceb_u_str kor21("");
class iceb_u_str kontshzar("");
int		i,i1;
time_t		vrem;
double		sym;
short		prn;
char		sh[64];
char		shet[64];
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
char		kontr1[64];
short		kodnu; /*Код начисления/удержания*/
char		koresp[512];
char		bros[1024];
int		metpr; /*1-приход 2 расход*/
short		knah;
char		kodgni[64];
int		polekor; //Номер поля с кореспондирующими счетами
char		koment[1024];
struct OPSHET	shetv;
int		kolpr=2;
char		shetzar[64];
SQLCURSOR curr;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kekv=0;
class iceb_u_spisok sp_prov;
class iceb_u_double sum_prov_dk;

if(ff_prot != NULL)
  fprintf(ff_prot,"%s-%d.%d tbn=%d\n",__FUNCTION__,mp,gp,tbn);

if(shrpz[0] == '\0')
  return;
if(iceb_pvglkni(mp,gp,wpredok) != 0)
 return;

gd=gp;
md=mp;

sprintf(nn,"%d-%d",mp,tbn);

iceb_u_dpm(&dd,&md,&gd,5);

  
//Проводки удаляем сразу так, как можно обнулить все начисления и удержания
if(iceb_udprgr(gettext("ЗП"),dd,md,gd,nn,0,0,wpredok) != 0)
 return;

sprintf(strsql,"select datz,suma,shet,prn,knah,kom from Zarp where \
datz >='%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and suma != 0.",
gp,mp,gp,mp,tbn);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
/*
printw("kolstr=%d\n",kolstr);
OSTANOV();
*/
if(kolstr == 0)
  return;

if(kodzv_nat != 0 && (kekv_at != 0 || kekv_nat != 0)) //Узнаём звание и определяем kekv
 {
  sprintf(strsql,"select zvan from Zarn where tabn=%d and god=%d and mes=%d",tbn,gp,mp);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
   {
    if(atoi(row1[0]) == kodzv_nat)
     kekv=kekv_nat;
    else
     kekv=kekv_at;
    
   }   
  else
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не нашли звание !!!\n");
   }
 }

/*Открываем файл настроек*/
sprintf(strsql,"select str from Alx where fil='zarnpr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки zarnpr.alx\n");
  return;
 }

char kontr_baz[56];
sprintf(kontr_baz,"%s%d",pktn,tbn);
kontshzar.new_plus(kontr_baz);
if(iceb_provsh(&kontshzar,shrpz,&shetv,avkvs,0,wpredok) != 0)
   return;

kontshzar.new_plus(kontr_baz);
if(shrpzbt[0] != '\0')
 if(iceb_provsh(&kontshzar,shrpzbt,&shetv,avkvs,0,wpredok) != 0)
   return;


kp=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  //Проверяем нужно ли делать проводки для этого начисления/удержания
  prn=atoi(row[3]);  
  if(prn == 2) //Удержания
    sprintf(strsql,"select prov from Uder where kod=%s",row[4]);
  if(prn == 1) //Начисления
    sprintf(strsql,"select prov from Nash where kod=%s",row[4]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    if(row1[0][0] == '1')
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"Для кода %s проводки выключены!\n",row[4]);
      continue;
     }
  sym=atof(row[1]);
  if(fabs(sym) < 0.01)
    continue;

  strncpy(sh,row[2],sizeof(sh)-1);


//  printw("\nsh=%s\n",sh);
  
  memset(shetzar,'\0',sizeof(shetzar));
  if(prn == 1)
   {
    if(iceb_u_proverka(shetb,sh,0,1) == 0)
     {
      if(shrpzbt[0] != '\0')
       strncpy(shetzar,shrpzbt,sizeof(shetzar)-1);   
      else
       strncpy(shetzar,shrpz,sizeof(shetzar)-1);   
     }
    else
      strncpy(shetzar,shrpz,sizeof(shetzar)-1);   
   }
  else
   {
    if(iceb_u_proverka(shetbu,sh,0,1) == 0)
     {
      if(shrpzbt[0] != '\0')
       strncpy(shetzar,shrpzbt,sizeof(shetzar)-1);   
      else
       strncpy(shetzar,shrpz,sizeof(shetzar)-1);   
     }
    else
     strncpy(shetzar,shrpz,sizeof(shetzar)-1);   
   }
  
  knah=atoi(row[4]);
  iceb_u_rsdat(&d,&m,&g,row[0],2);  

  deb=sym;
  if(prn == 2)
    deb*=(-1);
  kre=0.;
  memset(kom,'\0',sizeof(kom));
  strncpy(kom,row[5],sizeof(kom)-1);

  memset(kontr1,'\0',sizeof(kontr1));
  memset(kor11,'\0',sizeof(kor11));
  kor21.new_plus("");


  
  char  shet_zar_tek[56]; //Счёт зарплаты, который в конечном итоге будет использоваться в проводках
  class iceb_u_str kontr_shet_zar_tek(""); //Код контрагента для счёта зарплаты
    
  //Обязательно тут это копирование должно быть так как настроек в файле может и не быть
  strncpy(shet_zar_tek,shetzar,sizeof(shet_zar_tek)-1);
  kontr_shet_zar_tek.new_plus(kontshzar.ravno());
  strcpy(kor11,kontshzar.ravno());

  //Проверяем может это начисление для которого нужно делать проводки по дополнительному зарплатному счёту
  if(prn == 1)
   for(int ii=0; ii < dop_zar_sheta_spn.kolih(); ii++)
   if(iceb_u_proverka(dop_zar_sheta_spn.ravno(ii),knah,0,1) == 0)
    {
     strncpy(shet_zar_tek,dop_zar_sheta.ravno(ii),sizeof(shet_zar_tek)-1);
     kontr_shet_zar_tek.new_plus(kontr_baz);
     if(iceb_provsh(&kontr_shet_zar_tek,shet_zar_tek,&shetv,avkvs,0,wpredok) != 0)
       continue;

    }  
  /*Читаем файл настройки*/
  cur_alx.poz_cursor(0);
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    
//    printw("%s",row_alx[0]);
    if(row_alx[0][0] == '#' || row_alx[0][0] == 'f')
        continue;
    
    if(prn == 1 && row_alx[0][0] == '-')
     continue;
    if(prn == 2 && row_alx[0][0] == '+')
     continue;
     
    if(iceb_u_pole(row_alx[0],bros,2,'|') != 0)
       continue;
    kodnu=(int)iceb_u_atof(bros);
      if(kodnu == 0 || kodnu != knah)
       continue;
    int nast_tn=0;
    if(iceb_u_polen(bros,&nast_tn,2,':') == 0)
       {
        if(nast_tn != 0 && nast_tn != tbn)
         continue;
       }  
    if(iceb_u_polen(bros,strsql,sizeof(strsql),3,':') == 0) //Введён счёт вместо зарплатного счёта (661)
     {
      if(strsql[0] != '\0')
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"Введён счёт вместо зарплатного счёта=%s\n",strsql);
        strncpy(shet_zar_tek,strsql,sizeof(shet_zar_tek)-1);
        if(iceb_provsh(&kontr_shet_zar_tek,shet_zar_tek,&shetv,avkvs,0,wpredok) != 0)
         continue;
        strncpy(shet_zar_tek,shet_zar_tek,sizeof(shet_zar_tek)-1);
       }
     }
    polekor=4;
    // Смотрим настройку на комментарий
    if(iceb_u_polen(bros,strsql,sizeof(strsql),2,'/') == 0)
     {
      if(iceb_u_polen(strsql,koment,sizeof(koment),1,':') != 0) 
       strncpy(koment,strsql,sizeof(koment)-1);
      if(koment[0] != '\0')
       {
        if(iceb_u_strstrm(row[5],koment) != 0)
          polekor++;
       }
     }
      memset(bros,'\0',sizeof(bros));    
      iceb_u_pole(row_alx[0],bros,1,'|');
      i1=strlen(bros);
      metpr=0;
      for(i=0; i < i1; i++)
       {
        if(bros[i] == '+')
         {
          metpr=1;
          break;
         }
        if(bros[i] == '-')
         {
          metpr=2;
          break;
         }
       }

      if(metpr == 0 || metpr != prn)
       continue;


      /*Берем счет*/
      memset(bros,'\0',sizeof(bros));
      iceb_u_pole(row_alx[0],bros,3,'|');

      memset(shet,'\0',sizeof(shet));
      if(iceb_u_pole(bros,shet,1,':') != 0)
       strncpy(shet,bros,sizeof(shet)-1);
      else
       iceb_u_pole(bros,kontr1,2,':');

      if(shet[0] == '\0' || iceb_u_strstrm(shet,"*") == 1) /*Если счёт не введён то подходит для всех счетов этого начисления/удержания*/
       strcpy(shet,sh);
      
      if(iceb_u_SRAV(shet,sh,1) != 0)
        continue;

      if(kontr1[0] != '\0')
        kontr_shet_zar_tek.new_plus(kontr1);
      else       
        kontr_shet_zar_tek.new_plus(kontr_baz);

      if(ff_prot != NULL)
       fprintf(ff_prot,"2проверяем контрагент %s счёт %s\n",kontshzar.ravno(),shet);
      if(iceb_provsh(&kontr_shet_zar_tek,shet,&shetv,avkvs,0,wpredok) != 0)
         continue;

      /*Берем кореспонденцию*/
      memset(koresp,'\0',sizeof(koresp));
      iceb_u_pole(row_alx[0],koresp,polekor,'|');
      if(koresp[0] == '\0')
       continue;

//    printw("\n%s",row_alx[0]);
      

      i1=iceb_u_pole2(koresp,',');
      for(i=0; i <= i1; i++)
       {
        memset(shet,'\0',sizeof(shet));

        if(i1 > 0)
         iceb_u_pole(koresp,bros,i+1,',');
        else
         strcpy(bros,koresp);      

        if(i1 > 0 && i == 0)
         i1--;
         
        /*Проверяем есть ли код контрагента*/
        if(iceb_u_pole(bros,shet,1,':') != 0)
         strcpy(shet,bros);
        else
         iceb_u_polen(bros,&kor21,2,':');

        if(shet[0] == '\0')
         continue;
        if(kor21.getdlinna() <= 1)
          kor21.new_plus(kontr_baz);
        if(iceb_provsh(&kor21,shet,&shetv,avkvs,1,wpredok) != 0)
           continue;
/****************
         1я проводка 
           делаем одиночную проводку и переходим на просмотр следующего 
           начисления/удержания
         Nя проводка
           делаем одиночную проводку и следующий в цепочке счет должен 
           кореспонди
********************/
        kolpr=2;
        if(shetv.stat == 1)
          kolpr=1;
/**********
        printw("shet=%s shet_zar_tek=%s kor21=%s kontr_shet_zar_tek=%s deb=%f kre=%f\n",
        shet,shet_zar_tek,kor21,kontr_shet_zar_tek,deb,kre);
        OSTANOV();        
***************/
        if(prn == 1)
         {
          if(kolpr == 2)
            iceb_zapmpr(g,m,d,shet,shet_zar_tek,kor21.ravno(),kontr_shet_zar_tek.ravno(),deb,kre,kom,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
          if(kolpr == 1)
            iceb_zapmpr(g,m,d,shet,"",kor21.ravno(),kontr_shet_zar_tek.ravno(),deb,kre,kom,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
          
         }
        if(prn == 2)
         {
          if(kolpr == 2)
           iceb_zapmpr(g,m,d,shet_zar_tek,shet,kontr_shet_zar_tek.ravno(),kor21.ravno(),deb,kre,kom,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
          if(kolpr == 1)
           iceb_zapmpr(g,m,d,"",shet,kontr_shet_zar_tek.ravno(),kor21.ravno(),deb,kre,kom,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
        if(kolpr == 2)
         {
          strncpy(shet_zar_tek,shet,sizeof(shet_zar_tek)-1);
          kontr_shet_zar_tek.new_plus(kor21.ravno());
         }
        kor21.new_plus("");
  
       }
// Настройка может быть только одна
      break;
     }
  
  if(kontr1[0] != '\0')
    kor21.new_plus(kontr1);

  if(iceb_u_SRAV(sh,shpn,0) == 0)
   {
    /*Читаем код города налоговой инспекции*/
    sprintf(strsql,"select kodg from Kartb where tabn=%d",tbn);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tbn);
      iceb_menu_soob(strsql,wpredok);
      continue;      
     }    

    /*Читаем код контрагента*/
    memset(kodgni,'\0',sizeof(kodgni)-1);
    if(atoi(row1[0]) != 0)
     {
      strncpy(kodgni,row1[0],sizeof(kodgni)-1);
      
      sprintf(strsql,"select kontr from Gnali where kod=%s",kodgni);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        class iceb_u_str repl;
               
        sprintf(strsql,"%s %s !",
        gettext("Не найден код города налоговой инспекции"),kodgni);
        repl.plus(strsql);
        
        sprintf(strsql,"%s:%d !",
        gettext("Табельный номер"),tbn);
        repl.ps_plus(strsql);
        
        iceb_menu_soob(&repl,wpredok);
        continue;      
       }
      else
       kor21.new_plus(row1[0]);
     }
   }

  if(kor21.getdlinna() <= 1)
    kor21.new_plus(kontr_baz);
  if(iceb_provsh(&kor21,sh,&shetv,avkvs,1,wpredok) != 0)
     continue;
  kolpr=2;
  if(shetv.stat == 1)
    kolpr=1;

  if(prn == 1)
   {
/********
    printw("+*sh=%s shet_zar_tek=%s kor21=%s kontr_shet_zar_tek=%s deb=%f kre=%f\n",
    sh,shet_zar_tek,kontr_shet_zar_tek,kor21,deb,kre);
    OSTANOV();
*************/
    if(kolpr == 2)
      iceb_zapmpr(g,m,d,sh,shet_zar_tek,kor21.ravno(),kontr_shet_zar_tek.ravno(),deb,kre,kom,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
    if(kolpr == 1)
      iceb_zapmpr(g,m,d,sh,"",kor21.ravno(),kontr_shet_zar_tek.ravno(),deb,kre,kom,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
   }
  if(prn == 2)
   {
/************
    printw("-*shet=%s shet_zar_tek=%s kor1=%s kor2=%s deb=%f kre=%f\n",
    sh,shet_zar_tek,kontr_shet_zar_tek,kor21,deb,kre);
    OSTANOV();
*************/
    if(kolpr == 2)
      iceb_zapmpr(g,m,d,shet_zar_tek,sh,kontr_shet_zar_tek.ravno(),kor21.ravno(),deb,kre,kom,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
    if(kolpr == 1)
      iceb_zapmpr(g,m,d,"",sh,kontr_shet_zar_tek.ravno(),kor21.ravno(),deb,kre,kom,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
   }
 }



/*Выполнение проводок по соцстаху*/
if(iceb_u_sravmydat(1,mp,gp,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
 zaravprf_old(tbn,mp,gp,&cur_alx,&kp,&sp_prov,&sum_prov_dk,ff_prot,wpredok);
else
 zaravprf(tbn,mp,gp,&cur_alx,&kp,&sp_prov,&sum_prov_dk,ff_prot,wpredok);


memset(kor11,'\0',sizeof(kor11));
kor21.new_plus(gettext("ЗП"));
time(&vrem);

sprintf(strsql,"LOCK TABLES Prov WRITE,Blok READ,icebuser READ");
class iceb_lock_tables lock(strsql);

/*Запись проводок из памяти в базу*/
iceb_zapmpr1(nn,kor11,0,vrem,kor21.ravno(),dd,md,gd,0,&sp_prov,&sum_prov_dk,ff_prot,wpredok);
}

/**************************************/
/*Расчет проводок по социальным фондам начиная с 1.1.2011*/
/**************************************/

void zaravprf(int tbn,short mp,short gp,class SQLCURSOR *cur_alx,
short *kolprov,
class iceb_u_spisok *sp_prov,
class iceb_u_double *sum_prov_dk,
FILE *ff_prot,
GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row,row2;
char		strsql[1024];
class iceb_u_str kor11("");
class iceb_u_str kor21("");;
char		kontr1[64];
class iceb_u_str kontsocsh("");
class iceb_u_str kontsocsh1("");
class iceb_u_str kontsocsh2("");
short		kodz;
double		sum,sumi,sumas,sumasb,sumap,sumapb;
char		socshet[64]; /*Соц. счет если он один*/
char		socshet1[64]; /*Соц. счет не бюджетный*/
char		socshet2[64]; /*Соц. счет бюджетный*/
char		nahshet[64];
short		d,m,g;
char		koment[1024],koment1[1024];
double		deb,kre;
short		knah;
char		shet1[64],shet[64];
char		bros[1024];
char		koresp[1024];
int		i,i1;
char		kodnv[1024];
int		polekor; //Номер поля с кореспондирующими счетами
struct OPSHET   shetv;
int		kolpr=2;
short           metzmp=0; //Метка запуска проверки необлагаемой материальной помощи
double          sumnmp=0.; //Сумма необлагаемой материальной помощи
double          sumnmpz=0.; //Сумма необлагаемой материальной помощи
SQLCURSOR curr;
char shet_zamena[64];
int kekv=kekv_nfo;
SQL_str row_alx;
int metka_bol=0;

if(ff_prot != NULL)
 fprintf(ff_prot,"\nВыполнение проводок на фонд зарплаты.\nkekv=%d\n",kekv);
 
class iceb_u_double SUMA;
class iceb_u_spisok NAHIS;
if(zaravpt_osw(tbn,mp,gp,&NAHIS,&SUMA,wpredok) != 0)
 return;
int kodf_esv_bol=0;
if(iceb_poldan("Код фонда единого социального взноса для больничных",&kodf_esv_bol,"zarnast.alx",wpredok) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден Код фонда единого социального взноса для больничных!\n");
 }

/*Смотрим записи социальных отчислений*/
sprintf(strsql,"select kodz,shet,sumas,sumap,sumapb from Zarsocz where \
datz='%04d-%d-01' and tabn=%d",gp,mp,tbn);
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
/*
printw("kolstr=%d\n",kolstr);
OSTANOV();
*/
if(kolstr <= 0)
  return;

int kolzap=NAHIS.kolih();

while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"\nЗапись фонда-%s %s %s %s %s\n\
-----------------------------------------------------------\n",
    row[0],row[1],row[2],row[3],row[4]);

  kodz=atoi(row[0]);
  metka_bol=0;
  if(kodz == kodf_esv_bol) /*фонд для больничного нужно смотреть только больничные*/
   metka_bol=1;

  memset(socshet,'\0',sizeof(socshet));
  memset(socshet1,'\0',sizeof(socshet1));
  memset(socshet2,'\0',sizeof(socshet2));
  strncpy(socshet,row[1],sizeof(socshet)-1);
  if(iceb_u_polen(socshet,socshet1,sizeof(socshet1)-1,1,',') == 0)
     iceb_u_polen(socshet,socshet2,sizeof(socshet2)-1,2,',');

  sumas=atof(row[2]);
  sumap=atof(row[3]);
  sumapb=atof(row[4]);
  sumasb=0.;

  if(sumapb != 0.)
   {
    sumasb=sumas*sumapb/sumap;
   }

  /*Берем коды не входящие в расчет*/
  memset(kodnv,'\0',sizeof(kodnv));
  sprintf(strsql,"select kodn from Zarsoc where kod=%d\n",kodz);
  if(sql_readkey(&bd,strsql,&row2,&curr) != 1)
   {
    sprintf(strsql,"%s-%s %d !",__FUNCTION__,gettext("Не найден код соц. страхования"),kodz);
    iceb_menu_soob(strsql,wpredok);
   }  
  else
   strncpy(kodnv,row2[0],sizeof(kodnv)-1);
   
  sprintf(strsql,"%s%d",pktn,tbn);
  kontsocsh.new_plus(strsql);
  if(socshet1[0] == '\0' && socshet2[0] == '\0')
   if(iceb_provsh(&kontsocsh,socshet,&shetv,avkvs,0,wpredok) != 0)
     return;

  if(socshet1[0] != '\0')
   {
    sprintf(strsql,"%s%d",pktn,tbn);
    kontsocsh1.new_plus(strsql);
    if(iceb_provsh(&kontsocsh1,socshet1,&shetv,avkvs,0,wpredok) != 0)
     return;
   }
  if(socshet2[0] != '\0')
   {
    sprintf(strsql,"%s%d",pktn,tbn);
    kontsocsh2.new_plus(strsql);
    if(iceb_provsh(&kontsocsh2,socshet2,&shetv,avkvs,0,wpredok) != 0)
     return;
   }
  /*Повторно*/
  kor11.new_plus("");
  kor21.new_plus("");
  sumi=0.;
  sumnmp=sumnmpz;

  for(int nomz=0; nomz < kolzap; nomz++)
   {

    if(fabs(sumi) > fabs(sumas))
      break;

     if(sumas < 0. && sumi < sumas)
      break;

    char kodnah[32];
    char dataz[16];
    iceb_u_polen(NAHIS.ravno(nomz),kodnah,sizeof(kodnah),3,'|');
    knah=atoi(kodnah);

    if(metka_bol == 0) /*если фонд не больничный то больничные пропускаем*/
     if(provkodw(kodbl,knah) >= 0)
       continue;
    if(metka_bol == 1) /*если фонд больничный то не больничные пропускаем*/
     if(provkodw(kodbl,knah) < 0)
       continue;
    sum=SUMA.ravno(nomz);

    if(ff_prot != NULL)
      fprintf(ff_prot,"%s = %.2f\n",NAHIS.ravno(nomz),sum);

    iceb_u_polen(NAHIS.ravno(nomz),dataz,sizeof(dataz),1,'|');

    /*Проверяем на коды не входящие в расчет */
    if(kodnv[0] != '\0')
     if(iceb_u_pole1(kodnv,kodnah,',',0,&i1) == 0 ||
      iceb_u_SRAV(kodnv,kodnah,0) == 0)
       continue;

    iceb_u_rsdat(&d,&m,&g,dataz,2);


    //Записей с кодами материальной помощи может быть несколько
    if(provkodw(kodmp,knah) >= 0)
     {
      if(metzmp == 0)
       {
        sumnmpz=sumnmp=0.;
        metzmp++;
       }
      if(sumnmp > 0.)
       {
        if(sumnmp > sum)
         {
          sumnmp-=sum;
         }
        else
         {
          sum-=sumnmp;
          sumnmp=0.;
         }
       }
     }

    memset(nahshet,'\0',sizeof(nahshet));
    iceb_u_polen(NAHIS.ravno(nomz),nahshet,sizeof(nahshet),2,'|');
    memset(koment,'\0',sizeof(koment));
    iceb_u_polen(NAHIS.ravno(nomz),koment,sizeof(koment),4,'|');
    kre=0.;

    if(fabs(sumi+sum) <= fabs(sumas))
     deb=sum;
    else
     {
      deb=sumas-sumi;
     }
    sumi+=sum;
    
//    printw("sumas=%f sumap=%f deb=%f\n",sumas,sumap,deb);
    if(ff_prot != NULL)
     fprintf(ff_prot,"Сумма начисления %.2f\n\
Вычисляем сумму взноса которая приходится на эту сумму\n\
%.2f*%.2f/%.2f=%.2f\n",
      deb, 
      deb,sumap,sumas,
      deb*sumap/sumas);
      
    deb=deb*sumap/sumas;
   
    //не округляем округлять будем при выполнении проводок
//    deb=okrug(deb,okrg);         

//    printw("deb=%f\n",deb);
//    OSTANOV();
         
    memset(kontr1,'\0',sizeof(kontr1));
    kor21.new_plus("");

    memset(shet1,'\0',sizeof(shet1));
    strncpy(shet1,socshet,sizeof(shet1)-1);
    
    kor11.new_plus(kontsocsh.ravno());
    if(socshet1[0] != '\0')
     {
      strncpy(shet1,socshet1,sizeof(shet1)-1);
      kor11.new_plus(kontsocsh1.ravno());
     }   
    if(socshet2[0] != '\0' && shetb != NULL)
      if(iceb_u_pole1(shetb,nahshet,',',0,&i1) == 0 || iceb_u_SRAV(shetb,nahshet,0) == 0)
       {
        strncpy(shet1,socshet2,sizeof(shet1)-1);
        kor11.new_plus(kontsocsh2.ravno());
       }
    if(fabs(deb) < 0.01)
     continue;

    memset(shet_zamena,'\0',sizeof(shet_zamena));

    /*Читаем файл настройки*/
    cur_alx->poz_cursor(0);
    while(cur_alx->read_cursor(&row_alx) != 0)
     {
      
      if(row_alx[0][0] == '#')
        continue;        
      if(row_alx[0][0] == '+')
        continue;        
      if(row_alx[0][0] == '-')
        continue;        
      if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') != 0)
       continue;
      if(iceb_u_strstrm(bros,"f") == 0)
       continue;

      /*Берем код*/
      iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
      if(iceb_u_atof(bros) != kodz)
        continue;         

      /*Соц счёт который будет использоваться в проводке - в настройке проводок он может быть заменён*/
//      class iceb_u_str soc_shet_prov(shet1); 

      polekor=4;
      // Смотрим настройку на комментарий
      if(iceb_u_polen(bros,strsql,sizeof(koment1),2,'/') == 0)
       {
        if(iceb_u_polen(strsql,koment1,sizeof(koment1),1,':') != 0)
         strncpy(koment1,strsql,sizeof(koment1)-1);
        if(koment1[0] != '\0')
         {
          if(iceb_u_strstrm(koment,koment1) != 0)
            polekor++;
         }
       }     
      /*Смотрим табельный номер*/
      if(iceb_u_polen(bros,strsql,sizeof(strsql),2,':') == 0)
       {
        if(atoi(strsql) != tbn)
         {
          continue;
         }
       }
      /*Смотрим счёт в замену соц. счёта*/
      if(iceb_u_polen(bros,strsql,sizeof(strsql),3,':') == 0)
       {
        if(strsql[0] != '\0')
         {
          strncpy(shet1,strsql,sizeof(shet1)-1);
          if(ff_prot != NULL)
           fprintf(ff_prot,"Заменили соц. счёт на %s\n",strsql);
          if(iceb_provsh(&kor11,shet1,&shetv,avkvs,0,wpredok) != 0)
              continue;
         }
       }
      /*Берем счет*/
      memset(bros,'\0',sizeof(bros));
      iceb_u_polen(row_alx[0],bros,sizeof(bros),3,'|');
              
      memset(shet,'\0',sizeof(shet));
      if(iceb_u_polen(bros,shet,sizeof(shet),1,':') != 0)
       {
        strncpy(shet,bros,sizeof(shet)-1);
       }
      else
       {
        iceb_u_polen(bros,kontr1,sizeof(kontr1),2,':');
        //Проверяем для какого кода начисления настройка
        int kod_nah_nast=0;
        if(iceb_u_polen(bros,&kod_nah_nast,4,':') == 0)
         if(kod_nah_nast != 0 && kod_nah_nast != knah)
          continue;
       }        

      if(shet[0] == '\0' || iceb_u_strstrm(shet,"*") == 1) /*Если счёт не введён то подходит*/
       strcpy(shet,nahshet);
        
       
      if(iceb_u_SRAV(shet,nahshet,1) != 0)
       {
        kontr1[0]='\0';
        continue;
       }

      if(ff_prot != NULL)
       {
        fprintf(ff_prot,"%s",row_alx[0]);
        fprintf(ff_prot,"Обнаружили настройкy на счёт-%s код контрагента=%s\n",shet,kontr1);
       }         
     //настройка на замену одного счета другим
     //делаем это только после сравнения счетов
     iceb_u_polen(bros,shet_zamena,sizeof(shet_zamena),3,':');
     if(shet_zamena[0] == '*')
      {
       SQLCURSOR cursh;
       SQL_str   rowsh;
       //взять счет из карточки
       sprintf(strsql,"select shet from Zarn where tabn=%d and god=%d and mes=%d",tbn,gp,mp);
       if(sql_readkey(&bd,strsql,&rowsh,&cursh) != 1)
        {
         class iceb_u_str repl;
         sprintf(strsql,"%s-Не найдена настройка по дате %d.%d для определения счета в карточке !",
         __FUNCTION__,mp,gp);
         repl.plus(strsql);
         
         sprintf(strsql,"Табельный номер %d",tbn);
         repl.ps_plus(strsql);
         iceb_menu_soob(&repl,wpredok);
        }
       else
        strncpy(shet_zamena,rowsh[0],sizeof(shet_zamena)-1);
      }
       
      /*Берем кореспонденцию*/
      memset(koresp,'\0',sizeof(koresp));
      iceb_u_polen(row_alx[0],koresp,sizeof(koresp),polekor,'|');
      if(koresp[0] == '\0')
       continue;
      if(ff_prot != NULL)
        fprintf(ff_prot,"Строка кореспондирующих счетов = %s\n",koresp);

      i1=iceb_u_pole2(koresp,',');
      for(i=0; i <= i1; i++)
       {
         
        memset(shet,'\0',sizeof(shet));
        kor21.new_plus("");

        if(i1 > 0)
         iceb_u_polen(koresp,bros,sizeof(bros),i+1,',');
        else
         strcpy(bros,koresp);      

        if(ff_prot != NULL)
          fprintf(ff_prot,"Взяли счёт-%s\n",bros);

        if(i1 > 0 && i == 0)
         i1--;
         
        /*Проверяем есть ли код контрагента*/
        if(iceb_u_polen(bros,shet,sizeof(shet),1,':') != 0)
         strcpy(shet,bros);
        else
         iceb_u_polen(bros,&kor21,2,':');

        if(ff_prot != NULL)
          fprintf(ff_prot,"Счёт-%s код контрагента-%s\n",shet,kor21.ravno());

        if(shet[0] == '\0')
         continue;

        if(kor21.ravno()[0] == '\0')
         { 
          sprintf(strsql,"%s%d",pktn,tbn);
          kor21.new_plus(strsql);
         }
        if(iceb_provsh(&kor21,shet,&shetv,avkvs,1,wpredok) != 0)
           continue;
        //Если счет не балансовый то никаких проводок для соц-фондов быть не может
        kolpr=2;
        if(shetv.stat == 1)
           kolpr=1; 

        if(ff_prot != NULL)
          fprintf(ff_prot,"\n-shet=%s shet1=%s kor21=%s kor11=%s deb=%.2f kre=%.2f\nshet_zamena=%s\n",shet,shet1,kor21.ravno(),kor11.ravno(),deb,kre,shet_zamena);

        if(kolpr == 2)
          iceb_zapmpr(g,m,d,shet,shet1,kor21.ravno(),kor11.ravno(),deb,kre,koment,kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
        if(kolpr == 1)
          iceb_zapmpr(g,m,d,shet,"",kor21.ravno(),kor11.ravno(),deb,kre,koment,kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);

        if(kolpr == 2)
         {
          strncpy(shet1,shet,sizeof(shet1)-1);
          kor11.new_plus(kor21.ravno());
         }
        kor21.new_plus("");
       }

     }

    if(kontr1[0] != '\0')
     {
      kor21.new_plus(kontr1);
     }

    if(kor21.getdlinna() <= 1)
     {
      sprintf(strsql,"%s%d",pktn,tbn);
      kor21.new_plus(strsql);
     }
//    printw("nahshet=%s shet1=%s kor21=%s kor11=%s deb=%f\n",
//    nahshet,shet1,kor21,kor11,deb);

    if(iceb_provsh(&kor21,nahshet,&shetv,avkvs,1,wpredok) != 0)
       continue;

    kolpr=2;
    if(shetv.stat == 1)
      kolpr=1;
    
    if(shet_zamena[0] != '\0')
      strncpy(nahshet,shet_zamena,sizeof(nahshet)-1);

    if(ff_prot != NULL)
      fprintf(ff_prot,"\n*nahshet=%s shet1=%s kor21=%s kor11=%s deb=%f kre=%f\n\
shet_zamena=%s\n",nahshet,shet1,kor21.ravno(),kor11.ravno(),deb,kre,shet_zamena);

    if(kolpr == 2)
      iceb_zapmpr(g,m,d,nahshet,shet1,kor21.ravno(),kor11.ravno(),deb,kre,koment,kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
    if(kolpr == 1)
      iceb_zapmpr(g,m,d,nahshet,"",kor21.ravno(),kor11.ravno(),deb,kre,koment,kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
 
   }
 }
//OSTANOV();
}

/**************************************/
/*Расчет проводок по социальным фондам до 1.1.2011*/
/**************************************/

void zaravprf_old(int tbn,short mp,short gp,class SQLCURSOR *cur_alx,
short *kolprov,
class iceb_u_spisok *sp_prov,
class iceb_u_double *sum_prov_dk,
FILE *ff_prot,GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row,row2;
char		strsql[1024];
char		kor11[64];
class iceb_u_str kor21("");
char		kontr1[64];
class iceb_u_str kontsocsh("");
class iceb_u_str kontsocsh1("");
class iceb_u_str kontsocsh2("");
short		kodz;
double		sum,sumi,sumas,sumasb,sumap,sumapb;
char		socshet[64]; /*Соц. счет если он один*/
char		socshet1[64]; /*Соц. счет не бюджетный*/
char		socshet2[64]; /*Соц. счет бюджетный*/
char		nahshet[64];
short		d,m,g;
char		koment[1024],koment1[1024];
double		deb,kre;
short		knah;
char		shet1[64],shet[64];
char		bros[1024];
char		koresp[512];
int		i,i1;
char		kodnv[512];
int		polekor; //Номер поля с кореспондирующими счетами
struct OPSHET   shetv;
int		kolpr=2;
short           metzmp=0; //Метка запуска проверки необлагаемой материальной помощи
double          sumnmp=0.; //Сумма необлагаемой материальной помощи
double          sumnmpz=0.; //Сумма необлагаемой материальной помощи
SQLCURSOR curr;
SQL_str row_alx;
char shet_zamena[64];
int kekv=kekv_nfo;
//printw("\nzaravprf %d\n",tbn);
if(ff_prot != NULL)
 fprintf(ff_prot,"\nВыполнение проводок на фонд зарплаты\n");
 
class iceb_u_double SUMA;
class iceb_u_spisok NAHIS;
if(zaravpt_osw(tbn,mp,gp,&NAHIS,&SUMA,wpredok) != 0)
 return;


/*Смотрим записи социальных отчислений*/
sprintf(strsql,"select kodz,shet,sumas,sumap,sumapb from Zarsocz where \
datz='%04d-%d-01' and tabn=%d",gp,mp,tbn);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
/*
printw("kolstr=%ld\n",kolstr);
OSTANOV();
*/
if(kolstr <= 0)
  return;

int kolzap=NAHIS.kolih();

while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"Запись фонда-%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);

  kodz=atoi(row[0]);
  memset(socshet,'\0',sizeof(socshet));
  memset(socshet1,'\0',sizeof(socshet1));
  memset(socshet2,'\0',sizeof(socshet2));
  strncpy(socshet,row[1],sizeof(socshet)-1);
  if(iceb_u_polen(socshet,socshet1,sizeof(socshet1)-1,1,',') == 0)
     iceb_u_polen(socshet,socshet2,sizeof(socshet2)-1,2,',');
  sumas=atof(row[2]);
  sumap=atof(row[3]);
  sumapb=atof(row[4]);
  sumasb=0.;
  if(sumapb != 0.)
   {
    sumasb=sumas*sumapb/sumap;
   }
  /*Берем коды не входящие в расчет*/
  memset(kodnv,'\0',sizeof(kodnv));
  sprintf(strsql,"select kodn from Zarsoc where kod=%d\n",kodz);
  if(sql_readkey(&bd,strsql,&row2,&curr) != 1)
   {
    class iceb_u_str repl;
    repl.plus(strsql);
    sprintf(strsql,"zaravpr-Не найден код соц. страхования %d !",kodz);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
   }  
  else
   strncpy(kodnv,row2[0],sizeof(kodnv)-1);
   
  sprintf(strsql,"%s%d",pktn,tbn);
  kontsocsh.new_plus(strsql);
  if(socshet1[0] == '\0' && socshet2[0] == '\0')
   if(iceb_provsh(&kontsocsh,socshet,&shetv,avkvs,0,wpredok) != 0)
     return;

  if(socshet1[0] != '\0')
   {
    sprintf(strsql,"%s%d",pktn,tbn);
    kontsocsh1.new_plus(strsql);
    if(iceb_provsh(&kontsocsh1,socshet1,&shetv,avkvs,0,wpredok) != 0)
     return;
   }
  if(socshet2[0] != '\0')
   {
    sprintf(strsql,"%s%d",pktn,tbn);
    kontsocsh2.new_plus(strsql);
    if(iceb_provsh(&kontsocsh2,socshet2,&shetv,avkvs,0,wpredok) != 0)
     return;
   }
  /*Повторно*/
  memset(kor11,'\0',sizeof(kor11));
  sumi=0.;
  sumnmp=sumnmpz;

  for(int nomz=0; nomz < kolzap; nomz++)
   {

    if(fabs(sumi) > fabs(sumas))
      break;

     if(sumas < 0. && sumi < sumas)
      break;

    sum=SUMA.ravno(nomz);
    char kodnah[20];
    char dataz[11];

//    printw("%s = %.2f\n",NAHIS.SPISOK_return(nomz),sum);

    iceb_u_polen(NAHIS.ravno(nomz),kodnah,sizeof(kodnah),3,'|');
    knah=atoi(kodnah);
    iceb_u_polen(NAHIS.ravno(nomz),dataz,sizeof(dataz),1,'|');

    /*Проверяем на коды не входящие в расчет */
    if(kodnv[0] != '\0')
     if(iceb_u_pole1(kodnv,kodnah,',',0,&i1) == 0 ||
      iceb_u_SRAV(kodnv,kodnah,0) == 0)
       continue;
       
    iceb_u_rsdat(&d,&m,&g,dataz,2);


    //Записей с кодами материальной помощи может быть несколько
    if(provkodw(kodmp,knah) >= 0)
     {
      if(metzmp == 0)
       {
        sumnmpz=sumnmp=0.;
//        sumnmpz=sumnmp=matpom1(tbn,mp,gp,kodmp,NULL,1,&matpomb,0);
        metzmp++;
       }
      if(sumnmp > 0.)
       {
        if(sumnmp > sum)
         {
          sumnmp-=sum;
         }
        else
         {
          sum-=sumnmp;
          sumnmp=0.;
         }
       }
     }

    memset(nahshet,'\0',sizeof(nahshet));
    iceb_u_polen(NAHIS.ravno(nomz),nahshet,sizeof(nahshet),2,'|');
    memset(koment,'\0',sizeof(koment));
    iceb_u_polen(NAHIS.ravno(nomz),koment,sizeof(koment),4,'|');
    kre=0.;

    if(fabs(sumi+sum) <= fabs(sumas))
     deb=sum;
    else
     {
      deb=sumas-sumi;
     }
    sumi+=sum;

//    printw("sumas=%f sumap=%f deb=%f\n",sumas,sumap,deb);

    deb=deb*sumap/sumas;
    //не округляем округлять будем при выполнении проводок
//    deb=iceb_okrug(deb,okrg);         

//    printw("deb=%f\n",deb);
//    OSTANOV();
         
    memset(kontr1,'\0',sizeof(kontr1));
    memset(kor11,'\0',sizeof(kor11));
    kor21.new_plus("");

    memset(shet1,'\0',sizeof(shet1));
    strncpy(shet1,socshet,sizeof(shet1)-1);
    
    strcpy(kor11,kontsocsh.ravno());
    if(socshet1[0] != '\0')
     {
      strncpy(shet1,socshet1,sizeof(shet1)-1);
      strcpy(kor11,kontsocsh1.ravno());
     }   
    if(socshet2[0] != '\0' && shetb != NULL)
      if(iceb_u_pole1(shetb,nahshet,',',0,&i1) == 0 || iceb_u_SRAV(shetb,nahshet,0) == 0)
       {
        strncpy(shet1,socshet2,sizeof(shet1)-1);
        strcpy(kor11,kontsocsh2.ravno());
       }
    if(fabs(deb) < 0.01)
     continue;

    memset(shet_zamena,'\0',sizeof(shet_zamena));

    /*Читаем файл настройки*/
    cur_alx->poz_cursor(0);
    while(cur_alx->read_cursor(&row_alx) != 0)
     {
//        printw("%s",row_alx[0]);
        if(row_alx[0][0] == '#')
          continue;        
        if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') != 0)
         continue;
        if(iceb_u_strstrm(bros,"f") == 0)
         continue;

        /*Берем код*/
        iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
        if((short)iceb_u_atof(bros) != kodz)
          continue;         

        polekor=4;
        // Смотрим настройку на комментарий
        iceb_u_pole(bros,koment1,2,'/');      
        if(koment1[0] != '\0')
         {
          if(iceb_u_strstrm(koment,koment1) != 0)
            polekor++;
         }
//        printw("polekor=%d koment1=%s row1[4]=%s\n",polekor,koment1,row1[4]);
//        OSTANOV();
        /*Берем счет*/
        memset(bros,'\0',sizeof(bros));
        iceb_u_pole(row_alx[0],bros,3,'|');
                
        memset(shet,'\0',sizeof(shet));
        if(iceb_u_pole(bros,shet,1,':') != 0)
         {
          strncpy(shet,bros,sizeof(shet)-1);
         }
        else
         {
          iceb_u_pole(bros,kontr1,2,':');
          //Проверяем для какого кода начисления настройка
          int kod_nah_nast=0;
          if(iceb_u_polen(bros,&kod_nah_nast,4,':') == 0)
           if(kod_nah_nast != 0 && kod_nah_nast != knah)
            continue;
         }        

        
        if(iceb_u_SRAV(shet,nahshet,1) != 0)
         {
          kontr1[0]='\0';
          continue;
         }

        if(ff_prot != NULL)
         {
          fprintf(ff_prot,"%s",row_alx[0]);
          fprintf(ff_prot,"Обнаружили настройкy на счёт-%s код контрагента=%s\n",shet,kontr1);
         }         
       //настройка на замену одного счета другим
       //делаем это только после сравнения счетов
       iceb_u_pole(bros,shet_zamena,3,':');
//       printw("\n***shet_zamena=%s\n",shet_zamena);
       if(shet_zamena[0] == '*')
        {
         SQLCURSOR cursh;
         SQL_str   rowsh;
         //взять счет из карточки
         sprintf(strsql,"select shet from Zarn where tabn=%d \
and god=%d and mes=%d",tbn,gp,mp);
         if(sql_readkey(&bd,strsql,&rowsh,&cursh) != 1)
          {
           class iceb_u_str repl;
           repl.plus("zaravprw");
           
           sprintf(strsql,"Не найдена настройка по дате %d.%dг. для определения счета в карточке !",mp,gp);
           repl.ps_plus(strsql);
           
           sprintf(strsql,"Табельный номер %d",tbn);
           repl.ps_plus(strsql);
           iceb_menu_soob(&repl,wpredok);
          }
         else
          strncpy(shet_zamena,rowsh[0],sizeof(shet_zamena)-1);
        }
         
        /*Берем кореспонденцию*/
        memset(koresp,'\0',sizeof(koresp));
        iceb_u_pole(row_alx[0],koresp,polekor,'|');
        if(koresp[0] == '\0')
         continue;
        if(ff_prot != NULL)
          fprintf(ff_prot,"Строка кореспондирующих счетов = %s\n",koresp);

        i1=iceb_u_pole2(koresp,',');
        for(i=0; i <= i1; i++)
         {
//          if(ff_prot != NULL)
//           fprintf(ff_prot,"i=%d i1=%d\n",i,i1);
           
          memset(shet,'\0',sizeof(shet));
          kor21.new_plus("");

          if(i1 > 0)
           iceb_u_pole(koresp,bros,i+1,',');
          else
           strcpy(bros,koresp);      

          if(ff_prot != NULL)
            fprintf(ff_prot,"Взяли счёт-%s\n",bros);

          if(i1 > 0 && i == 0)
           i1--;
           
          /*Проверяем есть ли код контрагента*/
          if(iceb_u_pole(bros,shet,1,':') != 0)
           strcpy(shet,bros);
          else
           iceb_u_polen(bros,&kor21,2,':');

          if(ff_prot != NULL)
            fprintf(ff_prot,"Счёт-%s код контрагента-%s\n",shet,kor21.ravno());

          if(shet[0] == '\0')
           continue;

          if(kor21.getdlinna() <= 1)
           {
            sprintf(strsql,"%s%d",pktn,tbn);
            kor21.new_plus(strsql);
           }
          if(iceb_provsh(&kor21,shet,&shetv,avkvs,1,wpredok) != 0)
             continue;
          //Если счет не балансовый то никаких проводок для соц-фондов быть не может
          kolpr=2;
          if(shetv.stat == 1)
             kolpr=1; 

          if(ff_prot != NULL)
            fprintf(ff_prot,"\n-shet=%s shet1=%s kor21=%s kor11=%s deb=%.2f kre=%.2f\n\
shet_zamena=%s\n",shet,shet1,kor21.ravno(),kor11,deb,kre,shet_zamena);

          if(kolpr == 2)
            iceb_zapmpr(g,m,d,shet,shet1,kor21.ravno(),kor11,deb,kre,koment,kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
          if(kolpr == 1)
            iceb_zapmpr(g,m,d,shet,"",kor21.ravno(),kor11,deb,kre,koment,kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);

          if(kolpr == 2)
           {
            strcpy(shet1,shet);
            strcpy(kor11,kor21.ravno());
           }

          kor21.new_plus("");
/*    
          printw("shet=%s shet1=%s kor11=%s kor21=%s kontr1=%s\n",
          shet,shet1,kor11,kor21,kontr1);
          OSTANOV();        
*/    
         }

     }

    if(kontr1[0] != '\0')
     {
      kor21.new_plus(kontr1);
     }

    if(kor21.getdlinna() <= 1)
     {
      sprintf(strsql,"%s%d",pktn,tbn);
      kor21.new_plus(strsql);
     }
//    printw("nahshet=%s shet1=%s kor21=%s kor11=%s deb=%f\n",
//    nahshet,shet1,kor21,kor11,deb);

    if(iceb_provsh(&kor21,nahshet,&shetv,avkvs,1,wpredok) != 0)
       continue;

    kolpr=2;
    if(shetv.stat == 1)
      kolpr=1;
    
    if(shet_zamena[0] != '\0')
      strncpy(nahshet,shet_zamena,sizeof(nahshet)-1);

    if(ff_prot != NULL)
      fprintf(ff_prot,"\n*nahshet=%s shet1=%s kor21=%s kor11=%s deb=%f kre=%f\n\
shet_zamena=%s\n",nahshet,shet1,kor21.ravno(),kor11,deb,kre,shet_zamena);

    if(kolpr == 2)
      iceb_zapmpr(g,m,d,nahshet,shet1,kor21.ravno(),kor11,deb,kre,koment,kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
    if(kolpr == 1)
      iceb_zapmpr(g,m,d,nahshet,"",kor21.ravno(),kor11,deb,kre,koment,kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
 
   }
 }
//OSTANOV();
}
#if 0
#############################################################3
/*****************************/
/*Программа формирования масивов объединенных одинаковых строк*/
/*Если строк несколько то при выполнении проводок по каждой строке*/
/*возникает погрешность при округлении до копеек . Если одинаковые строки объединить*/
/* то погрешность будет меньше.*/
/**************************************************/

int  zaravpt_os(int tbn,short mp,short gp,class iceb_u_spisok *data,class iceb_u_double *suma,GtkWidget *wpredok)
{
SQL_str row1;
int kolstr2;
char strsql[512];

SQLCURSOR cur1;
/*********
sprintf(strsql,"select datz,suma,shet,knah,kom from Zarp where \
datz >='%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and \
suma != 0. and prn='1' order by knah asc",
gp,mp,gp,mp,tbn);
***********/
sprintf(strsql,"select datz,suma,shet,knah,kom from Zarp where \
datz >='%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and \
suma != 0. and prn='1' order by suma asc",
gp,mp,gp,mp,tbn);

if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr2 <= 0)
  return(1);
int nomer=0;
while(cur1.read_cursor(&row1) != 0)
 {
  sprintf(strsql,"%s|%s|%s|%s|",row1[0],row1[2],row1[3],row1[4]);
  if((nomer=data->find(strsql)) < 0)
     data->plus(strsql);
  
  suma->plus(atof(row1[1]),nomer);
 }
 
return(0);
}
##################################################################33
#endif

