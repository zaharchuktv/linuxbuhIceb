/*$Id: rozn_oth.c,v 1.6 2011-01-13 13:49:52 sasa Exp $*/
/*02.05.2005	02.05.2005	Белых А.И.	rozn_oth.c
Получение отчётов по розничной продаже
*/
#include "../headers/buhg_g.h"
#include "rozn_oth.h"

int rozn_oth_m(class rozn_oth_rr *rek_ras,int);
int rozn_oth1_r(class rozn_oth_rr *datark,int kassa,const char *fam,GtkWidget *wpredok);
int rozn_oth2_r(class rozn_oth_rr *datark,int kassa,const char *fam,GtkWidget *wpredok);

iceb_u_str rozn_oth_rr::datan;
iceb_u_str rozn_oth_rr::datak;

void rozn_oth(int metka,int kassa,const char *fam) //0-отчёт по продажам 1-сводный отчёт
{
class rozn_oth_rr data;

if(rozn_oth_m(&data,metka) != 0)
 return;

if(metka == 0)
 if(rozn_oth1_r(&data,kassa,fam,NULL) != 0)
  return;
if(metka == 1)
 if(rozn_oth2_r(&data,kassa,fam,NULL) != 0)
  return;
iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
