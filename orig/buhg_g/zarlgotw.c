/*$Id: zarlgotw.c,v 1.13 2011-04-18 09:10:20 sasa Exp $*/
/*14.04.2011	26.04.2004	Белых А.И.	zarlgotw.c
Чтение процента льгот по подоходному налогу
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"

extern SQL_baza bd;
extern int metka_psl; /*Метка применения социальной льготы 0-применяется 1-нет*/

float  zarlgotw(int tabn,short mr,short gr,int *kollgot,class iceb_u_str *kod_lgot,FILE *ff_prot,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
char bros[512];

*kollgot=0;
kod_lgot->new_plus("");

if(metka_psl == 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Расчёт без применения социальной льготы!!!\n",__FUNCTION__);
  return(0.);
 }
sprintf(strsql,"select str from Alx where fil='zarlgot.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки zarlgot.alx\n");
  return(0.);
 }
long tabnom;
float procent=0.;
short d,m,g;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
    continue;
  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') != 0)
    continue;

  tabnom=atoi(bros);

  if(tabnom != tabn)
    continue;

  iceb_u_polen(row_alx[0],bros,sizeof(bros),3,'|');

  if(bros[0] != '\0')
   {
    d=m=g=0;
    iceb_u_rsdat(&d,&m,&g,bros,1);
    if(iceb_u_sravmydat(1,mr,gr,d,m,g) < 0)
     continue;
   }   

  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|') != 0)
    continue;

  procent=iceb_u_atof(bros);

  if(ff_prot != NULL)
   fprintf(ff_prot,"Льгота %.f%%\n",procent);

  iceb_u_polen(row_alx[0],kollgot,4,'|');
  iceb_u_polen(row_alx[0],kod_lgot,5,'|');
  if(procent != 0. && *kollgot == 0)
   *kollgot=1;
   
   
 }

return(procent);

}
