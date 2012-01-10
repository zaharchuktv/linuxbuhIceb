/*$Id: zarprtnw.c,v 1.10 2011-02-21 07:36:00 sasa Exp $*/
/*29.11.2010	07.02.2008	Белых А.И.	zarprtnw.c
Проверка есть ли табельный номер в файле и входит ли дата в диапазон действия настройки
Если вернули 1- табельный номер есть
             0- нет
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"

extern SQL_baza bd;

int zarprtnw(short mr,short gr,int tabn,const char *imaf,FILE *ff_prot,GtkWidget *wpredok)
{
char		strsql[1024];
char datan[112];
char datak[112];
class iceb_u_file fil;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

if(ff_prot != NULL)
 fprintf(ff_prot,"Проверяем файл %s\n",imaf);

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки %s\n",imaf);
  return(1);
 }

short dr=1;
short d,m,g;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;



  if(tabn != atoi(row_alx[0]))
    continue;
  if(ff_prot != NULL)
    fprintf(ff_prot,"Найдена настройка\n%s",row_alx[0]);

  if(iceb_u_polen(row_alx[0],datan,sizeof(datan),2,'|') == 0)
   {
    if(datan[0] != '\0')
     {
      if(iceb_u_rsdat(&d,&m,&g,datan,1) == 0)
       if(iceb_u_sravmydat(dr,mr,gr,1,m,g) < 0)
        {
         if(ff_prot != NULL)
          fprintf(ff_prot,"Дата начала %d.%d.%d < %d.%d.%d\n",dr,mr,gr,1,m,g);
         continue;      
        }  
     }   
   }

  if(iceb_u_polen(row_alx[0],datak,sizeof(datak),3,'|') == 0)
   {
    if(datak[0] != '\0')
     {
      if(iceb_u_rsdat(&d,&m,&g,datak,1) == 0)
       if(iceb_u_sravmydat(dr,mr,gr,1,m,g) >  0)
        {
         if(ff_prot != NULL)
          fprintf(ff_prot,"Дата конца %d.%d.%d > %d.%d.%d\n",dr,mr,gr,1,m,g);
         continue;      
        }
     }   
   }
  return(1);
 }

return(0);
}
