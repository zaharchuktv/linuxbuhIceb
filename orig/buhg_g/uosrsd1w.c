/* $Id: uosrsd1w.c,v 1.9 2011-02-21 07:35:58 sasa Exp $ */
/*17.11.2009	25.11.1999	Белых А.И.	uosrsd1w.c
Распечатка списка документов относящихзся к конкретному инвентарному 
номеру.
*/
#include <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        "buhg_g.h"
#include <unistd.h>

extern char	*organ;
extern SQL_baza bd;

void uosrsd1w(int in,GtkWidget *wpredok)
{
struct  tm      *bf;
FILE		*ff;
char		imaf[32];
long		kolstr;
SQL_str         row,row1;
SQLCURSOR curr;
char		strsql[512];
short		d1,m1,g1;
short		tp;
char		kodop[32];
time_t 		tmm;
char		naim[512];


//Читаем наименование инвентарного номера
memset(naim,'\0',sizeof(naim));
sprintf(strsql,"select naim from Uosin where innom=%d",in);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) > 0)
  strncpy(naim,row[0],sizeof(naim)-1); 

sprintf(strsql,"select * from Uosdok1 where innom=%d \
order by datd asc, tipz desc",in);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
sprintf(imaf,"uosd%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

time(&tmm);
bf=localtime(&tmm);
iceb_u_zagolov(gettext("Список документов по инвентарному номеру"),0,0,0,0,0,0,organ,ff);

fprintf(ff,"%s: %d %s\n",gettext("Инвентарный номер"),in,naim);

fprintf(ff,"\
-------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 |  Дата    | Н/д |Опе|Под|М.о.|  Налоговый учет     |  Бухгалтерский учет |Коф.инд.|\n\
 |          |     |   |        |Бал.стоим.| Износ    |Бал.стоим.| Износ    |        |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------\n");
double i_bs_nu=0.;
double i_iz_nu=0.;
double i_bs_bu=0.;
double i_iz_bu=0.;
double bs_nu=0.;
double iz_nu=0.;
double bs_bu=0.;
double iz_bu=0.;

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d1,&m1,&g1,row[0],2);
  tp=atoi(row[1]);

  sprintf(strsql,"select kodop from Uosdok where datd='%s' and \
nomd='%s'",row[0],row[4]);
  memset(kodop,'\0',sizeof(kodop));
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %s %s !",gettext("Не найден документ"),row[4],row[1]);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   strncpy(kodop,row1[0],sizeof(kodop)-1);
  
  bs_nu=atof(row[8]);
  iz_nu=atof(row[9]);
  bs_bu=atof(row[14]);
  iz_bu=atof(row[15]);
     
  if(tp == 1)
   {
    fprintf(ff,"+");
    i_bs_nu+=bs_nu;
    i_bs_bu+=bs_bu;
    i_iz_nu+=iz_nu;
    i_iz_bu+=iz_bu;  
   }
  if(tp == 2)
   {
    fprintf(ff,"-");
    i_bs_nu-=bs_nu;
    i_bs_bu-=bs_bu;
    i_iz_nu-=iz_nu;
    i_iz_bu-=iz_bu;  
   }
  fprintf(ff," %02d.%02d.%d %-*s %-*s %-3s %-4s %10.2f %10.2f %10.2f %10.2f %8.2f",
  d1,m1,g1,
  iceb_u_kolbait(5,row[4]),row[4],
  iceb_u_kolbait(3,kodop),kodop,
  row[5],row[6],atof(row[8]),atof(row[9]),
  atof(row[14]),atof(row[15]),atof(row[10]));

  if(atoi(row[2]) == 0)
   {
    fprintf(ff," %*.*s\n",
    iceb_u_kolbait(10,gettext("Не подтвержден")),
    iceb_u_kolbait(10,gettext("Не подтвержден")),
    gettext("Не подтвержден"));
   }

  if(atoi(row[2]) == 1)
   {
    fprintf(ff," %*.*s\n",
    iceb_u_kolbait(10,gettext("Подтвержден")),
    iceb_u_kolbait(10,gettext("Подтвержден")),
    gettext("Подтвержден"));
   }
 }    

fprintf(ff,"\
-------------------------------------------------------------------------------------\n");
fprintf(ff,"%31s %10.2f %10.2f %10.2f %10.2f\n",gettext("Итого"),i_bs_nu,i_iz_nu,i_bs_bu,i_iz_bu);


iceb_podpis(ff,wpredok);
fclose(ff);
class iceb_u_spisok imafil;
class iceb_u_spisok naimf;

imafil.plus(imaf);
naimf.plus(gettext("Список документов по инвентарному номеру"));

iceb_ustpeh(imaf,0,wpredok);

iceb_rabfil(&imafil,&naimf,"",0,wpredok);
}





