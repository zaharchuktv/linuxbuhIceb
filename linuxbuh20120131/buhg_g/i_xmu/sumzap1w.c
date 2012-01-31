/* $Id: sumzap1w.c,v 1.10 2011-02-21 07:35:57 sasa Exp $ */
/*13.10.2009	18.11.1997	Белых А.И.	sumzap1w.c
Получение суммы по документа материального учета
*/
#include        <stdlib.h>
#include        <math.h>
#include        <ctype.h>
#include        <time.h>
#include        <errno.h>
#include        "../headers/buhg_g.h"

extern double	okrg1;  /*Округление 1*/
extern SQL_baza bd;

void sumzap1w(int d,int m,int g,const char *nn,int skl,double *suma,
double *sumabn,int lnds,short vt,double *sumkor,int tipz,double *kolihmat,GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
short		kgrm;
double		cena,kolih,bb;
SQLCURSOR cur;
SQLCURSOR cur1;

*sumabn=0.;
*suma=0.;
*sumkor=0.;
*kolihmat=0.;
/*Узнаем сумму скидки*/

sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and sklad=%d and nomd='%s' and nomerz=13",
g,skl,nn);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  bb=iceb_u_atof(row[0]);
  *sumkor=iceb_u_okrug(bb,0.01);
 }

/*Узнаем сумму всех записей*/
sprintf(strsql,"select kodm,cena,kolih,voztar from Dokummat1 \
where datd='%04d-%d-%d' and sklad=%d and nomd='%s' and tipz=%d",
g,m,d,skl,nn,tipz);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  
  bb=atof(row[1])*atof(row[2]);
  bb=iceb_u_okrug(bb,okrg1);
  *kolihmat+=atof(row[2]);
  
  /*Узнаем код группы*/
  sprintf(strsql,"select kodgr from Material where kodm=%s",
  row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !\n",gettext("Не найден код материалла"),row[0]);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }
  kgrm=atoi(row1[0]);     

  if(atof(row[3]) == 1 || kgrm == vt || lnds > 0)
    *sumabn=*sumabn+bb;
  else
    *suma=*suma+bb;

 }

/*Читаем услуги*/
sprintf(strsql,"select kolih,cena from Dokummat3 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",g,m,d,skl,nn);
/*printw("\nstrsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[0]);
  cena=atof(row[1]);
  if(kolih > 0)
    bb=kolih*cena;
  else
    bb=cena;
  bb=iceb_u_okrug(bb,okrg1);
  *suma=*suma+iceb_u_okrug(bb,okrg1);

 }


}
