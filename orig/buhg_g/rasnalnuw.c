/* $Id: rasnalnuw.c,v 1.16 2011-06-07 08:52:26 sasa Exp $ */
/*06.06.2011    22.01.1994      Белых А.И.      rasnalnuw.c
Распечатка налоговых накладных для услуг
*/
#include <stdlib.h>
#include	"buhg_g.h"

void rasnalnuw_old(short dd,short md,short gd,int podr,const char *nomdok,const char *imaf,short tipnn,GtkWidget *wpredok);
int rasnalnuw21122010(short dd,short md,short gd,int podr,const char *nomdok,const char *imaf,short tipnn,GtkWidget *wpredok);

extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern SQL_baza bd;

int rasnalnuw(short dd,short md,short gd,int podr,
const char *nomdok,const char *imaf,
short tipnn, //Тип накладной
GtkWidget *wpredok)
{
if(iceb_u_sravmydat(dd,md,gd,10,1,2011) < 0)
 {
  rasnalnuw_old(dd,md,gd,podr,nomdok,imaf,tipnn,wpredok);
  return(0);
 }
else
  return(rasnalnuw21122010(dd,md,gd,podr,nomdok,imaf,tipnn,wpredok));

}
/**********************************************************************/

int rasnalnuw21122010(short dd,short md,short gd,int podr,
const char *nomdok,const char *imaf,
short tipnn, //Тип накладной
GtkWidget *wpredok)
{
SQL_str         row,row1;
int		kolstr;
char		strsql[512];
FILE            *f1;
char            bros1[512];
short           i;
double          itogo;
double          bb,bb2;
char		datop[16];
double		sumkor;
char		kpos[32];
char		frop[512];
char		uspr[512];
char		osnov[512];
int		tipz;
char		nomnn[32]; /*Номер налоговой накладной*/
char		naim[512];
double		cena,kolih;
char		ei[32];
short 		metka;
int		kodzap;
short		d,m,g;
short		dnn,mnn,gnn; //Дата налоговой накладной
char		kodop[32];
float		procent=0.;
class iceb_u_str naimnal("");
printf("%s\n",__FUNCTION__);

float pnds=0.;

SQLCURSOR curr;		
SQLCURSOR cur;		


/*Читаем шапку документа*/
sprintf(strsql,"select kontr,tp,sumkor,datop,nomnn,forop,uslpr,osnov,datpnn,kodop,pn \
from Usldokum where datd='%04d-%02d-%02d' and podr=%d and nomd='%s'",
gd,md,dd,podr,nomdok);

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  iceb_u_str repl;
  
  repl.plus("rasnalnu-Не найден документ !"),
  
  sprintf(strsql,"Дата:%d.%d.%d Документ:%s Подразделение:%d",
  dd,md,gd,nomdok,podr);
  
  repl.ps_plus(strsql);
  
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }
pnds=atof(row[10]);
memset(frop,'\0',sizeof(frop));
memset(uspr,'\0',sizeof(uspr));
memset(osnov,'\0',sizeof(osnov));
memset(datop,'\0',sizeof(datop));
sumkor=0.;

strncpy(kpos,row[0],sizeof(kpos)-1);
tipz=atoi(row[1]);
sumkor=atof(row[2]);
if(row[3][0] != '0')
 {
  iceb_u_rsdat(&d,&m,&g,row[3],2);
  sprintf(datop,"%02d.%02d.%d",d,m,g);
 }
iceb_u_rsdat(&dnn,&mnn,&gnn,row[8],2);
if(datop[0] == '\0')
 {
  
  memset(strsql,'\0',sizeof(strsql));
  iceb_poldan("Дату отгрузки в налоговой накладной ставить всегда",strsql,"matnast.alx",wpredok);
  if(iceb_u_SRAV(strsql,"Вкл",1) == 0)
   sprintf(datop,"%02d.%02d.%d",dnn,mnn,gnn);
 }
strncpy(nomnn,row[4],sizeof(nomnn)-1);
strncpy(frop,row[5],sizeof(frop)-1);
strncpy(uspr,row[6],sizeof(uspr)-1);
strncpy(osnov,row[7],sizeof(osnov)-1);
strncpy(kodop,row[9],sizeof(kodop)-1);

usldopnalw(tipz,kodop,&procent,&naimnal);
int nomer_str=0;

