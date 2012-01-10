/*$Id: iceb_u_startfil.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*05.07.2002	05.07.2002	Белых А.И.	iceb_u_startfil.c
Выдача в файл начальной команды для определения формата файла
*/
#include        <stdio.h>


void   iceb_u_startfil(FILE *ff)
{

fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/

}
