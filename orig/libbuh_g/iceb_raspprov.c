/*$Id: iceb_raspprov.c,v 1.13 2011-02-21 07:36:07 sasa Exp $*/
/*12.11.2008	04.07.2001	Белых А.И.	iceb_raspprov.c
Распечатка проводок из подсистем "Материальный учет",
"Расчет зарплаты" "Учет основных средств", "Учет кассовых ордеров",
"Учет услуг"
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <math.h>
#include        "iceb_libbuh.h"
#include        <unistd.h>

extern char	*organ;
extern SQL_baza	bd;

void iceb_raspprov(const char *zapros,
short dd,short md,short gd, //Дата документа
const char *nomd, //номер документа
int pods, //1-материальный учет
          //2-Расчет зарплаты
          //3-Учет основных средств
          //4-Учет услуг
          //5-Учет кассовых ордеров
          //6-платежные документы
          //7-командировочные расходы
GtkWidget *wpredok)
{
double	deb,kre;
double	ideb=0.,ikre=0.;
char	imaf[56];
FILE	*ff;
short   d,m,g;
time_t	tmm;
struct  tm      *bf;
SQL_str		row;
SQLCURSOR       cur;
int kolstr=0;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }
if(kolstr == 0)
 return;

sprintf(imaf,"provp%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
iceb_u_startfil(ff);
time(&tmm);
bf=localtime(&tmm);
fprintf(ff,"%s\n\n\
%s.\n\
%s %d.%d.%d  %s: %d:%d\n",
organ,
gettext("Распечатка проводок"),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),bf->tm_hour,bf->tm_min);

if(pods == 1)
 fprintf(ff,"%s\n",gettext("Материальный учет"));
if(pods == 2)
 fprintf(ff,"%s.\n",gettext("Расчет зарплати"));
if(pods == 3)
 fprintf(ff,"%s.\n",gettext("Учет основных средств"));
if(pods == 4)
 fprintf(ff,"%s.\n",gettext("Учет услуг"));
if(pods == 5)
 fprintf(ff,"%s.\n",gettext("Учет кассовых ордеров"));
if(pods == 6)
 fprintf(ff,"%s.\n",gettext("Платежные документы"));
if(pods == 7)
 fprintf(ff,"%s.\n",gettext("Учёт командировочных расходов"));

if(pods == 2)
 {
  fprintf(ff,"%d.%d%s %s\n",
  md,gd,
  gettext("г."),
  nomd);
 }
else
 { 
  fprintf(ff,"%s:%s %s %d.%d.%d%s\n",
  gettext("Документ"),
  nomd,
  gettext("от"),
  dd,md,gd,
  gettext("г."));
 }
fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
  Дата    |Счет |СчетК|  Дебет   |  Кредит  |Конт.|Кто |  Коментарий\n"));
fprintf(ff,"\
--------------------------------------------------------------------------\n");
while(cur.read_cursor(&row) != 0)
 {
  deb=atof(row[4]);
  kre=atof(row[5]);

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  
  fprintf(ff,"%02d.%02d.%d %-*s %-*s %10.2f %10.2f %-*s %-4s %s\n",
  d,m,g,
  iceb_u_kolbait(5,row[1]),row[1],
  iceb_u_kolbait(5,row[2]),row[2],
  deb,kre,
  iceb_u_kolbait(5,row[3]),row[3],
  row[7],row[6]);

  ideb+=deb;
  ikre+=kre;  
 }
fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f\n",
iceb_u_kolbait(22,gettext("Итого")),gettext("Итого"),ideb,ikre);


iceb_podpis(ff,wpredok);
fclose(ff);

iceb_u_spisok fil;
iceb_u_spisok naim;

fil.plus(imaf);
naim.plus(gettext("Распечатка проводок"));

iceb_rabfil(&fil,&naim,"",0,wpredok);
 
}
