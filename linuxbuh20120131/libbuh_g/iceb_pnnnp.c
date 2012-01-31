/*$Id: iceb_pnnnp.c,v 1.4 2011-02-21 07:36:07 sasa Exp $*/
/*08.12.2009	13.11.2009	Белых А.И.	iceb_pnnnp.c
Проверка номера счет-фактуры поствщика - введена ли она в шапку какого-нибудь документа
Если вернуди 0-не введена
             1-введена
*/
#include "iceb_libbuh.h"
extern SQL_baza bd;

int iceb_pnnnp(const char *datann, /*Дата получения счет-фактуры*/
const char *datavnn,/*Дата выписки счет-фактуры*/
const char *kodkontr,const char *nomnn,
const char *datadok,const char *nomdok,
int metka_ps, /*1-материальный учёт
                2-учёт услуг
                3-учёт основных средств
                */
GtkWidget *wpredok)
{
//printf("iceb_pnnnp-%s %s %s %s %s %d\n",datann,kodkontr,nomnn,datadok,nomdok,metka_ps);

if(datadok[0] == '\0')
 return(0);

if(nomnn[0] == '\0')
 return(0);

if(kodkontr[0] == '\0')
 return(0);

if(datann[0] == '\0')
 return(0);

if(nomdok[0] == '\0')
 return(0);

if(metka_ps == 0)
 return(0);

short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,datadok,1);
short dn,mn,gn;
iceb_u_rsdat(&dn,&mn,&gn,datann,1);
 
char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur,cur1;

if(metka_ps == 1)
 sprintf(strsql,"select datd,nomd from Dokummat where datpnn='%04d-%02d-%02d' and kontr='%s' and nomnn='%s' \
and nomd <> '%s' and tip=1",
 gn,mn,dn,kodkontr,nomnn,nomdok);
else
 sprintf(strsql,"select datd,nomd from Dokummat where datpnn='%04d-%02d-%02d' and kontr='%s' and nomnn='%s' and tip=1",
 gn,mn,dn,kodkontr,nomnn);
//printf("iceb_pnnnp-%s\n",strsql);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
 {
  sprintf(strsql,"%s\n%s\n%s:%s %s:%s",
  gettext("Такой номер счет-фактуры уже введён!"),
  gettext("Материальный учёт"),
  gettext("Дата документа"),
  iceb_u_datzap(row[0]),
  gettext("Номер документа"),
  row[0]);
  iceb_menu_soob(strsql,wpredok);      
  return(1);
 }

if(datavnn[0] != '\0')
 {
  sprintf(strsql,"select sklad,nomd,sodz from Dokummat2 where god=%d and nomerz=14",gd);
  int kolstr=0;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,"",strsql,wpredok);
   }  
  int kolstr1=0;
  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    if(iceb_u_sravmydat(row[2],datavnn) != 0)
     continue;

    sprintf(strsql,"select datd,nomd,kontr,nomnn from Dokummat where datd >= '%04d-01-01' \
 and datd <= '%04d-12-31' and sklad=%s and nomd='%s' and tip=1",
    gd,gd,row[0],row[1]);
    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,"",strsql,wpredok);
      continue;
     }  
    while(cur1.read_cursor(&row1) != 0)
     {

      if(iceb_u_SRAV(kodkontr,row[2],0) != 0)
       continue;
      if(iceb_u_SRAV(nomnn,row[3],0) != 0)
       continue;

      sprintf(strsql,"%s\n%s\n%s:%s %s:%s",
      gettext("Такой номер счет-фактуры уже введён!"),
      gettext("Материальный учёт"),
      gettext("Дата документа"),
      iceb_u_datzap(row1[0]),
      gettext("Номер документа"),
      row1[1]);
      iceb_menu_soob(strsql,wpredok);      
      return(1);
    }
     
   }
 }


if(metka_ps == 2)
 sprintf(strsql,"select datd,nomd from Usldokum  where datpnn='%04d-%02d-%02d' and kontr='%s' and nomnn='%s' \
and nomd <> '%s' and tp=1",
 gn,mn,dn,kodkontr,nomnn,nomdok);
