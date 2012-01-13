/*$Id: iceb_u_srav_r.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*11.09.2009	21.02.2007	Белых А.И.	iceb_u_srav_r.c
Сравнение двух строк
Если вернуди 0-строки равны
             1-не равны
*/
#include "iceb_util.h"


int iceb_u_srav_r(const char *stroka1,const char *stroka2,int metka) //0-полное сравнение 1-первых символов
{

char obr1[strlen(stroka1)+1];
char obr2[strlen(stroka2)+1];

strcpy(obr1,stroka1);
strcpy(obr2,stroka2);

//Переводим в нижний регистр
iceb_u_tolower_str(obr1);
iceb_u_tolower_str(obr2);

return(iceb_u_SRAV(obr1,obr2,metka));



}
