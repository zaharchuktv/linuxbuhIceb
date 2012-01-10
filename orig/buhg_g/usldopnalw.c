/*$Id: usldopnalw.c,v 1.9 2011-02-21 07:35:58 sasa Exp $*/
/*29.09.2009	21.03.2002	Белых А.И.	usldopnalw.c
Определение процента и наименования дополнительного налога
Если вернули 0 нашли налог
             1 нет
*/

#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"

extern SQL_baza bd;

int usldopnalw(int tipz, // 1+ 2 -
const char *kodop, //Код операции
float *procent,
iceb_u_str *naim)
{
char	bros[1024];
int	vozvr=1;

*procent=0.;


SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"uslnast.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  
  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|') != 0)
    continue;
  if(tipz == 1 && iceb_u_strstrm(bros,"+") != 1)
    continue;
  if(tipz == 2 && iceb_u_strstrm(bros,"-") != 1)
    continue;
    
  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),3,'|') != 0)
    continue;
  if(iceb_u_proverka(bros,kodop,0,1) != 0)
    continue;

  memset(bros,'\0',sizeof(bros));
  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),4,'|') != 0)
    continue;
  *procent=iceb_u_atof(bros);
  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),5,'|') != 0)
    continue;
  naim->new_plus(bros);
  vozvr=0;
  break;  
 }
 
return(vozvr);
}
