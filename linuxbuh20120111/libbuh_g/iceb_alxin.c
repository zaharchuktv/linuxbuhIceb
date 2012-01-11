/*$Id: iceb_alxin.c,v 1.2 2011-01-13 13:49:58 sasa Exp $*/
/*07.09.2010	07.09.2010	Белых А.И.	iceb_alxin.c
Загрузка файла alx в базу
*/
#include <glib.h>
#include <errno.h>
#include "iceb_libbuh.h"

void iceb_alxin(const char *imaf,GtkWidget *wpredok)
{
char stroka[5120];
char strsql[5632];
FILE *ff;
class iceb_u_str imafz;
class iceb_u_str strin;
int nom_op=iceb_getuid(wpredok);
time_t vrem=time(NULL);
int nomer_str=0;

int kolb=iceb_u_pole2(imaf,G_DIR_SEPARATOR);
if(kolb == 0)
 imafz.plus(imaf);
else
 iceb_u_polen(imaf,&imafz,kolb,G_DIR_SEPARATOR);  
if((ff=fopen(imaf,"r")) == NULL)
  {
   iceb_er_op_fil(imaf,"",errno,wpredok);
   return;
  }

class iceb_lock_tables kkk("LOCK TABLES Alx WRITE");

sprintf(strsql,"delete from Alx where fil='%s'",imafz.ravno());
if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return;
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  strin.new_plus(stroka);  
  sprintf(strsql,"insert into Alx values('%s',%d,%d,%ld,'%s')",
  imafz.ravno(),++nomer_str,nom_op,vrem,strin.ravno_filtr());

  iceb_sql_zapis(strsql,1,0,wpredok);
 }
fclose(ff);

}



