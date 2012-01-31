/*$Id: iceb_spks.c,v 1.11 2011-02-21 07:36:08 sasa Exp $*/
/*11.11.2008	18.12.2003	Белых А.И.	iceb_spks.c
Расчет сальдо по контрагенту и конкретному счету с развернутым сальдо
*/
#include  <time.h>
#include  "iceb_libbuh.h"
#include  "iceb_spks.h"

char iceb_spks_data::datn[11];
char iceb_spks_data::datk[11];
char iceb_spks_data::shets[RAZMER_SHETS];

int     iceb_spks_v(class iceb_spks_data *data,iceb_u_str *zag,GtkWidget *window);
int     iceb_spks_r(class iceb_spks_data *data,iceb_u_str *zag);

extern SQL_baza	bd;
extern char *name_system;
extern short startgodb;

void  iceb_spks(const char *shet,const char *kodkon,GtkWidget *window)
{
time_t          tmm;
struct  tm      *bf;
iceb_spks_data data;
iceb_u_str zag;
char strsql[512];
SQL_str row;
SQLCURSOR cur;
printf("iceb_spks\n");

data.shetsrs.new_plus(shet);

strncpy(data.shet,data.shetsrs.ravno(),sizeof(data.shet));


if(data.datn[0] == '\0')
 {
  time(&tmm);
  bf=localtime(&tmm);
  if(startgodb != 0)  
    sprintf(data.datn,"01.01.%d",startgodb);
  else
    sprintf(data.datn,"01.01.%d",bf->tm_year+1900);
  sprintf(data.datk,"%02d.%02d.%d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
  
 }

data.kontr.plus(kodkon);

zag.plus(gettext("Счет"));
zag.plus(": ");
zag.plus_ps(shet);

zag.plus_ps(gettext("Расчет сальдо по контрагенту."));
zag.plus(kodkon);
zag.plus(" ");

sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodkon);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  zag.plus(row[0]);
  data.naikontr.plus(row[0]);
 } 
if( iceb_spks_v(&data,&zag,window) == 0)
  return;
printf("iceb_spks расчет\n");

sprintf(strsql,"%s %s%s %s %s%s",
gettext("Период с"),
data.datn,
gettext("г."),
gettext("по"),
data.datk,
gettext("г."));

zag.ps_plus(strsql);

if(window != NULL)
  gtk_widget_hide(window);


if(iceb_spks_r(&data,&zag) == 0)
  iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);

if(window != NULL)
  gtk_widget_show_all(window);

}
