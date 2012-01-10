/*$Id: iceb_insfil.c,v 1.4 2011-02-21 07:36:07 sasa Exp $*/
/*25.01.2011	12.10.2010	Белых А.И.	iceb_insfil.c
Вставка содержимого настроечного файла в файл
Возвращает количество вставленных строк
*/
#include "iceb_libbuh.h"
extern SQL_baza bd;

int iceb_insfil(const char *imaf,FILE *ff,GtkWidget *wpredok)
{
int kolih_str_ins=0;
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);
printf("%s-%s\n",__FUNCTION__,imaf);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr == 0)
 {
  printf("Файл не найден!\n");
  return(0);
 }
while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;
  fprintf(ff,"%s",row[0]);
  kolih_str_ins++;
 }

return(kolih_str_ins);
}