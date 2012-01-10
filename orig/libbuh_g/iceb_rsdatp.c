/*$Id: iceb_rsdatp.c,v 1.12 2011-02-21 07:36:07 sasa Exp $*/
/*11.11.2008	20.10.2000	Белых А.И.	iceb_rsdatp.c
Проверка правильности ввода дат начала и конца поиска
с расшифровкой
Если вернули 0-порядок
             1-нет
*/

#include	"iceb_libbuh.h"


int   iceb_rsdatp(const char *datn, //Дата начала
class iceb_u_str *datk, //Дата конца
GtkWidget *wpredok)
{
short dn,mn,gn,dk,mk,gk;
int voz=0;
char stroka[112];

if((voz=iceb_rsdatp(&dn,&mn,&gn,datn,&dk,&mk,&gk,datk->ravno(),wpredok)) != 0)
 return(voz);

if(datk->getdlinna() <= 1)
 {
  sprintf(stroka,"%02d.%02d.%04d",dk,mk,gk);
  datk->new_plus(stroka);
 }

return(0);
}
/*****************************************************/
int   iceb_rsdatp(const char *datn, //Дата начала
const char *datk, //Дата конца
GtkWidget *wpredok)
{
short dn,mn,gn,dk,mk,gk;

return(iceb_rsdatp(&dn,&mn,&gn,datn,&dk,&mk,&gk,datk,wpredok));

}

/*************************************/

int   iceb_rsdatp(short *dn,short *mn,short *gn,const char *datnn, //Дата начала
short *dk,short *mk,short *gk,const char *datkk, //Дата конца
GtkWidget *wpredok)
{

char datn[56];
char datk[56];
memset(datn,'\0',sizeof(datn));
memset(datk,'\0',sizeof(datk));
strncpy(datn,datnn,sizeof(datn)-1);
strncpy(datk,datkk,sizeof(datk)-1);

if(iceb_u_rsdat(dn,mn,gn,datn,0) != 0)
 { 
  iceb_u_str repl;
  repl.plus(gettext("Не верно ведена дата начала !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 } 

if(datk[0] == '\0')
 {
  *dk=*dn; *mk=*mn; *gk=*gn;
  iceb_u_dpm(dk,mk,gk,5); 
 } 
else   
 if(iceb_u_rsdat(dk,mk,gk,datk,0) != 0)
  { 
   iceb_u_str repl;
   repl.plus(gettext("Не верно ведена дата конца !"));
   iceb_menu_soob(&repl,wpredok);
   return(1);
  } 

if(iceb_u_sravmydat(*dn,*mn,*gn,*dk,*mk,*gk) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Дата начала не может быть больше даты конца !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
