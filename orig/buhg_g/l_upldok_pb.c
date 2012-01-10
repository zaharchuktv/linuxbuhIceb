/*$Id: l_upldok_pb.c,v 1.7 2011-02-21 07:35:54 sasa Exp $*/
/*18.11.2008	11.03.2008	Белых А.И.	l_upldok_pb.c
Проверка даты на блокировку в "Учёте путевых листов"
*/
#include "buhg_g.h"

int l_upldok_pb(short m,short g,GtkWidget *wpredok)
{
if(iceb_pbpds(m,g,wpredok) != 0)
 return(1);

return(0);

}

int l_upldok_pb(const char *datadok,GtkWidget *wpredok)
{
short d,m,g;
iceb_u_rsdat(&d,&m,&g,datadok,1);

return(l_upldok_pb(m,g,wpredok));


}
