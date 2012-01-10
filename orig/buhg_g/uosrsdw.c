/* $Id: uosrsdw.c,v 1.6 2011-02-21 07:35:58 sasa Exp $ */
/*17.11.2009    22.08.1997      Белых А.И.      uosrsdw.c
Распечатка перечня документов по группе 2 и 3 или инвентарному
номеру
*/
#include <stdlib.h>
#include        <time.h>
#include        <errno.h>
#include        "buhg_g.h"
#include <unistd.h>

extern char     *organ;
extern SQL_baza bd;

void uosrsdw(int in,GtkWidget *wpredok)
{
struct  tm      *bf;
time_t 		tmm;
int		innom;
FILE		*ff;
char		imaf[32];
long		kolstr;
SQL_str         row,row1;
SQLCURSOR curr;
char		strsql[512];
short		d,m,g;
short		tp;
char		kontr[32];
char		kodop[32];
short		podt=0;
double		bs,iz,bsby,izby;
double		ibs=0.,iiz=0.;
double		ibsby=0.,iizby=0.;
char hna[32];
sprintf(hna,"%d",in*(-1));

if(in < 0)
 sprintf(strsql,"select datd,innom,tipz,nomd,bs,iz,bsby,izby \
from Uosdok1");
else
 sprintf(strsql,"select datd,innom,tipz,nomd,bs,iz,bsby,izby \
from Uosdok1 where innom=%d",in);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),wpredok);
  return;
 }



time(&tmm);
bf=localtime(&tmm);

sprintf(imaf,"uspd%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

iceb_u_zagolov(gettext("Распечатка записей документов"),0,0,0,0,0,0,organ,ff);

if(in < 0)
  fprintf(ff,"%s %d\n",gettext("Группа"),in*(-1));
else
 fprintf(ff,"%s %d\n",gettext("Инвентарный номер"),in);

fprintf(ff,"\
---------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Инвент.|О|  Дата    |Номер|Опе|Контр|Налоговый учет   |Бухгалтерский уч.|\n\
номер  | | документа|докум|рац|агент|Бал.ст. | Износ  |Бал.ст. | Износ  |\n"));

fprintf(ff,"\
---------------------------------------------------------------------------------\n");

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  innom=atoi(row[1]);
  
  /*Читаем переменные данные*/
  class poiinpdw_data rekin;

  poiinpdw(innom,m,g,&rekin,wpredok);

  if(in < 0)
   if(iceb_u_SRAV(hna,rekin.hna.ravno(),0) != 0)
    continue;

  tp=atoi(row[2]);
  bs=atof(row[4]);
  iz=atof(row[5]);
  bsby=atof(row[6]);
  izby=atof(row[7]);
  if(tp == 2)
   {
    bs*=-1;
    bsby*=-1;
    izby*=-1;
   }  

  /*Читаем шапку документа*/
  sprintf(strsql,"select kontr,kodop,podt from Uosdok where datd='%d.%02d.%02d' and \
  nomd='%s'",g,m,d,row[3]);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %s %d.%d.%d%s",
    gettext("Не найден документ !"),row[3],d,m,g,
    gettext("г."));
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    strncpy(kontr,row1[0],sizeof(kontr)-1);
    strncpy(kodop,row1[1],sizeof(kodop)-1);
    podt=atoi(row1[2]);
   }  

  fprintf(ff,"%7d ",innom);

  if(tp == 1)
   {
    fprintf(ff,"+");
   }
  if(tp == 2)
   {
    fprintf(ff,"-");
   }

  fprintf(ff," %02d.%02d.%d %-*s %-*s %-*s %8.2f %8.2f %8.2f %8.2f",
  d,m,g,
  iceb_u_kolbait(5,row[3]),row[3],
  iceb_u_kolbait(3,kodop),kodop,
  iceb_u_kolbait(5,kontr),kontr,
  bs,iz,bsby,izby);
  ibs+=bs;
  iiz+=iz;
  ibsby+=bsby;
  iizby+=izby;
  if(podt == 0)
   {
    fprintf(ff," %s\n",gettext("Не подтвержден"));
   }
  if(podt == 1)
   {
    fprintf(ff," %s\n",gettext("Подтвержден"));
   }
 }

fprintf(ff,"%*s %8.2f %8.2f %8.2f %8.2f\n",
iceb_u_kolbait(36,gettext("Итого")),gettext("Итого"),ibs,iiz,ibsby,iiz);

fclose(ff);

class iceb_u_spisok imafil;
class iceb_u_spisok naimf;

imafil.plus(imaf);
naimf.plus(gettext("Список документов"));
iceb_ustpeh(imaf,0,wpredok);
iceb_rabfil(&imafil,&naimf,"",0,wpredok);

}
