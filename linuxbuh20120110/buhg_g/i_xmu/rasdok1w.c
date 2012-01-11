/*$Id: rasdok1w.c,v 1.20 2011-08-29 07:13:43 sasa Exp $*/
/*16.08.2011	09.07.2003	Белых А.И.	rasdok1w.c
Распечатка накладных с учетной ценой
*/
#include        <stdlib.h>
#include        <errno.h>
#include	"../headers/buhg_g.h"
#include        <unistd.h>
#define		KOLSTROK 77

void	vztr1(short,short,short,int,const char*,double*,FILE*,int*,int*,int,GtkWidget*);
void    srasdok1(int,int*,int*,FILE*);
void    shrasdok1(int,int*,int*,int,FILE*);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern short    srtnk; /*0-не включена 1-включена сортировка записей в накладной*/
extern short	vtara; /*Код группы возвратная тара*/
extern SQL_baza bd;
extern int kol_strok_na_liste;

void rasdok1w(short dg,short mg,short gg, //дата документа
int skl,  //Склад
const char *nomdok, //Номер документа
short lnds,  //Льготы по НДС*
double ves, //Вес
GtkWidget *wpredok)
{
char		kontr[64];
char		kodop[64];
int		tipz=0;
char		naimpr[512];
char		strsql[512];
char		bros[512];
char		bros1[512];
SQL_str		row,row1;
char		nomnn[64]; /*Номер налоговой накладной*/
char		nn1[64]; /*Номер встречного документа*/
short		mtpr=0; /*Метка первого и второго прохода*/
short		mtar=0; /*Метка наличия тары в накладной*/
char		stroka[1024];
char		kdtr[112]; /*Коды тары*/
short		kgrm=0; /*код группы материалла*/
char		naim[512];
int             kodm,nk;
double		kratn,cenan;
short		vtar=0; /*Метка возвратной тары*/
short		mnt=0; /*Метка наличия тары*/
double		mest=0.;
int		i1;
int		kollist=0,kolstrok=0;
char		ein[64];
char		shu[64];
double		kolih=0.;
double		cenareal=0.; //Цена реализации
double		suma=0.;
double		itogo=0.;
double		itogoreal=0.;
int		kolstr=0;
double		sumkor=0.;
double		sumt=0.;
int		nomer=0;
char		artikul[512];
char		kodkontr00[64];

int kolstrok_nl=iceb_kolstrok(KOLSTROK);
SQLCURSOR cur,cur1;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gg,mg,dg,skl,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
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
strcpy(kontr,row[3]);
strcpy(kodop,row[6]);
tipz=atoi(row[0]);
nomnn[0]='\0';
strncpy(nomnn,row[5],sizeof(nomnn)-1);
nn1[0]='\0';
strncpy(nn1,row[11],sizeof(nn1)-1);

int mvnpp=0;
memset(naimpr,'\0',sizeof(naimpr));
/*Читаем наименование операции*/
if(tipz == 1)
 strcpy(bros,"Prihod");
if(tipz == 2)
 strcpy(bros,"Rashod");

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros,kodop);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код операции"));
  repl.plus(" ");
  repl.plus(kodop);
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);  
 }
else
 {

  mvnpp=atoi(row[1]);
  strncpy(naimpr,row[0],sizeof(naimpr)-1);
  
 }

short kor=0;
memset(bros,'\0',sizeof(bros)); 
iceb_poldan("Корректор межстрочного растояния",bros,"matnast.alx",wpredok);
kor=(short)iceb_u_atof(bros);

short ots=0;
memset(bros,'\0',sizeof(bros)); 
iceb_poldan("Отступ от левого края",bros,"matnast.alx",wpredok);
ots=(short)iceb_u_atof(bros);

memset(kdtr,'\0',sizeof(kdtr));
iceb_poldan("Код тары",kdtr,"matnast.alx",wpredok);

strcpy(kodkontr00,"00");
if(tipz == 2)
 {
  memset(bros,'\0',sizeof(bros));
  if(iceb_poldan(kodop,bros,"matnast.alx",wpredok) == 0)
   {
    memset(kodkontr00,'\0',sizeof(kodkontr00));
    strncpy(kodkontr00,bros,sizeof(kodkontr00)-1);
   }
 }



sortnakw(dg,mg,gg,skl,nomdok,wpredok);

char imaftmp[64];
char imafdok[64];
FILE *fftmp;
FILE *ffdok;

sprintf(imaftmp,"sn%d.tmp",getpid());
if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return;
 }

sprintf(imafdok,"nsuc%d.lst",getpid());
if((ffdok = fopen(imafdok,"w")) == NULL)
 {
  iceb_er_op_fil(imafdok,"",errno,wpredok);
  return;
 }

