/*$Id: saldokasw.c,v 1.4 2011-02-21 07:35:57 sasa Exp $*/
/*27.02.2009	27.02.2009	Белых А.И.	saldokasw.c
Перенос сальдо для подсистемы "Материальный учёт"
*/
#include <stdlib.h>
#include "buhg_g.h"

int saldokasw_r(short godp,GtkWidget *wpredok);


void saldokasw()
{
char strsql[512];
char gods[5];
memset(gods,'\0',sizeof(gods));
sprintf(strsql,"Перенос сальдо для подсистемы \"Учёт кассовых ордеров\" по кодам целевго назначения\n\
Введите год на который нужно перенести сальдо\n");

if(iceb_menu_vvod1(strsql,gods,sizeof(gods),NULL) != 0)
 return;

saldokasw_r(atoi(gods),NULL);

}