//печатаем заголовок налоговой накладной
if(iceb_nalnaks(imaf,&nomer_str,dd,md,gd,dnn,mnn,gnn,nomnn,osnov,uspr,frop,kpos,tipz,0,&f1,wpredok) != 0)
 return(1);
nomer_str+=iceb_insfil("nalnakst.alx",f1,wpredok);
//iceb_nalnakh(&nomer_str,f1);  


sprintf(strsql,"select metka,kodzap,kolih,cena,ei,dnaim from Usldokum1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d order by metka,kodzap asc",
gd,md,dd,podr,nomdok,tipz);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

class iceb_u_str kod_ei_usl;
iceb_poldan("Код единицы измерения услуги",&kod_ei_usl,"uslnast.alx",wpredok);

char razdel[8];
strcpy(razdel," I");
itogo=0.;
bb2=0.;
i=0;
while(cur.read_cursor(&row) != 0)
 {
  i++;

  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kolih=atof(row[2]);
  cena=atof(row[3]);
  cena=iceb_u_okrug(cena,okrcn);

  /*Читаем наименование материалла*/
  memset(naim,'\0',sizeof(naim));

  if(metka == 0)
   sprintf(strsql,"select naimat from Material where kodm=%d",
   kodzap);

  if(metka == 1)
   sprintf(strsql,"select naius from Uslugi where kodus=%d",
   kodzap);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
//    printw("%s\n",strsql);
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
  strncpy(ei,row[4],sizeof(ei)-1);

  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);

  if(i == 2)
   {
    memset(datop,'\0',sizeof(datop));
    memset(razdel,'\0',sizeof(razdel));
   }  

  /*печатаем строку*/
  if(iceb_u_proverka(kod_ei_usl.ravno(),ei,0,1) == 0 && kolih == 1.)    
    iceb_nalnak_str(1,tipnn,razdel,datop,naim,ei,kolih,cena,bb,f1);
  else
    iceb_nalnak_str(0,tipnn,razdel,datop,naim,ei,kolih,cena,bb,f1);
  itogo+=bb;


 }
char punkt_zak[512];
memset(punkt_zak,'\0',sizeof(punkt_zak));
if(tipnn == 3)
 {
  //Смотрим пунк закона освобождения от НДС

  sprintf(bros1,"Пункт закона освобождения от НДС для операции %s",kodop);

  iceb_poldan(bros1,punkt_zak,"uslnast.alx",wpredok);
  
 }
//Печатаем концовку налоговой накладной
iceb_nalnake(tipz,tipnn,itogo,sumkor,naimnal.ravno(),procent,pnds,"uslnast.alx",kodop,okrg1,0.,f1,wpredok);
iceb_ustpeh(imaf,3,wpredok);
return(0);
}
/******************************************************/
void rasnalnuw_old(short dd,short md,short gd,int podr,
const char *nomdok,const char *imaf,
short tipnn, //Тип накладной
GtkWidget *wpredok)
{
SQL_str         row,row1;
int		kolstr;
char		strsql[512];
FILE            *f1,*f2;
short           i;
double          itogo;
double          bb,bb2;
char		datop[16];
double		sumkor;
char		bros[512];
char		kpos[32];
char		frop[512];
char		uspr[512];
char		osnov[512];
int		tipz;
char		nomnn[32]; /*Номер налоговой накладной*/
char		naim[512];
double		cena,kolih;
char		ei[32];
short 		metka;
int		kodzap;
short		d,m,g;
short		dnn,mnn,gnn; //Дата налоговой накладной
char		kodop[32];
float		procent=0.;
class iceb_u_str naimnal("");
int kor=0;
printf("%s\n",__FUNCTION__);

float pnds=0.;

SQLCURSOR curr;		
SQLCURSOR cur;		


/*Читаем шапку документа*/
sprintf(strsql,"select kontr,tp,sumkor,datop,nomnn,forop,uslpr,osnov,datpnn,kodop,pn \
from Usldokum where datd='%04d-%02d-%02d' and podr=%d and nomd='%s'",
gd,md,dd,podr,nomdok);

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s Не найден документ!Дата:%d.%d.%d Документ:%s Подразделение:%d",__FUNCTION__, dd,md,gd,nomdok,podr);
  iceb_menu_soob(strsql,wpredok);
  return;
 }
pnds=atof(row[10]);
memset(frop,'\0',sizeof(frop));
memset(uspr,'\0',sizeof(uspr));
memset(osnov,'\0',sizeof(osnov));
memset(datop,'\0',sizeof(datop));
sumkor=0.;

