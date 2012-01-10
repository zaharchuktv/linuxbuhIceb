/*$Id: prosprkrw.c,v 1.10 2011-02-21 07:35:56 sasa Exp $*/
/*07.08.2010	28.10.2002	Белых А.И.	prosprkr.c
Определение сумм по счетам для выполнения проводок для командировочных расходов
*/
#include <stdlib.h>
#include        <math.h>
#include        "buhg_g.h"

extern double   okrg1; /*Округление*/
extern SQL_baza bd;


void prosprkrw(short dd,short md,short gd,
const char *nomdok,
int metkareg, //0-суммы на экран 1-проверить все ли проводки сделаны
class iceb_u_str *shet_sum,
GtkWidget *wpredok)
{
char		bros[512];
char		strsql[512];
int		kolstr=0;
SQL_str		row;
int		nomer=0;
double		suma;
double		itogo=0.;
int		kolshet=0;
int		i=0;
int		metkaprov=0;
int		metkavpr=0; //0-проводки не все сделаны 1-все

class iceb_u_spisok spis_shet; /*Список счетов*/
class iceb_u_double sum_shet; /*Суммы по счетам*/

sprintf(strsql,"select shet,kolih,cena,ss from Ukrdok1 \
where datd='%04d-%02d-%02d'and nomd='%s'",
gd,md,dd,nomdok);  
SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;

while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2])+atof(row[3]);
  suma=iceb_u_okrug(suma,okrg1);
  
  if((nomer=spis_shet.find(row[0])) == -1)
   {
    spis_shet.plus(row[0]); 
    kolshet++;
   }
  sum_shet.plus(suma,nomer);

 }

if(metkareg == 0)  //Вывод на экран
 {
  shet_sum->new_plus("");
  if(kolshet == 0)
   return;
  itogo=0.;

  for(i=0; i < kolshet ; i++)
   {
    sprintf(strsql,"%-*s %14s\n",iceb_u_kolbait(8,spis_shet.ravno(i)),spis_shet.ravno(i),iceb_u_prnbr(sum_shet.ravno(i)));
    if(i == 0)
     shet_sum->new_plus(strsql);
    else
     shet_sum->plus(strsql);
    itogo+=sum_shet.ravno(i);
   }

  sprintf(strsql,"%-*s %14s",iceb_u_kolbait(8,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(itogo));
  shet_sum->plus(strsql);

 }

if(metkareg == 1)  //Проверить все ли проводки сделаны
 {
  double sumaprov[kolshet];
  memset(&sumaprov,'\0',sizeof(sumaprov));
  
  sprintf(strsql,"select sh,shk,deb,kre from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%04d-%02d-%02d'",
  gettext("УКР"),0,nomdok,gd,md,dd);


  //printw("\n%s\n",strsql);
  //OSTANOV();

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }
  //printw("kolstr=%d\n",kolstr);
  //refresh();

  while(cur.read_cursor(&row) != 0)
   {
  //  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
    metkaprov++;

    i=spis_shet.find(row[1]);

    sumaprov[i]+=atof(row[3]);

   }
  
  class iceb_u_spisok repl;

  for(i=0; i < kolshet ; i++)
   {
    if(fabs(sumaprov[i] - sum_shet.ravno(i)) > 0.009)
     {
      metkavpr++;

      sprintf(strsql,"%s %s %s! %.14g != %.14g %d/%d",
      gettext("По счёту"),
      spis_shet.ravno(i),
      gettext("не сделаны все проводки"),
      sumaprov[i],sum_shet.ravno(i),i,kolshet);

      repl.plus(strsql);
     }
   }

  if(metkavpr > 0)
   {
    sprintf(strsql,"%s %s %s %d.%d.%d",
    gettext("Документ"),
    nomdok,
    gettext("от"),
    dd,md,gd);
    
    repl.plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    
   }

  if(metkavpr == 0 )
   {
    /*Проводки выполнены*/
    sprintf(strsql,"update Ukrdok \
set mprov=1 where datd='%04d-%02d-%02d' and nomd='%s'",
    gd,md,dd,nomdok);
   }
  else
   {
   //Если документ заблокирован то никаких меток не ставим
    if(iceb_pbpds(md,gd,wpredok) != 0)
     return;

    /*Проводки не выполнены*/
    sprintf(strsql,"update Ukrdok \
set mprov=0 where datd='%04d-%02d-%02d' and nomd='%s'",
    gd,md,dd,nomdok);

   }

  if(sql_zap(&bd,strsql) != 0)
   if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
    {
     sprintf(bros,"prosprkr-%s",gettext("Ошибка корректировки записи !"));
     iceb_msql_error(&bd,bros,strsql,wpredok);
    }


 }



}
