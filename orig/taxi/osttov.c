/*$Id: osttov.c,v 1.10 2011-02-21 07:36:21 sasa Exp $*/
/*06.11.2007	03.07.2007	Белых А.И.	osttov.c
Вычисление остатка по коду товара в бух-учёте вместе с записями в
подсистеме выписки счетов.
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        "i_rest.h"

double osttov_raz(int kodtov,int sklad,const char *ed_iz,FILE *ff,GtkWidget *wpredok);
double osttov_dt(int kodmat,int sklad,const char *ed_iz,FILE *ff,GtkWidget *wpredok);
double osttov_iz(int kodtov,int sklad,FILE *ff,GtkWidget *wpredok);

extern SQL_baza	bd;
extern short	startgod; /*Стартовый год просмотров для материального учёта*/

double osttov(int kodmat,int sklad,
const char *ed_iz, /*Единица измерения*/
const char *imaf,  //если имя файла указано то в этот файл выгружается протокол расчёта
GtkWidget *wpredok)
{
//printf("osttov-start\n");

double ikolih=0.;

FILE *ff=NULL;
if(imaf[0] != '\0')
 {
  if((ff = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"ostmat.c",errno,wpredok);
    return(0.);
   }
  SQL_str row;
  class SQLCURSOR cur;
  char naim_mat[100];
  char strsql[500];
  char naim_ei[100];
  
  memset(naim_mat,'\0',sizeof(naim_mat));
  sprintf(strsql,"select naimat from Material where kodm=%d",kodmat);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim_mat,row[0],sizeof(naim_mat)-1);
  
  memset(naim_ei,'\0',sizeof(naim_ei));
  sprintf(strsql,"select naik from Edizmer where kod='%s'",ed_iz);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim_ei,row[0],sizeof(naim_ei)-1);
  
  fprintf(ff,"Склад %d Код товара:%d/%s\n",sklad,kodmat,naim_mat);
  fprintf(ff,"Единица измерения:%s/%s\n",ed_iz,naim_ei);
 }

if(iceb_prov_iz(kodmat,wpredok) == 0)
 ikolih=osttov_iz(kodmat,sklad,ff,wpredok);
else 
 ikolih=osttov_dt(kodmat,sklad,ed_iz,ff,wpredok);

if(ff != NULL)
 {
  fprintf(ff,"\
--------------------------------------\n");
  fprintf(ff,"%26s:%10.2f\n","Общий остаток",ikolih);
  iceb_podpis(ff,wpredok);
  fclose(ff);
 }
 
//printf("osttov-end\n");
return(ikolih);
}
/***************************/
/*Расчёт остатка для товара*/
/***************************/
double osttov_dt(int kodmat,int sklad,
const char *ed_iz, /*Единица измерения*/
FILE *ff,
GtkWidget *wpredok)
{
char strsql[500];
SQL_str row;
class SQLCURSOR cur;
SQL_str row1;
class SQLCURSOR cur1;
int kolstr=0;
short d,m,g;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
double ikolih=0.;

short godn=startgod;
if(godn == 0)
 godn=gt;
if(gt < startgod)
  godn=gt; 

sprintf(strsql,"select nomk from Kart where kodm=%d and sklad=%d",kodmat,sklad);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
if(ff != NULL)
 {
  fprintf(ff,"Вычисляем остатки в материальном учёте:\n");
  fprintf(ff,"\
--------------------------------------\n");
  fprintf(ff,"\
  Дата    |Номер документа|Количество|\n");
  fprintf(ff,"\
--------------------------------------\n");
 } 

int tipz=0;
double kolih=0.;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select datdp,nomd,kolih,tipz from Zkart where sklad=%d \
  and nomk=%s and datdp >= '%04d-01-01' and datdp <= '%04d-%02d-%02d' order by datdp asc",
  sklad,row[0],godn,gt,mt,dt);
  if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
    return(1);
   }
  if(ff != NULL)
   fprintf(ff,"Карточка:%s\n",row[0]);
//  printf("osttov-%s\n",strsql);
  while(cur1.read_cursor(&row1) != 0)
   {
    tipz=atoi(row1[3]);
    iceb_u_rsdat(&d,&m,&g,row1[0],2);

    if(tipz == 1 && iceb_u_SRAV(row1[1],"000",0) == 0 && g != godn)
     continue;

    kolih=atof(row1[2]);
    if(tipz == 2)
     kolih*=-1;

    if(ff != NULL)
     fprintf(ff,"%02d.%02d.%d %-15s %10.2f\n",d,m,g,row1[1],kolih);
     
    ikolih+=kolih;
   }   

 }
if(ff != NULL)
 {
  fprintf(ff,"\
--------------------------------------\n");
  fprintf(ff,"%26s:%10.2f\n","Остаток по карточкам",ikolih);

  fprintf(ff,"\nСмотрим сколько выписано товара по счетам:\n");
  fprintf(ff,"\
--------------------------------------\n");
  fprintf(ff,"\
  Дата    |Номер документа|Количество|\n");
  fprintf(ff,"\
--------------------------------------\n");
 }
  
