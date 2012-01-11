/* $Id: matspow.c,v 1.9 2011-02-21 07:35:55 sasa Exp $ */
/*14.11.2009    03.06.1997      Белых А.И.      matspow.c
Соэдание массива операций - материалов
*/
#include        <stdlib.h>
#include        <math.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include        "dvtmcf3_r.h"

extern SQL_baza bd;

int             matspow(class dvtmcf3_r_data *data)
{
long		kolstr;
SQL_str         row;
char		strsql[512];

sprintf(strsql,"select kodm from Material");
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдено ни одной записи !"));
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,data->window);
  return(1);
 }



while(cur.read_cursor(&row) != 0)
  data->mko.plus(atoi(row[0]),-1);



sprintf(strsql,"%s %d\n",gettext("Количество кодов материалов"),data->mko.kolih());
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

/*Читаем список приходов*/

sprintf(strsql,"select kod from Prihod");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);

if(kolstr == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдено ни одной записи !"));
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
  data->spoprp.plus(row[0]);

/*Читаем список расходов*/
sprintf(strsql,"select kod from Rashod");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдено ни одной записи !"));
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
  data->spoprr.plus(row[0]);

/*Читаем список групп*/

sprintf(strsql,"select kod from Grup");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдено ни одной записи !"));
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,data->window);
  return(1);
 }
while(cur.read_cursor(&row) != 0)
  data->mkg.plus(atoi(row[0]),-1);



sprintf(strsql,"%s %d \n",gettext("Количество операций приходов"),data->spoprp.kolih());
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s %d\n",gettext("Количество операций расходов"),data->spoprr.kolih());
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s %d\n",gettext("Количество кодов групп материалов"),data->mkg.kolih());
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

/*Маcсивы приходов для материалов*/

data->mkm1p.make_class(data->spoprp.kolih()*data->mko.kolih());
data->mkm2p.make_class(data->spoprp.kolih()*data->mko.kolih());
data->mkm3p.make_class(data->spoprp.kolih()*data->mko.kolih());

/*Маcсивы расходов для материалов*/
data->mkm1r.make_class(data->spoprr.kolih()*data->mko.kolih());
data->mkm2r.make_class(data->spoprr.kolih()*data->mko.kolih());
data->mkm3r.make_class(data->spoprr.kolih()*data->mko.kolih());


/*Маcсивы приходов для групп материалов*/
data->mkg1p.make_class(data->spoprp.kolih()*data->mkg.kolih());
data->mkg2p.make_class(data->spoprp.kolih()*data->mkg.kolih());
data->mkg3p.make_class(data->spoprp.kolih()*data->mkg.kolih());

/*Маcсивы расходов для групп материалов*/
data->mkg1r.make_class(data->spoprr.kolih()*data->mkg.kolih());
data->mkg2r.make_class(data->spoprr.kolih()*data->mkg.kolih());
data->mkg3r.make_class(data->spoprr.kolih()*data->mkg.kolih());

/*Маcсивы расходов для отхода по материаллам*/
data->mko1r.make_class(data->spoprr.kolih()*data->mko.kolih());
data->mko2r.make_class(data->spoprr.kolih()*data->mko.kolih());
data->mko3r.make_class(data->spoprr.kolih()*data->mko.kolih());

return(0);
}