strncpy(kpos,row[0],sizeof(kpos)-1);
tipz=atoi(row[1]);
sumkor=atof(row[2]);
if(row[3][0] != '0')
 {
  iceb_u_rsdat(&d,&m,&g,row[3],2);
  sprintf(datop,"%02d.%02d.%d",d,m,g);
 }

if(datop[0] == '\0')
 {
  
  memset(bros,'\0',sizeof(bros));
  iceb_poldan("Дату отгрузки в налоговой накладной ставить всегда",bros,"matnast.alx",wpredok);
  if(iceb_u_SRAV(bros,"Вкл",1) == 0)
   sprintf(datop,"%02d.%02d.%d",dd,md,gd);
 }

//class iceb_u_str imaf_usl_nast;

//imafn("uslnast.alx",&imaf_usl_nast);
class iceb_u_str kod_ei_usl;
iceb_poldan("Код единицы измерения услуги",&kod_ei_usl,"uslnast.alx",wpredok);

strncpy(nomnn,row[4],sizeof(nomnn)-1);
strncpy(frop,row[5],sizeof(frop)-1);
strncpy(uspr,row[6],sizeof(uspr)-1);
strncpy(osnov,row[7],sizeof(osnov)-1);
iceb_u_rsdat(&dnn,&mnn,&gnn,row[8],2);
strncpy(kodop,row[9],sizeof(kodop)-1);

usldopnalw(tipz,kodop,&procent,&naimnal);
int nomer_str=0;

//печатаем заголовок налоговой накладной
iceb_rasnaln1(imaf,&nomer_str,&kor,dd,md,gd,dnn,mnn,gnn,nomnn,osnov,uspr,frop,kpos,tipz,&f1,&f2,wpredok);


sprintf(strsql,"select metka,kodzap,kolih,cena,ei,dnaim from Usldokum1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d order by metka,kodzap asc",
gd,md,dd,podr,nomdok,tipz);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
char razdel[8];
strcpy(razdel," I");
itogo=0.;
bb2=0.;
i=0;
while(cur.read_cursor(&row) != 0)
 {
  i++;

  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kolih=atof(row[2]);
  cena=atof(row[3]);
  cena=iceb_u_okrug(cena,okrcn);

  /*Читаем наименование материалла*/
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
     sprintf(strsql,"%s %d !\n",gettext("Не найден код материалла"),kodzap);
    if(metka == 1)
     sprintf(strsql,"%s %d !\n",gettext("Не найден код услуги"),kodzap);
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
  strncpy(ei,row[4],sizeof(ei)-1);

  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);

  if(i == 2)
   {
    memset(datop,'\0',sizeof(datop));
    memset(razdel,'\0',sizeof(razdel));
   }  
  if(iceb_u_proverka(kod_ei_usl.ravno(),ei,0,1) == 0 && kolih == 1.)
    fprintf(f1,"|%-4s|%-10s|%-*.*s|%-*s|%-*.*s|%10s",
    razdel,datop,
    iceb_u_kolbait(40,naim),iceb_u_kolbait(40,naim),naim,
    iceb_u_kolbait(7,naim),gettext("грн."),
    iceb_u_kolbait(9,gettext("Послуга")),iceb_u_kolbait(9,gettext("Послуга")),gettext("Послуга"),
    iceb_prcn(cena));
  else
    fprintf(f1,"|%-4s|%-10s|%-40.40s|%-7.7s|%9.9g|%10s",
    razdel,datop,naim,ei,kolih,iceb_prcn(cena));

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

  for(int nom=40; nom < iceb_u_strlen(naim); nom+=40)
   fprintf(f1,"|%4s|%-10s|%-*.*s|%-7.7s|%9.9s|%10s|%9s|%9s|%9s|%9s|%11s|\n",
    " "," ",
    iceb_u_kolbait(nom,iceb_u_adrsimv(nom,naim)),
    iceb_u_kolbait(nom,iceb_u_adrsimv(nom,naim)),
    iceb_u_adrsimv(nom,naim),
    " "," "," "," "," "," "," "," ");

  itogo+=bb;


 }

//Печатаем концовку налоговой накладной
iceb_rasnaln2(nomer_str,kor,tipnn,itogo,sumkor,naimnal.ravno(),procent,pnds,"uslnast.alx",kodop,okrg1,0.,f1,f2,wpredok);

}
