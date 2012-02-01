/*$Id: iceb_optimbazw.c,v 1.4 2011-02-21 07:36:07 sasa Exp $*/
/*14.11.2009	09.05.2005	Белых А.И.	iceb_optimbazw.c
Оптимизация базы 
*/


#include "iceb_libbuh.h"
extern char *imabaz;

int iceb_optimbazw_r(const char *imabaz,GtkWidget*);


void iceb_optimbazw(const char *imabaz,GtkWidget *wpredok)
{

iceb_u_spisok repl;
repl.plus(gettext("Оптимизация базы данных делается после удаления большого\n\
количества записей из базы данных. Выполнение оптимизации после удаления\n\
записей позволяет уменьшить размер базы."));
char strsql[100];
sprintf(strsql,"%s:%s",gettext("Имя базы данных"),imabaz);
repl.plus(strsql);

repl.plus(gettext("Оптимизировать ? Вы уверены ?"));

if(iceb_menu_danet(&repl,2,wpredok) == 2)
  return;


iceb_optimbazw_r(imabaz,wpredok);

}