//смотрим сколько списано в выписке счетов
sprintf(strsql,"select datd,nomd,kolih from Restdok1 where kodz=%d and skl=%d and mz=0 and ms=0",
kodmat,sklad);

//  printf("osttov-%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[2])*-1;
  
  if(ff != NULL)
   {
    iceb_u_rsdat(&d,&m,&g,row[0],2);
    fprintf(ff,"%02d.%02d.%d %-15s %10.2f\n",d,m,g,row[1],kolih);
   }
  ikolih+=kolih;
 }

if(ff != NULL)
 {
  fprintf(ff,"\
--------------------------------------\n");
  fprintf(ff,"%26s:%10.2f\n","Остаток поcле учёта счетов",ikolih);
 }
 
/*Смотрим сколько было списано в состве изделий*/
ikolih+=osttov_raz(kodmat,sklad,ed_iz,ff,wpredok);
if(fabs(ikolih) < 0.000001)
 ikolih=0.;
 

return(ikolih);
}
/************************************/
/*Расчёт списания в составе изделий*/
/***********************************/

double osttov_raz(int kodtov,int sklad,const char *ed_iz,FILE *ff,GtkWidget *wpredok)
{

int kolstr;
SQL_str row;
class SQLCURSOR cur;
double kolih_spis=0.;

char strsql[500];
class iceb_u_spisok sp_dok; //Список документов дата|номер документа
class iceb_u_int    kod_tov; //Список кодов товаров в документах
class iceb_u_double kolih_tov_dok; //количество товара в документе

class iceb_u_int o_kod_tov; //Общий список товаров

sprintf(strsql,"select datd,nomd,kodz,kolih from Restdok1 where skl=%d and ms=0 and mz=0",sklad);
 
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
int kodt=0;
while(cur.read_cursor(&row) != 0)
 {
  kodt=atoi(row[2]);
  if(iceb_prov_iz(kodt,wpredok) != 0)
   continue;

  sprintf(strsql,"%s|%s",row[0],row[1]);
  sp_dok.plus(strsql);

  kod_tov.plus(kodt);
  kolih_tov_dok.plus(atof(row[3]),-1);

  if(o_kod_tov.find(kodt) < 0)
   o_kod_tov.plus(kodt); 
 }

int kolih_iz=o_kod_tov.kolih();
if(kolih_iz == 0)
 return(0.);

class iceb_u_double kolih_kod_tov; //Количество проверяемого товара в изделии

kolih_kod_tov.make_class(o_kod_tov.kolih());
double kolih=0.;
int nomer=0;

for(int ii=0; ii < kolih_iz; ii++)
 {
//  class iceb_u_int sp_v_tov; /*Список входящих товаров*/
//  class iceb_u_double kolih_v_tov; /*Количество входящих товаров*/
//  class iceb_u_int metka_mu; /*Список меток 0-товар 1-услуга*/
  
//  iceb_razuz_kod(o_kod_tov.ravno(ii),&sp_v_tov,&kolih_v_tov,&metka_mu,wpredok);

  class iceb_razuz_data data;
  data.kod_izd=o_kod_tov.ravno(ii);
  iceb_razuz_kod(&data,wpredok);

  sprintf(strsql,"%d|%s",kodtov,ed_iz);
  if((nomer=data.kod_det_ei.find(strsql)) < 0)
   continue;
  kolih_kod_tov.plus(data.kolih_det_ei.ravno(nomer),ii);

 }


if(ff != NULL)
 {
  fprintf(ff,"\nСмотрим сколько выписано в составе изделий:\n");
  fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
  fprintf(ff,"\
  Дата    |Номер документа|Количество|Кол.из.|Колич.в из.|Код из.|Наименование изделия \n");
/*         123456789012345 1234567890 1234567 12345678901 1234567*/
  fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

 }
short d,m,g;
char naim_iz[100];
char nomdok[50];

for(int ii=0; ii < kolih_iz; ii++)
 {
  kolih=kolih_kod_tov.ravno(ii);
  if(kolih == 0.) /*В этом изделии нет входимости проверяемого товара*/
   continue; 

  for(int kk=0; kk < sp_dok.kolih(); kk++)
   {
    if(kod_tov.ravno(kk) != o_kod_tov.ravno(ii))
     continue;
       
    kolih=kolih_tov_dok.ravno(kk)*kolih;
    kolih_spis+=kolih;

    if(ff != NULL)
     {
      memset(naim_iz,'\0',sizeof(naim_iz));
      sprintf(strsql,"select naimat from Material where kodm=%d",o_kod_tov.ravno(ii));
      if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
       strncpy(naim_iz,row[0],sizeof(naim_iz)-1);
      
      iceb_u_polen(sp_dok.ravno(kk),strsql,sizeof(strsql),1,'|');
      iceb_u_polen(sp_dok.ravno(kk),nomdok,sizeof(nomdok),2,'|');
      iceb_u_rsdat(&d,&m,&g,strsql,2);

      fprintf(ff,"%02d.%02d.%04d %-15s %10.10g %7.7g %11.11g %7d %s\n",
      d,m,g,nomdok,kolih,kolih_tov_dok.ravno(kk),kolih_kod_tov.ravno(ii),o_kod_tov.ravno(ii),naim_iz);
     }
   }



 }

kolih_spis*=-1;

return(kolih_spis);

}
/***************************/
/*Расчёт остатка для изделия*/
/****************************/

