/*$Id: uosvhd.c,v 1.10 2011-08-29 07:13:44 sasa Exp $*/
/*14.11.2007	24.10.2007	Белых А.И.	ousvhd.c
Вызов подпрограмм ввода информации в шапку документа
Если вернули 0-шапка записана
             1-нет
*/
#include <stdlib.h>
#include "buhg_g.h"

int uosvhd1(class iceb_u_str *datadok,class iceb_u_str *nomdok,const char *kod_op,GtkWidget *wpredok);
int uosvhd2(class iceb_u_str *datadok,class iceb_u_str *nomdok,const char *kod_op,GtkWidget *wpredok);
int uosvhdzv(class iceb_u_str *datadok,class iceb_u_str *nomdok,int tipz,const char *kod_op,GtkWidget *wpredok);
int uosvhdvp(class iceb_u_str *datadok,class iceb_u_str *nomdok,int tipz,const char *kod_op,GtkWidget *wpredok);


int uosvhd(class iceb_u_str *datadok,
class iceb_u_str *nomdok,
class iceb_u_str *kod_op,  /*код операции заполняется только для нового документа*/
int tipz,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

if(datadok->getdlinna() > 1)
 {
  sprintf(strsql,"select kodop from Uosdok where datd='%s' and nomd='%s'",datadok->ravno_sqldata(),
  nomdok->ravno());
  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден документ"));
    repl.plus(" !!!");

    sprintf(strsql,"%s:%s %s:%s",
    gettext("Дата"),
    datadok->ravno(),
    gettext("Номер документа"),
    nomdok->ravno());
    
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);  
    
    return(1);
   }   
  
  kod_op->new_plus(row[0]);
 }

if(tipz == 1)
  sprintf(strsql,"select vido from Uospri where kod='%s'",kod_op->ravno());
if(tipz == 2)
  sprintf(strsql,"select vido from Uosras where kod='%s'",kod_op->ravno());
int vido=0;
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s-Не найден код операции %s !",__FUNCTION__,kod_op->ravno());
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

vido=atoi(row[0]);
//printf("tipz=%d vido=%d\n",tipz,vido);

if(tipz == 1)
 {
  if(vido == 0) /*приход внешний*/
     return(uosvhd1(datadok,nomdok,kod_op->ravno(),wpredok));
  if(vido == 1) /*внутреннее перемещение*/
     return(uosvhdvp(datadok,nomdok,1,kod_op->ravno(),wpredok));
  if(vido == 2) /*смена стоимости*/
     return(uosvhdzv(datadok,nomdok,1,kod_op->ravno(),wpredok));
 }
if(tipz == 2)
 {
  if(vido == 0) /*Расход внешний*/
     return(uosvhd2(datadok,nomdok,kod_op->ravno(),wpredok));
  if(vido == 1) /*внутреннее перемещение*/
     return(uosvhdvp(datadok,nomdok,2,kod_op->ravno(),wpredok));
  if(vido == 2) /*смена стоимости*/
     return(uosvhdzv(datadok,nomdok,2,kod_op->ravno(),wpredok));
 }
return(1);
}