rashapw(tipz,&kolstrok,kodkontr00,kontr,ots,kor,ffdok,wpredok);


char naiskl[512];
char fmoll[512];
memset(naiskl,'\0',sizeof(naiskl));
memset(fmoll,'\0',sizeof(fmoll));

/*Читаем наименование склада*/
sprintf(strsql,"select naik,fmol from Sklad where kod=%d",
skl);

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
fprintf(ffdok,"\n%s\n",naiskl);
kolstrok+=2;

if(mvnpp == 1) /*Внутреннее перемещение*/
 {
  if(iceb_u_atof(kontr) == 0. && iceb_u_pole(kontr,bros,2,'-') == 0 && kontr[0] == '0')
   {
    iceb_u_pole(kontr,bros,2,'-');
    int skl1=(int)iceb_u_atof(bros);
    /*Читаем наименование склада*/
    sprintf(strsql,"select naik,fmol from Sklad where kod=%d",
    skl1);
    naiskl[0]='\0';
    char fmol[512];
    memset(fmol,'\0',sizeof(fmol));
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не найден код склада"));
      repl.plus(" ");
      repl.plus(skl1);
      repl.plus(" !");
      iceb_menu_soob(&repl,wpredok);  
     }
    else
     {
      strncpy(naiskl,row[0],sizeof(naiskl)-1);
      strncpy(fmol,row[1],sizeof(fmol)-1);
     }
    if(tipz == 1)
       fprintf(ffdok,"\n\
%s:\n%s %d <= %s %d %s\n\
%s <= %s\n",
       naimpr,gettext("Склад"),skl,
       gettext("Склад"),skl1,naiskl,fmoll,fmol);
    if(tipz == 2)
       fprintf(ffdok,"\n\
%s:\n%s %d => %s %d %s\n\
%s => %s\n",
       naimpr,gettext("Склад"),skl,
       gettext("Склад"),skl1,naiskl,fmoll,fmol);
    kolstrok+=3;

   }
  else
   {
    fprintf(ffdok,"\n%s\n",naimpr);
    kolstrok+=2;
   }
 }
else
 { 
  bros[0]='\0';

  if(iceb_poldan("Наименование операции в накладной",bros,"matnast.alx",wpredok) == 0)
   if(iceb_u_SRAV(bros,"Вкл",1) == 0)
    {
     fprintf(ffdok,"\n%s\n",naimpr);
     kolstrok+=2;
    }   
 }

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

if(tipz == 1)
  fprintf(ffdok,"\n\
              %s %s %s %02d.%02d.%d%s\n",
  gettext("ПРИХОДНАЯ НАКЛАДНАЯ"),bros,
  gettext("от"),dg,mg,gg,gettext("г."));

if(tipz == 2)
  fprintf(ffdok,"\n\
              %s %s %s %02d.%02d.%d%s\n",
  gettext("РАСХОДНАЯ НАКЛАДНАЯ"),bros,
  gettext("от"),dg,mg,gg,gettext("г."));
kolstrok+=2;

/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(ffdok,gettext("Доверенность"));
  fprintf(ffdok," N");
  iceb_u_pole(row[0],bros,1,'#');
  fprintf(ffdok," %s",bros);
  iceb_u_pole(row[0],bros,2,'#');
  fprintf(ffdok," %s %s\n",gettext("от"),bros);
  kolstrok++;
 }

/*Читаем через кого*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,2);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(ffdok,"%s: %s\n",gettext("Через кого"),row[0]);
  kolstrok++;
 }

if(tipz == 2)
 {
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,3);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    fprintf(ffdok,"%s: %s\n",gettext("Основание"),row[0]);
    kolstrok++;
   }
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,9);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    short d,m,g;
    iceb_u_rsdat(&d,&m,&g,row[0],0);
    if(d == 0 || iceb_u_sravmydat(d,m,g,dg,mg,gg) >= 0)
     {
      fprintf(ffdok,"\x1B\x45%s: %s\x1B\x45\x1b\x46\n",
      gettext("Конечная дата расчета"),row[0]);
      kolstrok++;
     }
   }

  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,10);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)

  if(nomnn[0] != '\0')
   {
    fprintf(ffdok,"%s: %s\n",
    gettext("Номер налоговой накладной"),nomnn);
    kolstrok++;
   }    
 }

if(nn1[0] != '\0' && tipz == 1)
 {
  fprintf(ffdok,"%s: %s\n",
  gettext("Номер встречной накладной"),nn1);
  kolstrok++;
 }
if(nn1[0] != '\0' && tipz == 2 && mvnpp > 0 )
 {
  fprintf(ffdok,"%s: %s\n",
  gettext("Номер встречной накладной"),nn1);
  kolstrok++;
 }

fprintf(ffdok,"\x1B\x4D"); /*12-знаков*/
//fprintf(ffdok,"\x0F");  /*Ужатый режим*/

