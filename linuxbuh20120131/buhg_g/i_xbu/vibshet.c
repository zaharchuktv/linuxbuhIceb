/*$Id: vibshet.c,v 1.10 2011-02-21 07:35:58 sasa Exp $*/
/*21.01.2004	05.01.2004	Белых А.И.	vibshet.c
Выбор счета для выполнения проводок
*/
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

void m_vprov_bs(char*);
void m_vprov_bs_gr(char*);
void m_vprov_nbs(char*);
void m_vprov_nbs_gr(char*);


void vibshet(int metka_gr) //0-обычные проводки 1- по гуппам контрагентов
{
char shet[32];
iceb_u_str repl;

memset(shet,'\0',sizeof(shet));

if(metka_gr == 0)
  repl.plus(gettext("Введите номер счета (субсчета)"));
else
  repl.plus(gettext("Введите номер счета с развернутым сальдо"));

naz:;

if(iceb_menu_vvod1(repl.ravno(),shet,sizeof(shet),NULL) != 0)
 return;

printf("vibshet shet=%s\n",shet);

iceb_u_str shetv;
iceb_u_str naimv;

if(shet[0] == '\0')
 {
 if(iceb_l_plansh(1,&shetv,&naimv,NULL) != 0)
   return;
 else
  strncpy(shet,shetv.ravno(),sizeof(shet)-1);
 }
OPSHET rekshet;
if(iceb_prsh1(shet,&rekshet,NULL) != 0)
  goto naz;

printf("vibshet shet=%s stat=%d\n",shet,rekshet.stat);

if(metka_gr == 1)
 if(rekshet.saldo == 0)
  {
   iceb_u_str repl;
   repl.plus(gettext("Счет имеет свернутое сальдо !"));
   iceb_menu_soob(&repl,NULL);
   goto naz;  
  }

if(metka_gr == 0)
 {
  if(rekshet.stat == 0)
   m_vprov_bs(shet);
  else
   m_vprov_nbs(shet);
 }
if(metka_gr == 1)
 {
  if(rekshet.stat == 0)
   m_vprov_bs_gr(shet);
  else
   m_vprov_nbs_gr(shet);
 }
}
