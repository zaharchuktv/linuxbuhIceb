/*$Id: iceb_pbp.c,v 1.11 2011-02-21 07:36:07 sasa Exp $*/
/*07.09.2010	18.03.2008	Белых А.И.	iceb_pbp.c
Проверка заблокированы ли даты по которым сделаны проводки к документу
Если вернули 0-проводки не заблокированы
             1-хотябы одна проводка заблокирована
*/
#include        "iceb_libbuh.h"

extern SQL_baza bd;
extern int iceb_kod_podsystem;

int iceb_pbp(int metka_ps,short dd,short md,short gd,const char *nomdok,
int pod,int tipz,const char *soob,
GtkWidget *wpredok)
{
char kto[30];
memset(kto,'\0',sizeof(kto));

iceb_mpods(metka_ps,kto);

return(iceb_pbp(kto,dd,md,gd,nomdok,pod,tipz,soob,wpredok));

}
/********************************/
/********************************/

int iceb_pbp(const char *kto,short dd,short md,short gd,const char *nomdok,
int pod,int tipz,const char *soob,
GtkWidget *wpredok)
{
char		strsql[512];

SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

sprintf(strsql,"select distinct datp,sh,shk from Prov where kto='%s' and pod=%d \
and nomd='%s' and datd = '%04d-%02d-%02d' and tz=%d and deb <> 0.",
kto,pod,nomdok,gd,md,dd,tipz);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"",strsql,wpredok);
  return(1);
 }
short d,m,g;

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  if(iceb_pblok(m,g,iceb_kod_podsystem,wpredok) != 0)
   {
    class iceb_u_str repl;
    sprintf(strsql,gettext("Проводки на дату %d.%d г. заблокированы!"),m,g);
    repl.plus(strsql);
    if(soob[0] != '\0')
      repl.ps_plus(soob);     
    iceb_menu_soob(&repl,wpredok);    
    return(1);  
   }
/********************  
  if(iceb_prblm(m,g,imaf_blok.ravno(),wpredok) != 0)
   {
    class iceb_u_str repl;
    sprintf(strsql,gettext("Проводки на дату %d.%d г. заблокированы!"),m,g);
    repl.plus(strsql);
    if(soob[0] != '\0')
      repl.ps_plus(soob);     
    iceb_menu_soob(&repl,wpredok);    
    return(1);  
   }
*****************/ 
  if(iceb_pbsh(m,g,row[1],row[2],"",wpredok) != 0)
   return(1);

 }

return(0);
}
