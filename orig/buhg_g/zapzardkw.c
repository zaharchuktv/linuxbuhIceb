/*$Id: zapzardkw.c,v 1.7 2011-01-13 13:49:55 sasa Exp $*/
/*25.07.2008	31.05.2006	Белых А.И.	zapzardkw.c
Запись шапки документа для подсистемы "Зароботная плата".
*/
#include <time.h>
#include "buhg_g.h"
#include <unistd.h>


int zapzardkw(short dd,short md,short gd,
const char *nomdok,
int podr,
const char *koment,
short prn,
GtkWidget *wpredok)
{
char strsql[512];
time_t vrem;
time(&vrem);
class iceb_u_str koment1(koment);

sprintf(strsql,"insert into Zardok values (%d,'%04d-%02d-%02d','%s',%d,'%s',%d,%ld,%d)",
gd,gd,md,dd,nomdok,podr,koment1.ravno_filtr(),iceb_getuid(wpredok),vrem,prn);

return(iceb_sql_zapis(strsql,0,0,wpredok));

}