srasdok1(tipz,&kollist,&kolstrok,ffdok);

naz:;

while(fgets(stroka,sizeof(stroka),fftmp) != NULL)
 {
  iceb_u_pole(stroka,bros,1,'|');
  kgrm=atoi(bros);
  
  iceb_u_polen(stroka,naim,sizeof(naim),2,'|');

  iceb_u_pole(stroka,bros,3,'|');
  kodm=atoi(bros);

  iceb_u_pole(stroka,bros,4,'|');
  nk=atoi(bros);

  iceb_u_pole(stroka,bros,5,'|');
  cenan=atof(bros);
  cenan=iceb_u_okrug(cenan,okrcn);
  
  iceb_u_pole(stroka,bros,6,'|');
  kratn=atof(bros);

  iceb_u_pole(stroka,ein,7,'|');

  iceb_u_pole(stroka,bros,8,'|');
  vtar=atoi(bros);  

  iceb_u_pole(stroka,shu,9,'|');

  iceb_u_pole(stroka,bros,10,'|');
  kolih=atof(bros);

  memset(artikul,'\0',sizeof(artikul));
  iceb_u_polen(stroka,artikul,sizeof(artikul),12,'|');

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
  sprintf(bros,"%d",kgrm);
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
  mest=0.;
  if(kratn != 0.)
   {
    mest=kolih/kratn;
    mest=iceb_u_okrug(mest,0.1);
   }

  suma=cenan*kolih;
  suma=iceb_u_okrug(suma,okrg1);

  itogo+=suma;
  

  cenareal=0.;
  if(tipz == 1)
   {
    //Читаем цену реализации в списке товаров
    if(mvnpp == 1) /*Внутреннее перемещение*/
     {
      //Читаем учетную цену в карточке
      sprintf(strsql,"select cenap from Kart where sklad=%d and nomk=%d",skl,nk);
      if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
         cenareal=atof(row[0]);    
     }
    else
     {
      sprintf(strsql,"select cenapr from Material where kodm=%d",kodm);
      if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
       cenareal=atof(row[0]);    
     }
   }
  if(tipz == 2)
   {
    //Читаем учетную цену в карточке
    sprintf(strsql,"select cena,cenap from Kart where sklad=%d and nomk=%d",skl,nk);
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     {
      if(mvnpp == 1) /*Внутреннее перемещение*/
       cenareal=atof(row[1]);    
      else
       cenareal=atof(row[0]);    
     }    
   }
  double suma1=cenareal*kolih;  
  suma1=iceb_u_okrug(suma1,okrg1);

  itogoreal+=suma1;

  if(artikul[0] != '\0')
   {
    sprintf(strsql," %s:%s",gettext("Артикул"),artikul);
    strcat(naim,strsql);
   }

  shrasdok1(tipz,&kollist,&kolstrok,kolstrok_nl,ffdok);

  fprintf(ffdok,"%2d %5d %-*.*s %-*.*s %8s %10.10g",
  ++nomer,
  kodm,
  iceb_u_kolbait(25,naim),iceb_u_kolbait(25,naim),naim,
  iceb_u_kolbait(4,ein),iceb_u_kolbait(4,ein),ein,
  iceb_prcn(cenan),kolih);
  
  fprintf(ffdok," %10s %8.2f",
  iceb_prcn(suma),cenareal);

  rasdokkr(kolih,mest,kratn,ffdok);
  if(iceb_u_strlen(naim) > 25)
   {
    shrasdok1(tipz,&kollist,&kolstrok,kolstrok_nl,ffdok);
    fprintf(ffdok,"%2s %5s %s\n"," "," ",iceb_u_adrsimv(25,naim));
   }  

 }
 
mtpr++;
/*
printw("mtpr=%d kdtr=%s mtar=%d\n",mtpr,kdtr,mtar);
OSTANOV();
*/
if(mtpr == 1 && kdtr[0] != '\0' && mtar != 0) /*Распечатываем отдельно тару*/
 {
  kolstrok++;

  fprintf(ffdok,"\
- - - - - - - - - - - - - - - - - - - - %s - -- - - - - - - - - - - - - - - - - - - - - \n",
    gettext("Т а р а"));

  rewind(fftmp);
  goto naz;
 }

