/*$Id: iceb_mariq_slvi.c,v 1.11 2011-02-21 07:36:07 sasa Exp $*/
/*24.12.2004	24.12.2004	Белых А.И.	iceb_mariq_slvi.c
Служебное внесение/извлечение денег из кассы
*/

#include <stdlib.h>
#include "iceb_libbuh.h"


int 	iceb_mariq_slvi(struct KASSA *kasr,
double *sum, //сумма которая будет внесена/изъята
int metka) //0-внесение 1-извлечение
{
char		strsql[512];
char		suma[20];
iceb_u_str repl;

*sum=0.;
if(metka == 0)
 repl.plus(gettext("Введите сумму служебного внесения."));
if(metka == 1)
 repl.plus(gettext("Введите сумму служебного извлечения."));
repl.ps_plus(gettext("Касса"));
repl.plus(":");
repl.plus(kasr->nomer);

memset(suma,'\0',sizeof(suma));
if(iceb_menu_vvod1(&repl,suma,20,NULL) != 0)
 return(1);

*sum=atof(suma);
if(metka == 1)
 *sum*=(-1);
 
sprintf(strsql,"FSTART: MARIA_SLUG\n\
%s\n\
%s\n\
%.2f\n\
FEND:\n",
kasr->prodavec.ravno(),kasr->parol,*sum);  
//  printw("\n%s\n",strsql);
return(iceb_cmd(kasr, strsql,NULL));
}
