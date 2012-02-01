/* $Id: rasnalnw.c,v 1.18 2011-06-07 08:52:27 sasa Exp $ */
/*06.06.2011    22.01.1994      Белых А.И.      rasnalnw.c
Распечатка счет-фактур
*/
#include <stdlib.h>
#include        <errno.h>
#include	"../headers/buhg_g.h"
#define		KOLSTRLIST 105
#include <unistd.h>

void rasnalnw_old(short dd,short md,short gd,int skl,const char *nomdok,const char *imaf,short tipnn,GtkWidget *wpredok);
int rasnalnw21122010(short dd,short md,short gd,int skl,const char *nomdok,const char *imaf,short tipnn,GtkWidget *wpredok);

extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern SQL_baza bd;

int rasnalnw(short dd,short md,short gd,int skl,const char *nomdok,
const char *imaf,short tipnn,  //Тип документа
GtkWidget *wpredok)
{
if(iceb_u_sravmydat(dd,md,gd,10,1,2011) < 0)
 {
  rasnalnw_old(dd,md,gd,skl,nomdok,imaf,tipnn,wpredok);
  return(0);
 }
else
 return(rasnalnw21122010(dd,md,gd,skl,nomdok,imaf,tipnn,wpredok));

}

/*****************************************************/


int rasnalnw21122010(short dd,short md,short gd,int skl,const char *nomdok,
const char *imaf,short tipnn,  //Тип документа
GtkWidget *wpredok)
{
SQL_str         row,row1;
int		kolstr;
char		strsql[512];
FILE            *ff=NULL,*f1=NULL;
short           i;
double          itogo;
double          bb,bb2;
short           d,m,g;
char		datop[16];
double		sumt; /*Сумма по возвратной таре*/
short		vt;
double		sumkor;
char		bros[512];
char		kpos[32];
char		frop[512];
char		uspr[512];
char		osnov[512];
short		tipz;
char		nomnn[32]; /*Номер счет-фактуры*/
char		naim[512];
short		kgrm;
double		cena,kolih;
short		vtar;
char		ei[32];
char		str[512];
char		imaf1[56];
short		metz;
short		kolstdok=0;
//short		kollist=1;
short		dnn,mnn,gnn;
char kodop[56];
class iceb_u_str imaf_nast("matnast.alx");


sumt=0.;
vt=0;
memset(bros,'\0',sizeof(bros));
iceb_poldan("Код группы возвратная тара",bros,imaf_nast.ravno(),wpredok);
vt=(short)iceb_u_atof(bros);


SQLCURSOR cur,cur1;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gd,md,dd,skl,nomdok);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден документ"));
  repl.plus(" !!!");

  sprintf(strsql,"%s:%d.%d.%d %s:%s %s:%d",
  gettext("Дата"),
  dd,md,gd,
  gettext("Номер документа"),
  nomdok,
  gettext("Склад"),
  skl);
  
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);  
  return(1);
 }
float pnds=atof(row[13]);

strcpy(kpos,row[3]);
tipz=atoi(row[0]);
nomnn[0]='\0';
strncpy(nomnn,row[5],sizeof(nomnn)-1);
strncpy(kodop,row[6],sizeof(kodop)-1);
iceb_u_rsdat(&dnn,&mnn,&gnn,row[12],2);

sprintf(strsql,"select nomerz,sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d",
gd,nomdok,skl);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

memset(frop,'\0',sizeof(frop));
memset(uspr,'\0',sizeof(uspr));
memset(osnov,'\0',sizeof(osnov));
memset(datop,'\0',sizeof(datop));
sumkor=0.;


if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  metz=atoi(row[0]);
  if(metz == 3)
    strncpy(osnov,row[1],sizeof(osnov)-1);
  if(metz == 7)
    strncpy(uspr,row[1],sizeof(uspr)-1);
  if(metz == 8)
    strncpy(frop,row[1],sizeof(frop)-1);
  if(metz == 9)
   {
    iceb_u_rsdat(&d,&m,&g,row[1],0);
    if(d != 0 && iceb_u_sravmydat(d,m,g,dd,md,gd) > 0)
     sprintf(datop,"%02d.%02d.%d",d,m,g);
   }
  if(metz == 13)
   {
    sumkor=iceb_u_atof(row[1]);
    sumkor=iceb_u_okrug(sumkor,0.01);
   }
 }

