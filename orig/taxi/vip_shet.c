/*23.03.2006	22.03.2006	Белых А.И.	vip_shet.c
Запуск нужной программы выписки счетов
*/

#include <stdlib.h>
#include "i_rest.h"

int  vip_shet(short god,iceb_u_str *nomdok,
GtkWidget *wpredok)
{

char bros[512];

int metka=0;
iceb_poldan("Вариант работы режима выписки счетов",bros,"restnast.alx",wpredok);
metka=atoi(bros);
int voz;

if(metka == 2)
 voz=l_restdok(god,nomdok,wpredok);
else
 voz=l_restdok2(god,nomdok,wpredok);

return(voz);
}