fclose(fftmp);
unlink(imaftmp);

/*Распечатываем услуги*/
sprintf(strsql,"select * from Dokummat3 where \
datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",gg,mg,dg,skl,nomdok);
/*printw("\nstrsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
char naiusl[512];

while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[4]);
  cenan=atof(row[5]);
  cenan=iceb_u_okrug(cenan,okrcn);

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
  
  if(kolih > 0)
    suma=cenan*kolih;
  else
    suma=cenan;
  
  suma=iceb_u_okrug(suma,okrg1);

  shrasdok1(tipz,&kollist,&kolstrok,kolstrok_nl,ffdok);

  fprintf(ffdok,"%2d %5s %-*.*s %-*.*s %8s %10.10g %8s\n",
  ++nomer," ",
  iceb_u_kolbait(25,naiusl),iceb_u_kolbait(25,naiusl),naiusl,
  iceb_u_kolbait(4,row[3]),iceb_u_kolbait(4,row[3]),row[3],
  iceb_prcn(cenan),kolih,iceb_prcn(suma));
  if(iceb_u_strlen(naiusl) > 25)
   {
    shrasdok1(tipz,&kollist,&kolstrok,kolstrok_nl,ffdok);
    fprintf(ffdok,"%2s %5s %s\n"," "," ",iceb_u_adrsimv(25,naiusl));
   }  

  itogo+=suma;

 }
 
fprintf(ffdok,"\
-------------------------------------------------------------------------------------------\n");
fprintf(ffdok,"\
%*s: %10s",iceb_u_kolbait(58,gettext("Итого")),gettext("Итого"),iceb_prcn(itogo));
fprintf(ffdok,"(%8s)\n",iceb_prcn(itogoreal));

double itogo1=itogo;
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

  double bb=0.;

  if(sumkor != 0)
   {
    if(sumkor > 0.)
      bb=sumkor*100./itogo;

    if(sumkor < 0.)
      bb=100*sumkor/(itogo-sumkor*-1);
    bb=iceb_u_okrug(bb,0.1);
    if(sumkor < 0)
      sprintf(kodop,"%s %.1f%% :",gettext("Снижка"),bb*(-1));
    if(sumkor > 0)
      sprintf(kodop,"%s %.1f%% :",gettext("Надбавка"),bb);

    fprintf(ffdok,"%*s %10.2f/З %s %.2f\n",
    iceb_u_kolbait(59,kodop),kodop,sumkor,gettext("НДС"),sumkor+sumkor*pnds/100.);


    if(sumkor < 0)
      sprintf(kodop,gettext("Итого со снижкой :"));
    if(sumkor > 0)
      sprintf(kodop,gettext("Итого с надбавкой :"));

    fprintf(ffdok,"%*s %10.2f\n",iceb_u_kolbait(59,kodop),kodop,sumkor+itogo);

   }

  bb=0.;

  if(lnds == 0 )
   {
   
    sprintf(kodop,"%s %.2f%% :",gettext("НДС"),pnds);
    bb=(itogo+sumkor)*pnds/100.;
    if(tipz == 1) //Читаем сумму НДС введенную вручную
     {
      sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d \
and nomd='%s' and nomerz=6",gg,skl,nomdok);
      if(sql_readkey(&bd,strsql,&row,&cur) == 1)
        bb=atof(row[0]);
     }

    bb=iceb_u_okrug(bb,0.01);
    fprintf(ffdok,"%*s %10.2f\n",iceb_u_kolbait(59,kodop),kodop,bb);
   }

  sumt=0.;

  if(mnt != 0)
     vztr1(dg,mg,gg,skl,nomdok,&sumt,ffdok,&kollist,&kolstrok,tipz,wpredok);
        
  itogo1=(itogo+sumkor)+bb+sumt;
  strcpy(bros,gettext("Итого к оплате:"));
  if(tipz == 1)
    strcpy(bros,gettext("Итого:"));

  fprintf(ffdok,"%*s %10.2f\n",iceb_u_kolbait(59,bros),bros,itogo1);
 }
else
 { 
  sumt=0.;
  if(mnt != 0)
     vztr1(dg,mg,gg,skl,nomdok,&sumt,ffdok,&kollist,&kolstrok,tipz,wpredok);
  itogo1+=sumt;
 }

memset(strsql,'\0',sizeof(strsql));
iceb_u_preobr(itogo1,strsql,0);

fprintf(ffdok,"%s:%s\n",gettext("Сумма прописью"),strsql);
fprintf(ffdok,"\x12");  /*Нормальный режим печати*/
fprintf(ffdok,"\x1B\x50"); /*10-знаков*/
fprintf(ffdok,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/

fprintf(ffdok,"\n\%s%s\n",gettext("\
Руководитель__________________        Главный  бухгалтер_______________\n\n"),
gettext("\
Отпустил______________________        Принял__________________________"));

iceb_podpis(ffdok,wpredok);

fclose(ffdok);

iceb_u_spisok imafils;
iceb_u_spisok naimf;
imafils.plus(imafdok);
naimf.plus(gettext("Распечатка накладной с ценами учета/реализации"));
iceb_ustpeh(imafdok,1,wpredok);

iceb_rabfil(&imafils,&naimf,"",0,wpredok);

}
/*******************************************/
/* Шапка документа                         */
/*******************************************/