else
 sprintf(strsql,"select datd,nomd from Usldokum where datpnn='%04d-%02d-%02d' and kontr='%s' and nomnn='%s' and tp=1",
 gn,mn,dn,kodkontr,nomnn);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
 {
  sprintf(strsql,"%s\n%s\n%s:%s %s:%s",
  gettext("Такой номер счет-фактуры уже введён!"),
  gettext("Учёт услуг"),
  gettext("Дата документа"),
  iceb_u_datzap(row[0]),
  gettext("Номер документа"),
  row[0]);
  iceb_menu_soob(strsql,wpredok);      
  return(1);
 }

if(datavnn[0] != '\0')
 {
  if(metka_ps == 2)
   sprintf(strsql,"select datd,nomd from Usldokum  where datdp='%s' and kontr='%s' and nomnn='%s' \
and nomd <> '%s' and tp=1 limit 1",
   iceb_u_dattosql(datavnn),kodkontr,nomnn,nomdok);
  else
   sprintf(strsql,"select datd,nomd from Usldokum where datdp='%s' and kontr='%s' and nomnn='%s' and tp=1 limit 1",
   iceb_u_dattosql(datavnn),kodkontr,nomnn);

  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
   {
    sprintf(strsql,"%s\n%s\n%s:%s %s:%s",
    gettext("Такой номер счет-фактуры уже введён!"),
    gettext("Учёт услуг"),
    gettext("Дата документа"),
    iceb_u_datzap(row[0]),
    gettext("Номер документа"),
    row[1]);
    iceb_menu_soob(strsql,wpredok);      
    return(1);
   }

 }



if(metka_ps == 3)
 sprintf(strsql,"select datd,nomd from Uosdok  where datpnn='%04d-%02d-%02d' and kontr='%s' and nomnn='%s' \
and nomd <> '%s' and tipz=1",
 gn,mn,dn,kodkontr,nomnn,nomdok);
else
 sprintf(strsql,"select datd,nomd from Uosdok where datpnn='%04d-%02d-%02d' and kontr='%s' and nomnn='%s' and tipz=1",
 gn,mn,dn,kodkontr,nomnn);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
 {
  sprintf(strsql,"%s\n%s\n%s:%s %s:%s",
  gettext("Такой номер счет-фактуры уже введён!"),
  gettext("Учёт основных средств"),
  gettext("Дата документа"),
  iceb_u_datzap(row[0]),
  gettext("Номер документа"),
  row[1]);
  iceb_menu_soob(strsql,wpredok);      
  return(1);
 }

if(datavnn[0] != '\0')
 {
  if(metka_ps == 3)
   sprintf(strsql,"select datd,nomd from Uosdok  where dvnn='%s' and kontr='%s' and nomnn='%s' \
and nomd <> '%s' and tipz=1 limit 1",
   iceb_u_dattosql(datavnn),kodkontr,nomnn,nomdok);
  else
   sprintf(strsql,"select datd,nomd from Uosdok where dvnn='%s' and kontr='%s' and nomnn='%s' and tipz=1 limit 1",
   iceb_u_dattosql(datavnn),kodkontr,nomnn);

  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
   {
    sprintf(strsql,"%s\n%s\n%s:%s %s:%s",
    gettext("Такой номер счет-фактуры уже введён!"),
    gettext("Учёт основных средств"),
    gettext("Дата документа"),
    iceb_u_datzap(row[0]),
    gettext("Номер документа"),
    row[1]);
    iceb_menu_soob(strsql,wpredok);      
    return(1);
   }
 }

char koment[108];
sprintf(koment,"нн%s",nomnn);

sprintf(strsql,"select sh,shk,komen from Prov where datp='%s' and kodkon='%s' and komen like '%s%%' limit 1",
iceb_u_dattosql(datann),kodkontr,koment);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
 {
  sprintf(strsql,"%s\n%s\n%s %s %s %s",
  gettext("Такой номер счет-фактуры уже введён!"),
  gettext("Главная книга"),
  datann,row[0],row[1],row[2]);
  iceb_menu_soob(strsql,wpredok);      
  return(1);
 }


return(0);
}
