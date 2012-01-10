/*$Id: iceb_start_rf.c,v 1.7 2011-01-13 13:49:59 sasa Exp $*/
/*23.12.2010	07.09.2007	Белых А.И.	iceb_start_rf.c
Чтение и выполнение запросов к базе сразу после открытия 
*/
#include <glib.h>
#include <stdlib.h>
#include        "iceb_libbuh.h"

extern char *putnansi;
extern class iceb_u_str kodirovka_iceb;

void iceb_start_rf()
{
char stroka[1024];

SQL_str row;
class SQLCURSOR cur;
iceb_sql_readkey("select VERSION()",&row,&cur,NULL);
if(atof(row[0]) < 4.1)
   return;   
sprintf(stroka,"SET NAMES %s",kodirovka_iceb.ravno_udsimv("-"));
iceb_sql_zapis(stroka,1,0,NULL);


/**********************************
FILE *ff;

class iceb_u_str imaf(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,"icebstartw.alx");

if((ff=fopen(iceb_imafnsi("icebstartw.alx"),"r")) == NULL)
 if((ff=fopen(imaf.ravno(),"r")) == NULL)
  {
   if(kodirovka_iceb.getdlinna() <= 1)
    return;
   SQL_str row;
   class SQLCURSOR cur;
   iceb_sql_readkey("select VERSION()",&row,&cur,NULL);
   if(atof(row[0]) < 4.1)
     return;   
   sprintf(stroka,"SET NAMES %s",kodirovka_iceb.ravno_udsimv("-"));
   iceb_sql_zapis(stroka,1,0,NULL);
   return;
  }
int dlinna=0;
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  if(stroka[0] == '#')
   continue;
  dlinna=strlen(stroka);  
  if(stroka[dlinna-1]  == '\n')
   stroka[dlinna-1]='\0';
  if(iceb_u_SRAV("SET NAMES",stroka,1) == 0)
   {
    SQL_str row;
    class SQLCURSOR cur;
    iceb_sql_readkey("select VERSION()",&row,&cur,NULL);
    if(atof(row[0]) < 4.1)
     continue;   
   }
  iceb_sql_zapis(stroka,1,0,NULL);
 }

fclose(ff);
**********************/


}