void srasdok1(int tipz,int *kollist,int *kolstrok,FILE *ffdok)
{

*kollist+=1;
*kolstrok+=5;

fprintf(ffdok,"%75s%s N%d\n","",gettext("Лист"),*kollist);

fprintf(ffdok,"\
-------------------------------------------------------------------------------------------\n");

if(tipz == 1)
 {
  fprintf(ffdok,gettext("\
N |Код  |Наименование товара,     |Ед. |Цена пр-|Количество|  Сумма   |  Цена  |Кол.|Крат-|\n"));
  fprintf(ffdok,gettext("\
  |ма-ла|      материалла         |изм.|бретения|          |          |реализац|м-ст|ность|\n"));
 }

if(tipz == 2)
 {
  fprintf(ffdok,gettext("\
N |Код  |Наименование товара,     |Ед. |  Цена  |Количество|  Сумма   |  Цена  |Кол.|Крат-|\n"));
  fprintf(ffdok,gettext("\
  |ма-ла|      материалла         |изм.|реализац|          |          |учетная |м-ст|ность|\n"));
 }

fprintf(ffdok,"\
-------------------------------------------------------------------------------------------\n");

}
/****************************/
/*Счетчик                   */
/****************************/
void   shrasdok1(int tipz,int *kollist,int *kolstrok,int kolstrok_nl,FILE *ffdok)
{
*kolstrok+=1;
if(*kolstrok <= kolstrok_nl)
 return;

fprintf(ffdok,"\f");
*kolstrok=0;
srasdok1(tipz,kollist,kolstrok,ffdok);
*kolstrok+=1; //Плюс следующая за шапкой строка

}
/*****************/
/*Распечатка тары*/
/*****************/

void vztr1(short dd,short md,short gd,int skl,const char *nomdok,
double *sumt,FILE *ffdok,int *kollist,int *kolstrok,int tipz,GtkWidget *wpredok)
{
double		itogo;
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
double		kolih,cenan;
int		nk,kgrm;
char		naim[512];
char		shu[32];
short		vtr;
int		i;
double		cenareal=0.;
int		kodm=0;
double		suma=0.;

sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 { 
  return;
 } 
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
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",
  row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
    {
     printf("vztr1-Не нашли наименование материалла по коду %s\n",row[4]);
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
  kodm=atoi(row[4]);
  kolih=atof(row[5]);
  cenan=atof(row[6]);
  cenan=iceb_u_okrug(cenan,okrcn);

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

  suma=cenan*kolih;
  suma=iceb_u_okrug(suma,okrg1);
  i++;
  if(i == 1)
   {   
//    shrasdok1(tipz,kollist,kolstrok,ffdok);
    fprintf(ffdok,"%s:\n",gettext("Оборотная тара"));
   }

//  shrasdok1(tipz,kollist,kolstrok,ffdok);

  cenareal=0.;
  if(tipz == 1)
   {
    //Читаем цену реализации в списке товаров
    sprintf(strsql,"select cenapr from Material where kodm=%d",kodm);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     cenareal=atof(row1[0]);    
   }
  if(tipz == 2)
   {
    //Читаем учетную цену в карточке
    sprintf(strsql,"select cena from Kart where sklad=%d and nomk=%d",skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     cenareal=atof(row1[0]);    
    
   }

  fprintf(ffdok,"%8d %-*.*s %-*s %10s %10.10g",
  kodm,
  iceb_u_kolbait(40,naim),iceb_u_kolbait(40,naim),naim,
  iceb_u_kolbait(4,row[7]),row[7],
  iceb_prcn(cenan),kolih);
  
  fprintf(ffdok," %10s %10.2f\n",
  iceb_prcn(suma),cenareal);
  if(iceb_u_strlen(naim) > 40)
   {
    fprintf(ffdok,"%8s %s\n"," ",iceb_u_adrsimv(40,naim));
   }  


  itogo+=suma;

 }
*sumt=itogo;
}
