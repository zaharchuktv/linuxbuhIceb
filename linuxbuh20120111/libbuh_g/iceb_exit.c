/*$Id: iceb_exit.c,v 1.5 2011-02-21 07:36:06 sasa Exp $*/
/*07.05.2005	14.12.2004	Белых А.И.	iceb_exit.c
Завершение работы программы
*/
#include <stdlib.h>
#include "iceb_libbuh.h"

extern SQL_baza	bd;

int iceb_exit(int metka)
{

sql_closebaz(&bd);
    
exit(metka);

}
