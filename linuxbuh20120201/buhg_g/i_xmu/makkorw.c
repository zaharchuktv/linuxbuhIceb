/* $Id: makkorw.c,v 1.11 2011-02-21 07:35:55 sasa Exp $ */
/*14.12.2010	20.01.1999	Белых А.И.	makkorw.c
Работа с файлом настройки для корректировки суммы по документа
Если все в норме возвращаем 0
                            1- ошибка
*/
#include        <stdlib.h>
#include        <errno.h>
#include <unistd.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

extern SQL_baza bd;

int	        makkorw(iceb_u_spisok *MENU,
short tr, //1-создать меню 2-взять настройку для выбранной корректировки
iceb_u_str *naikor, //Наименование корректировки*/
iceb_u_double *maskor,
const char *imaf)
{
char		br[1024],br1[1024];
short		ns,i,i1;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }


if(tr == 1)
 {
  ns=0;
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    if(row_alx[0][0] == '#')
     continue;

    if(iceb_u_SRAV(row_alx[0],"Наименование корректировки",1) == 0)
     {
      iceb_u_pole(row_alx[0],br,2,'|');
      MENU->plus(br);
      ns++;
     }
   }
 }

if(tr == 2)
 {
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    if(row_alx[0][0] == '#')
     continue;

    if(iceb_u_SRAV(row_alx[0],"Наименование корректировки",1) == 0)
     {
      iceb_u_pole(row_alx[0],br,2,'|');
      if(iceb_u_SRAV(naikor->ravno(),br,0) == 0)
       {

        cur_alx.read_cursor(&row_alx);
        iceb_u_pole(row_alx[0],br,2,'|');
        ns=iceb_u_pole2(br,',');        
        if(ns == 0 && br[0] != '\0')
         ns=1;
        maskor->make_class(ns*2+1);        
        maskor->plus(ns,0);
        for(i=1; i < ns+1; i++)
         {
          if(iceb_u_pole(br,br1,i,',') != 0)
            strcpy(br1,br); 
          maskor->plus(iceb_u_atof(br1),i);
         }
        cur_alx.read_cursor(&row_alx);
        iceb_u_pole(row_alx[0],br,2,'|');
        i1=1;
        for(i=ns+1; i < ns*2+1; i++)
         {
          memset(br1,'\0',sizeof(br1));
          if(iceb_u_pole(br,br1,i1,',') != 0)
            strcpy(br1,br); 
          i1++;
          maskor->plus(iceb_u_atof(br1),i);
         }
       }
     }
    
   }
 }


return(0);
}