if(datop[0] == '\0')
 {
  memset(bros,'\0',sizeof(bros));
  iceb_poldan("Дату отгрузки в счет-фактуре ставить всегда",bros,imaf_nast.ravno(),wpredok);
  if(iceb_u_SRAV(bros,"Вкл",1) == 0)
   sprintf(datop,"%02d.%02d.%d",dnn,mnn,gnn);
 }


int nomer_str=0;
//Печатаем шапку документа
if(iceb_nalnaks(imaf,&nomer_str,dd,md,gd,dnn,mnn,gnn,nomnn,osnov,uspr,frop,kpos,tipz,0,&f1,wpredok) != 0)
 return(1);
nomer_str+=iceb_insfil("nalnakst.alx",f1,wpredok);
//iceb_nalnakh(&nomer_str,f1);  

/*Готовим отсортированный файл*/

sortnakw(dd,md,gd,skl,nomdok,wpredok);

sprintf(imaf1,"sn%d.tmp",getpid());
if((ff = fopen(imaf1,"r")) == NULL)
if(errno != ENOENT) //Нет такого файла
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return(1);
 }
itogo=0.;
bb2=0.;
i=0;
kolstdok=nomer_str;
char razdel[8];
strcpy(razdel," I");
if(ff != NULL)
while(fgets(str,sizeof(str),ff) != NULL)
 {

//  iceb_u_pole(str,bros,1,'|');
//  kgrm=atoi(bros);

  iceb_u_pole(str,naim,1,'|');
//  naim=atoi(bros);
  
  iceb_u_pole(str,ei,3,'|');

  iceb_u_pole(str,bros,4,'|');
  kolih=atof(bros);

  iceb_u_pole(str,bros,5,'|');
  cena=atof(bros);
  cena=iceb_u_okrug(cena,okrcn);



//  iceb_u_pole(str,bros,8,'|');
//  vtar=atoi(bros);  


//  iceb_u_pole(str,bros,6,'|');
//  bb=atof(bros); 
  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);
  if(kgrm == vt || vtar == 1)
   {
    sumt+=bb;
    continue;
   }

  i++;
/********************
  kolstdok++;

  if(kolstdok >= KOLSTRLIST)
   {
    fprintf(f1,"\x1B\x33%c",20-kor);
    fprintf(f1,"\f %s %d.%d.%d%s %100s %s%d\n",
    nomnn,dd,md,gd,
    gettext("г."),
    " ",
    gettext("Лист N"),++kollist);
    iceb_insfil("nakls.alx",f1,wpredok);
    fprintf(f1,"\x1B\x33%c",24-kor);
    kolstdok=21; //На самом деле 14 но делаем больше чтобы уместился второй лист это связано с изменением межстрочного растояния при выводе второй шапки
   }
***********************/
  if(i == 2)
   {
    memset(datop,'\0',sizeof(datop));
    memset(razdel,'\0',sizeof(razdel));
   }    
  /*печатаем строку*/
  iceb_nalnak_str(0,tipnn,razdel,datop,naim,ei,kolih,cena,bb,f1);
  itogo+=bb;


 }
if(ff != NULL)
  fclose(ff);
unlink(imaf1);

/*Распечатываем услуги*/
sprintf(strsql,"select * from Dokummat3 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",gd,md,dd,skl,nomdok);
/*printw("\nstrsql=%s\n",strsql);*/

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

char naiusl[512];

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=iceb_u_okrug(cena,okrcn);

  if(kolih > 0)
    bb=cena*kolih;
  else
    bb=cena;

  bb=iceb_u_okrug(bb,okrg1);

  memset(naiusl,'\0',sizeof(naiusl));
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[10]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
     strncpy(naiusl,row1[0],sizeof(naiusl));
   }  
  if(naiusl[0] == '\0')
   strncpy(naiusl,row[7],sizeof(naiusl)-1);  
  else
   {
    strcat(naiusl," ");
    strcat(naiusl,row[7]);
   }
  iceb_nalnak_str(0,tipnn,"","",naiusl,ei,kolih,cena,bb,f1);
  itogo+=bb;
 }

//Печатаем концовку счет-фактуры
iceb_nalnake(tipz,tipnn,itogo,sumkor,"",0.,pnds,"matnast.alx",kodop,okrg1,sumt,f1,wpredok);
iceb_ustpeh(imaf,3,wpredok);
return(0);
}

/******************************************************************************/

