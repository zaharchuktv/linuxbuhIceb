/*$Id: iceb_provinnom.c,v 1.8 2011-02-21 07:36:07 sasa Exp $*/
/*27.09.2009	10.10.2002	Белых А.И.	iceb_provinnom.c
Проверить есть ли такой инвентарный номер
Если вернули 0 - нет
*/
#include        "iceb_libbuh.h"

extern SQL_baza	bd;

int iceb_provinnom(const char *innom,GtkWidget *wpredok)
{
char		strsql[512];
SQL_str		row;
SQLCURSOR       cur;
char		bros[512];

sprintf(strsql,"select innom from Uosin where innom=%s",innom);
if(sql_readkey(&bd,strsql,&row,&cur) == 0)
 {
  if(iceb_poldan("Проверка инвентарного номера в материальном учете",bros,"uosnast.alx",wpredok) == 0)
   if(iceb_u_SRAV(bros,"Вкл",1) != 0)
      return(0);

  sprintf(strsql,"select sklad,nomk  from Kart where innom='%s' and rnd=''",innom);
  if(sql_readkey(&bd,strsql,&row,&cur) == 0)
   return(0);

  if(sql_readkey(&bd,strsql,&row,&cur) > 0)
   {
    iceb_u_str repl;
    
    repl.plus(gettext("Такой инвентарный номер уже есть !"));
    
    sprintf(strsql,"%s %s %s %s",gettext("Склад"),row[0],
    gettext("карточка"),row[1]);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);    
   }   
 }
else
 {
  iceb_u_str repl;
  
  repl.plus(gettext("Такой инвентарный номер уже есть !"));
  repl.ps_plus(gettext("Учет основных средств."));
  
  iceb_menu_soob(&repl,wpredok);    
 }

return(1);
}
