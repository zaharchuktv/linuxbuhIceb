/* $Id: vzrkbzw.c,v 1.8 2011-02-21 07:35:58 sasa Exp $ */
/*15.05.2010	22.07.1997	Белых А.И.	vzrkbzw.c
Взять реквизиты организаций из базы для заполнения платежных 
поручений
*/
#include        "buhg_g.h"
#include        "dok4w.h"

extern class REC rec;
extern SQL_baza bd;

int vzrkbzw(const char *kodo,GtkWidget *wpredok)
{
SQL_str         row;
char		strsql[512];

SQLCURSOR cur;

char kontr00[56];
memset(kontr00,'\0',sizeof(kontr00));
    
sprintf(strsql,"select naikon,kod,naiban,mfo,nomsh,adresb,pnaim,rsnds from \
Kontragent where kodkon='%s'",kodo);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s\n%s",gettext("Не найден код контрагента в общем списке!"),kodo);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
else
 {
  rec.kodor1.new_plus(kodo);
  if(iceb_u_pole(row[0],strsql,1,'(') != 0)    
   strncpy(strsql,row[0],sizeof(strsql)-1);
  rec.naior1.new_plus(strsql);
  
  rec.kod1.new_plus(row[1]);
  rec.naiban1.new_plus(row[2]);
  rec.mfo1.new_plus(row[3]);
  rec.nsh1.new_plus(row[4]);
  rec.gorod1.new_plus(row[5]);
  rec.rsnds.new_plus(row[7]);
 }




iceb_poldan("Код контрагента для платёжного поручения",kontr00,"nastdok.alx",wpredok);
if(kontr00[0] == '\0')
  strcpy(kontr00,"00");
  
sprintf(strsql,"select naikon,kod,naiban,mfo,nomsh,adresb,pnaim,rsnds from \
Kontragent where kodkon='%s'",kontr00);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  char strok[1024];
  sprintf(strok,"vzrkbz-%s %s !",gettext("Не найден код контрагента"),kontr00);
  iceb_menu_soob(strok,wpredok);
  return(1);
 }
else
 {
  rec.kodor.new_plus("00");     
  rec.naior.new_plus("");

  if(row[6][0] == '\0')          
   {

    if(iceb_u_polen(row[0],strsql,sizeof(strsql),1,'(') != 0)    
      strncpy(strsql,row[0],sizeof(strsql)-1);
   }
  else
    strncpy(strsql,row[6],sizeof(strsql)-1);
  rec.naior.new_plus(strsql);


  rec.kod.new_plus(row[1]);
  rec.naiban.new_plus(row[2]);
  rec.mfo.new_plus(row[3]);
  rec.nsh.new_plus(row[4]);
  rec.gorod.new_plus(row[5]);
  rec.rsnds.new_plus(row[7]);
 }

return(0);
}
