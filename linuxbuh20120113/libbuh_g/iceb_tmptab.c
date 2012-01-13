/*$Id: iceb_tmptab.c,v 1.2 2011-01-13 13:49:59 sasa Exp $*/
/*03.03.2010	03.03.2010	Белых А.И.	iceb_tmptab.c
Cоздание и удаление временных таблиц
*/
#include "iceb_libbuh.h"

/****************************/
/*Конструктор объекта       */
/****************************/
iceb_tmptab::iceb_tmptab()
{
metka=0; /*таблица не создана*/

}
/****************************/
/*Деструктор объекта       */
/****************************/
iceb_tmptab::~iceb_tmptab()
{
drop_tab(NULL);
}
/*******************/
/*cоздаём талблицу*/
/******************/
int iceb_tmptab::create_tab(const char *ima_tabl,const char *zapros,GtkWidget *wpredok)
{
if(metka == 1)
 drop_tab(wpredok);

ima_tab.new_plus(ima_tabl);
 
int voz=iceb_sql_zapis(zapros,0,0,wpredok);
if(voz == 0)
 metka=1;
return(voz);
}

/*******************/
/*Удаляем таблицу*/
/******************/
int iceb_tmptab::drop_tab(GtkWidget *wpredok)
{
char strsql[312];
if(metka == 0 || ima_tab.getdlinna() <= 1)
 return(0);

sprintf(strsql,"drop table %s",ima_tab.ravno());
int voz=iceb_sql_zapis(strsql,0,0,wpredok);
if(voz == 0)
 metka=0;
return(voz); 
}
