/*$Id: i_vv_start.c,v 1.4 2011-01-13 13:50:09 sasa Exp $*/
/*11.09.2007	07.09.2007	Белых А.И.	i_vv_start.c
Чтение и выполнение запросов к базе сразу после открытия 
*/
//#include <iceb_util.h>
//#include <libsql.h>
#include <iceb_libbuh.h>
extern SQL_baza	bd;

void i_vv_start(const char *putnansi,const char *imabaz)
{
iceb_start_rf();
/*****************************************
FILE *ff;
class iceb_u_str imaf;
imaf.plus("putnansi");
imaf.plus("/");
imaf.plus("imabaz");
imaf.plus("/");
imaf.plus("icebstart.alx");

//iceb_imafn("icebstart.alx",&imaf);

if((ff=fopen(imaf.ravno(),"r")) == NULL)
 return;

char stroka[1024];
int dlinna=0;
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  if(stroka[0] == '#')
   continue;
  dlinna=strlen(stroka);  
  if(stroka[dlinna-1]  == '\n')
   stroka[dlinna-1]='\0';
  sql_zap(&bd,stroka);
 }

fclose(ff);
*****************************/


}
