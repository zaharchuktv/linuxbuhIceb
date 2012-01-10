/* $Id: iceb_udprgr.c,v 1.10 2011-02-21 07:36:08 sasa Exp $ */
/*11.11.2008	14.06.1998	Белых А.И.	iceb_udprgr.c
Удаление группы проводок принадлежащих одному документу
*/
#include        "iceb_libbuh.h"

extern SQL_baza	bd;

int iceb_udprgr(const char *kto,short dd,short md,short gd,const char *nn,
int pod,int tipz,GtkWidget *wpredok)
{
char		strsql[512];

if(iceb_pbp(kto,dd,md,gd,nn,pod,tipz,gettext("Проводки удалить невозможно!"),wpredok) != 0)
 return(1);

sprintf(strsql,"delete from Prov where kto='%s' and pod=%d \
and nomd='%s' and datd = '%04d-%02d-%02d' and tz=%d",
kto,pod,nn,gd,md,dd,tipz);

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
   iceb_msql_error(&bd,"iceb_udprgr-Ошибка удаления группы проводок !",strsql,wpredok);
  return(1);
 }
return(0);
}
