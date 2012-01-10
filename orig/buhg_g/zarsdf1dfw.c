/*$Id: zarsdf1dfw.c,v 1.13 2011-09-05 08:18:27 sasa Exp $*/
/*31.08.2011	01.04.2004	Белых А.И.	zarsdf1dfw.c
Чтение списка кодов признаков доходов с кодами начислений которые к ним относятся.
*/
#include <stdlib.h>
#include <errno.h>
#include "buhg_g.h"
#include        <unistd.h>
#include "sprkvrt1w.h"

extern SQL_baza bd;

void zarsdf1dfw(class sprkvrt1_nast *nast,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

//char stroka[1024];
char bros[1024];
char kodd[1024];

sprintf(strsql,"select str from Alx where fil='zarsdf1df.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarsdf1df.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  memset(kodd,'\0',sizeof(kodd));
  if(iceb_u_polen(row_alx[0],kodd,sizeof(kodd),1,'|') != 0)
   continue;
  
  if(iceb_u_SRAV(kodd,"Коды не входящие в форму 1ДФ",0) == 0)
   {
    iceb_u_sozmas(&nast->kodnvf8dr,row_alx[0]);
    continue;
   }
  if(iceb_u_SRAV(kodd,"Коды удержаний не входящие в форму 1ДФ",0) == 0)
   {
    iceb_u_sozmas(&nast->kodud,row_alx[0]);
    continue;
   }

  if(iceb_u_SRAV("Счета для поиска доходов по коду",kodd,1) == 0)
   {
  
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    if(bros[0] == '\0')
     continue;

    iceb_fplus(1,bros,&nast->sheta,&cur_alx);
     
    iceb_u_polen(row_alx[0],bros,sizeof(bros),7,' ');
    nast->kodi_pd_prov.plus(atoi(bros),-1);

    continue;
   }
  if(iceb_u_SRAV("Физическое лицо",kodd,0) == 0)
   {
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    if(iceb_u_SRAV(bros,"Вкл",1) == 0)
     {
      nast->TYP=1;
     }
    continue;
   }

  if(atoi(kodd) == 0)
   continue;

  if(atoi(kodd) == 133)
   {
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    strncpy(nast->gosstrah,bros,sizeof(nast->gosstrah));
    continue;
   }   

  nast->kodi_pd.plus(atoi(kodd),-1);
  iceb_u_polen(row_alx[0],kodd,sizeof(kodd),2,'|');
  nast->kodi_nah.plus(kodd);    

 }

}