double osttov_iz(int kodtov,int sklad,FILE *ff,GtkWidget *wpredok)
{
//printf("osttov_iz %d\n",kodtov);

SQL_str row;
class SQLCURSOR cur;

char strsql[500];

#if 0
#############3
class iceb_u_int sp_v_tov; /*Список входящих товаров*/
class iceb_u_double kolih_v_tov; /*Количество входящих товаров*/
class iceb_u_int metka_mu; /*Список меток 0-товар 1-услуга*/

#############3
#endif
class iceb_razuz_data data;
data.kod_izd=kodtov;
if(iceb_razuz_kod(&data,wpredok) == 0)
 return(0.);
 
if(ff != NULL)
 {
  
  fprintf(ff,"\
-----------------------------------------------------------------------\n");
  fprintf(ff,"\
 Код |          Наименование        |Кол.в изд.| Остаток  |Кол.изделий|\n");
/*
12345 123456789012345678901234567890 1234567890 1234567890 12345678901
*/
 fprintf(ff,"\
-----------------------------------------------------------------------\n");
 }
char naim_mat[100];
double ostatok=0.;
double kolih_v_iz=0.;
double kolih_iz=0.;
double max_kolih_iz=0.;
//int kolih_det=sp_v_tov.kolih();
int kolih_det=data.kod_det_ei.kolih();
int kod=0;
char eizm[20];
for(int ii=0; ii < kolih_det; ii++)
 {

  if(data.metka_mu.ravno(ii) == 1) /*Услуга*/
   continue;

//  kolih_v_iz=kolih_v_tov.ravno(ii);  
  kolih_v_iz=data.kolih_det_ei.ravno(ii);  
  memset(eizm,'\0',sizeof(eizm));

  iceb_u_polen(data.kod_det_ei.ravno(ii),&kod,1,'|');
  iceb_u_polen(data.kod_det_ei.ravno(ii),eizm,sizeof(eizm),2,'|');


  /*  printf("eizm=%s/%s/%d\n",eizm,data.kod_det_ei.ravno(ii),kod);  */
  ostatok=osttov(kod,sklad,eizm,"",wpredok);
  kolih_iz=0.;
  if(ostatok > 0.)
    kolih_iz=ostatok/kolih_v_iz;  

  if(max_kolih_iz == 0.)
    max_kolih_iz=kolih_iz;

  if(max_kolih_iz > kolih_iz)
   max_kolih_iz=kolih_iz;    

  if(ff != NULL)
   {
    memset(naim_mat,'\0',sizeof(naim_mat));
    sprintf(strsql,"select naimat from Material where kodm=%d",kod);
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     strncpy(naim_mat,row[0],sizeof(naim_mat)-1);
  
    fprintf(ff,"%5d %-30.30s %10.10g %10.10g %10.10g\n",
    kod,naim_mat,kolih_v_iz,ostatok,kolih_iz);

   }
 }
if(ff != NULL)
 {
  fprintf(ff,"\
-----------------------------------------------------------------------\n");

  fprintf(ff,"\nМаксимальное количество изделий на которые хватит остатков:%.10g\n",max_kolih_iz);
/************
  fprintf(ff,"\nВычисляем сколько уже выписано в счетах:\n");
  fprintf(ff,"\
--------------------------------------\n");
  fprintf(ff,"\
  Дата    |Номер документа|Количество|\n");
  fprintf(ff,"\
--------------------------------------\n");
***************/ 
 }


/***************
Документы не смотрим так как они уже учтены в предыдущем расчёте
class iceb_u_spisok sp_doks ; 
class iceb_u_double kolih_v_spdok;

sprintf(strsql,"select datd,nomd,kolih from Restdok1 where kodz=%d and skl=%d and mz=0 and ms=0",kodtov,sklad);
 
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

short d,m,g;
double kolih=0.;
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[2])*-1;
  max_kolih_iz+=kolih;
  if(ff != NULL)
   {
    
    iceb_u_rsdat(&d,&m,&g,row[0],2);
    fprintf(ff,"%02d.%02d.%d %-15s %10.10g\n",d,m,g,row[1],kolih);
   }  
 }
******************/

return(max_kolih_iz);

}
