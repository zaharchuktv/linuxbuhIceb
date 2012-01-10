/*$Id: iceb_prsh1.c,v 1.9 2011-02-21 07:36:07 sasa Exp $*/
/*11.11.2008	16.10.2000	Белых А.И.	iceb_prsh1.c
Проверка счета

Если вернули 0 - все впорядке
	     1 - нет
*/
#include	"iceb_libbuh.h"


int iceb_prsh1(const char *shet,struct OPSHET *shetv,GtkWidget *wpredok)
{
int		kl;
char		bros[512];

if((kl=iceb_prsh(shet,shetv,wpredok)) == 0)
 {
  iceb_u_str SOOB;
  sprintf(bros,gettext("Нет счета %s в плане счетов !"),shet);
  SOOB.plus(bros);
  iceb_menu_soob(&SOOB,wpredok);
  return(1);
 }
if(kl == 2)
 {
  iceb_u_str SOOB;
  sprintf(bros,gettext("Счет %s имеет субсчета ! Введите нужный субсчет."),shet);
  SOOB.plus(bros);
  iceb_menu_soob(&SOOB,wpredok);
  return(2);
 }
return(0);

}
