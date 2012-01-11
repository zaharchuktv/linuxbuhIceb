/*$Id: plc_gk.c,v 1.4 2011-01-13 13:49:52 sasa Exp $*/
/*21.01.2004	21.01.2004	Белых А.И.	plc_gk.c
Проверка логической целосности базы "Главная книга"
*/
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

int  m_plc_gk(char *datan,char *datak);
int  plc_gk_r(char *datan,char *datak);

void  plc_gk(void)
{
char datan[11];
char datak[11];


if(m_plc_gk(datan,datak) != 0)
 return;

plc_gk_r(datan,datak);

}