void rasnalnw_old(short dd,short md,short gd,int skl,const char *nomdok,
const char *imaf,short tipnn,  //Тип документа
GtkWidget *wpredok)
{
SQL_str         row,row1;
int		kolstr;
char		strsql[512];
FILE            *ff=NULL,*f1=NULL,*f2=NULL;
short           i;
double          itogo;
double          bb,bb2;
short           d,m,g;
char		datop[16];
double		sumt; /*Сумма по возвратной таре*/
short		vt;
int		kor;
double		sumkor;
char		bros[512];
char		kpos[32];
char		frop[512];
char		uspr[512];
char		osnov[512];
short		tipz;
char		nomnn[32]; /*Номер счет-фактуры*/
char		naim[512];
short		kgrm;
double		cena,kolih;
short		vtar;
char		ei[32];
char		str[512];
char		imaf1[56];
short		metz;
short		kolstdok=0;
short		kollist=1;
short		dnn,mnn,gnn;
char kodop[56];
class iceb_u_str imaf_nast("matnast.alx");


sumt=0.;
vt=0;
memset(bros,'\0',sizeof(bros));
iceb_poldan("Код группы возвратная тара",bros,imaf_nast.ravno(),wpredok);
vt=(short)iceb_u_atof(bros);


SQLCURSOR cur,cur1;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gd,md,dd,skl,nomdok);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден документ"));
  repl.plus(" !!!");

  sprintf(strsql,"%s:%d.%d.%d %s:%s %s:%d",
  gettext("Дата"),
  dd,md,gd,
  gettext("Номер документа"),
  nomdok,
  gettext("Склад"),
  skl);
  
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);  
  return;
 }
float pnds=atof(row[13]);

strcpy(kpos,row[3]);
tipz=atoi(row[0]);
nomnn[0]='\0';
strncpy(nomnn,row[5],sizeof(nomnn)-1);
strncpy(kodop,row[6],sizeof(kodop)-1);
iceb_u_rsdat(&dnn,&mnn,&gnn,row[12],2);

sprintf(strsql,"select nomerz,sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d",
gd,nomdok,skl);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

memset(frop,'\0',sizeof(frop));
memset(uspr,'\0',sizeof(uspr));
memset(osnov,'\0',sizeof(osnov));
memset(datop,'\0',sizeof(datop));
sumkor=0.;


if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  metz=atoi(row[0]);
  if(metz == 3)
    strncpy(osnov,row[1],sizeof(osnov)-1);
  if(metz == 7)
    strncpy(uspr,row[1],sizeof(uspr)-1);
  if(metz == 8)
    strncpy(frop,row[1],sizeof(frop)-1);
  if(metz == 9)
   {
    iceb_u_rsdat(&d,&m,&g,row[1],0);
    if(d != 0 && iceb_u_sravmydat(d,m,g,dd,md,gd) > 0)
     sprintf(datop,"%02d.%02d.%d",d,m,g);
   }
  if(metz == 13)
   {
    sumkor=iceb_u_atof(row[1]);
    sumkor=iceb_u_okrug(sumkor,0.01);
   }
 }

if(datop[0] == '\0')
 {
  memset(bros,'\0',sizeof(bros));
  iceb_poldan("Дату отгрузки в счет-фактуре ставить всегда",bros,imaf_nast.ravno(),wpredok);
  if(iceb_u_SRAV(bros,"Вкл",1) == 0)
   sprintf(datop,"%02d.%02d.%d",dd,md,gd);
 }


int nomer_str=0;
//Печатаем шапку документа
iceb_rasnaln1(imaf,&nomer_str,&kor,dd,md,gd,dnn,mnn,gnn,nomnn,osnov,uspr,frop,kpos,tipz,&f1,&f2,wpredok);


/*Готовим отсортированный файл*/

sortnakw(dd,md,gd,skl,nomdok,wpredok);

