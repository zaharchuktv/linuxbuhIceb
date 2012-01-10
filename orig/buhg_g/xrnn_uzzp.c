/*$Id: xrnn_uzzp.c,v 1.6 2011-02-21 07:35:59 sasa Exp $*/
/*07.05.2008	07.05.2008	Белых А.И.	xrnn_uzzp.c
Удаление записей в реестре налоговых накладных за период
*/

#include "buhg_g.h"
#include "xrnn_uzzp.h"

int xrnn_uzzp_m(class xrnn_uzzp_rek *rek_poi,GtkWidget *wpredok);
extern SQL_baza bd;

void xrnn_uzzp(GtkWidget *wpredok)
{
class xrnn_uzzp_rek data;

if(xrnn_uzzp_m(&data,NULL) != 0)
 return;


short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data.datan.ravno(),&dk,&mk,&gk,data.datak.ravno(),wpredok) != 0)
 return;

short d=dn;
short m=mn;
short g=gn;


while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  if(iceb_pbpds(m,g,wpredok) != 0)
   return;
  iceb_u_dpm(&d,&m,&g,3);
 }

if(iceb_parol(0,wpredok) != 0)
 return;

class iceb_gdite_data gdite;
iceb_gdite(&gdite,1,wpredok);

char strsql[512];
sprintf(strsql,"delete from Reenn where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",
gn,mn,dn,gk,mk,dk);

iceb_sql_zapis(strsql,1,0,wpredok);

iceb_refresh();

int kolud=sql_rows(&bd);
//printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);

sprintf(strsql,"delete from Reenn1 where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",
gn,mn,dn,gk,mk,dk);


iceb_sql_zapis(strsql,1,0,wpredok);

int kolud1=sql_rows(&bd);

int	metka_ver=0;
SQL_str row;
class SQLCURSOR cur;
strcpy(strsql,"select VERSION()");

iceb_sql_readkey(strsql,&row,&cur,wpredok);
if(iceb_u_SRAV(row[0],"3.",1) == 0)
  metka_ver=1;  

char imatab[56];

//printw("VERSION:%s\n",row[0]);
for(int i=0; i < 2 ;i++)
 { 
  if(i == 0)
    strcpy(imatab,"Reenn");
  if(i == 1)
    strcpy(imatab,"Reenn1");

//  printw(gettext("Оптимизация таблицы %s !\n"),imatab);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table %s",imatab);

  if(metka_ver == 1)
   {
    iceb_sql_zapis(strsql,1,0,wpredok);
   }
  else
   {
    iceb_sql_readkey(strsql,wpredok);
   }

 }

//printw("%s: %d\n",gettext("Количество удаленных записей"),kolud1);

sprintf(strsql,"%s: %d",gettext("Количество удаленных записей"),kolud+kolud1);
iceb_menu_soob(strsql,wpredok);
}
