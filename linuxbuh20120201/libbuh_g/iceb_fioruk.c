/*$Id: iceb_fioruk.c,v 1.8 2011-06-07 08:52:29 sasa Exp $*/
/*16.03.2010	04.04.2008	Белых А.И.	iceb_fioruk.c
Получение фамилий главного бухгалтера и руководителя предприятия
Если вернули 0-нашли
             1-нет
*/
#include "iceb_libbuh.h"



int iceb_fioruk(int metka, /*1-ищем фамилию руководителя 2-главного бухгалтера*/
class iceb_fioruk_rk *rek,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str imaf_nz("zarnast.alx");

rek->clear();

//iceb_imafn("zarnast.alx",&imaf_nz);
char tabnom[512];
memset(tabnom,'\0',sizeof(tabnom));

if(metka == 1)
  iceb_poldan("Табельный номер руководителя",tabnom,imaf_nz.ravno(),wpredok);
if(metka == 2)
  iceb_poldan("Табельный номер бухгалтера",tabnom,imaf_nz.ravno(),wpredok);
char strsql[512];
if(tabnom[0] != '\0')
 {
  sprintf(strsql,"select fio,dolg,inn,telef from Kartb where tabn=%s",tabnom);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s-Не найден табельный номер %s !",__FUNCTION__,tabnom);
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
  else
   {
    char stroka[512];
    iceb_u_polen(row[0],&rek->famil,1,' ');
    iceb_u_polen(row[0],&rek->ima,2,' ');
    iceb_u_polen(row[0],&rek->oth,3,' ');
    
    sprintf(stroka,"%.*s.%.*s.",
    iceb_u_kolbait(1,rek->ima.ravno()),rek->ima.ravno(),   
    iceb_u_kolbait(1,rek->oth.ravno()),rek->oth.ravno());
    
    strcpy(rek->inic,stroka);
    rek->famil_inic.new_plus(rek->famil.ravno());
    rek->famil_inic.plus(" ",rek->inic);
    rek->fio.new_plus(row[0]);
    rek->dolg.new_plus(row[1]);
    rek->inn.new_plus(row[2]);
    
    strncpy(stroka,row[3],sizeof(stroka)-1);
    iceb_u_ud_simv(stroka,"-/.()+,");
         
    rek->telef.new_plus(stroka);
    
   } 
 } 

return(0);

}
