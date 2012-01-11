/*$Id: provndsw.c,v 1.4 2011-01-13 13:49:52 sasa Exp $*/
/*11.11.2008	21.10.2002	Белых А.И.	provndsw.c
Проверка НДС
Если вернули 0 подходит условиям поиска
             1 нет
*/
#include        <stdlib.h>
#include        <math.h>
#include        "../headers/buhg_g.h"


int provndsw(const char *nds,SQL_str row)
{

if(nds[0] == '\0')
 return(0);
 
//Карточки с нулевым НДС
if(nds[0] == '0' && atof(row[9]) != 0.)
 return(1);

//Карточки с НДС нужного значения
if(nds[0] != '+' && nds[0] != '0' && (fabs(atof(row[9]) - atof(nds)) > 0.009 || atoi(row[3]) != 0))
 return(1);

//Карточки в цене которых включено НДС нужного значения
if(nds[0] == '+' && nds[1] != '\0' && (fabs(atof(row[9]) - atof(nds)) > 0.009 || atoi(row[3]) != 1) )
 return(1);

//Карточки в цене которых НДС любого значения
if(nds[0] == '+' && nds[1] == '\0' &&  atoi(row[3]) != 1 )
 return(1);

return(0);

}
