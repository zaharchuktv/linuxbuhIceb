/*$Id: l_ukrdok_pb.c,v 1.7 2011-02-21 07:35:53 sasa Exp $*/
/*18.11.2008	22.02.2008	Белых А.И.	l_ukrdok_pb.c
Проверка даты на блокировку в "Учёте командировочных расходов"
*/
#include "buhg_g.h"

int l_ukrdok_pb(short m,short g,GtkWidget *wpredok)
{
if(iceb_pbpds(m,g,wpredok) != 0)
 return(1);
 

return(0);

}

int l_ukrdok_pb(const char *datadok,GtkWidget *wpredok)
{
short d,m,g;
iceb_u_rsdat(&d,&m,&g,datadok,1);

return(l_ukrdok_pb(m,g,wpredok));


}