sprintf(imaf1,"sn%d.tmp",getpid());
if((ff = fopen(imaf1,"r")) == NULL)
if(errno != ENOENT) //Нет такого файла
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }
itogo=0.;
bb2=0.;
i=0;
kolstdok=nomer_str;
char razdel[8];
strcpy(razdel," I");
if(ff != NULL)
while(fgets(str,sizeof(str),ff) != NULL)
 {

//  iceb_u_pole(str,bros,1,'|');
//  kgrm=atoi(bros);

  iceb_u_pole(str,naim,1,'|');

  iceb_u_pole(str,ei,3,'|');

  iceb_u_pole(str,bros,4,'|');
  kolih=atof(bros);

  iceb_u_pole(str,bros,5,'|');
  cena=atof(bros);
  cena=iceb_u_okrug(cena,okrcn);

//  iceb_u_pole(str,bros,8,'|');
//  vtar=atoi(bros);  



  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);
  if(kgrm == vt || vtar == 1)
   {
    sumt+=bb;
    continue;
   }

  i++;
  kolstdok++;
  if(kolstdok >= KOLSTRLIST)
   {
    fprintf(f1,"\x1B\x33%c",20-kor);
    fprintf(f1,"\f %s %d.%d.%d%s %100s %s%d\n",
    nomnn,dd,md,gd,
    gettext("г."),
    " ",
    gettext("Лист N"),++kollist);
    iceb_insfil("nakls.alx",f1,wpredok);
    fprintf(f1,"\x1B\x33%c",24-kor);
    kolstdok=21; //На самом деле 14 но делаем больше чтобы уместился второй лист это связано с изменением межстрочного растояния при выводе второй шапки
   }

  if(i == 2)
   {
    memset(datop,'\0',sizeof(datop));
    memset(razdel,'\0',sizeof(razdel));
   }    
  fprintf(f1,"|%-4s|%-10s|%-*.*s|%-*.*s|%9.9g|%10s",
  razdel,datop,
  iceb_u_kolbait(40,naim),iceb_u_kolbait(40,naim),naim,
  iceb_u_kolbait(7,ei),iceb_u_kolbait(7,ei),ei,
  kolih,iceb_prcn(cena));

  if(tipnn == 0)
    fprintf(f1,"|%9s|%9s|%9s|%9s|%11s|\n",
     iceb_prcn(bb)," "," "," "," ");

  if(tipnn == 1)
    fprintf(f1,"|%9s|%9s|%9s|%9s|%11s|\n",
    " ",iceb_prcn(bb)," "," "," ");

  if(tipnn == 2)
    fprintf(f1,"|%9s|%9s|%9s|%9s|%11s|\n",
    " "," ",iceb_prcn(bb)," "," ");

  if(tipnn == 3)
    fprintf(f1,"|%9s|%9s|%9s|%9s|%11s|\n",
    " "," "," ",iceb_prcn(bb)," ");

  if(iceb_u_strlen(naim) > 40)
   fprintf(f1,"|%4s|%-10s|%-*.*s|%-7.7s|%9.9s|%10s|%9s|%9s|%9s|%9s|%11s|\n",
    " "," ",
    iceb_u_kolbait(40,iceb_u_adrsimv(40,naim)),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,naim)),
    iceb_u_adrsimv(40,naim),
    " "," "," "," "," "," "," "," ");

  itogo+=bb;


 }
if(ff != NULL)
  fclose(ff);
unlink(imaf1);

/*Распечатываем услуги*/
sprintf(strsql,"select * from Dokummat3 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",gd,md,dd,skl,nomdok);
/*printw("\nstrsql=%s\n",strsql);*/

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
char naiusl[500];

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=iceb_u_okrug(cena,okrcn);

  if(kolih > 0)
    bb=cena*kolih;
  else
    bb=cena;

  bb=iceb_u_okrug(bb,okrg1);

  memset(naiusl,'\0',sizeof(naiusl));
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[10]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
     strncpy(naiusl,row1[0],sizeof(naiusl));
   }  
  if(naiusl[0] == '\0')
   strncpy(naiusl,row[7],sizeof(naiusl)-1);  
  else
   {
    strcat(naiusl," ");
    strcat(naiusl,row[7]);
   }

  fprintf(f1,"|%4s|%10s|%-*.*s|%-*.*s|%9.9g|%10s",
  " "," ",
  iceb_u_kolbait(40,naiusl),iceb_u_kolbait(40,naiusl),naiusl,
  iceb_u_kolbait(7,row[3]),iceb_u_kolbait(7,row[3]),row[3],
  kolih,iceb_prcn(cena));

  if(tipnn == 0)
    fprintf(f1,"|%9s|%9s|%9s|%9s|%11s|\n",
     iceb_prcn(bb)," "," "," "," ");

  if(tipnn == 1)
    fprintf(f1,"|%9s|%9s|%9s|%9s|%11s|\n",
    " ",iceb_prcn(bb)," "," "," ");

  if(tipnn == 2)
    fprintf(f1,"|%9s|%9s|%9s|%9s|%11s|\n",
    " "," ",iceb_prcn(bb)," "," ");

  if(tipnn == 3)
    fprintf(f1,"|%9s|%9s|%9s|%9s|%11s|\n",
    " "," "," ",iceb_prcn(bb)," ");

  itogo+=bb;
 }

//Печатаем концовку счет-фактуры
iceb_rasnaln2(nomer_str,kor,tipnn,itogo,sumkor,"",0.,pnds,"matnast.alx",kodop,okrg1,sumt,f1,f2,wpredok);

}
