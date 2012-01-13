/*$Id: admin_makebaz.c,v 1.4 2011-02-21 07:35:50 sasa Exp $*/
/*12.09.2010	12.09.2010	Белых А.И.	admin_makebaz.c
Разметка базы данных для бухгалтерской системы iceB
*/

//zs
//#include  "buhg_g.h"
#include  "../headers/buhg_g.h"
//ze

void admin_makebaz_r(const char *imabaz,const char *host,const char *parol,const char *kodirovka,const char *fromnsi,const char *fromdoc,GtkWidget *wpredok);
int admin_makebaz_m(class iceb_u_str *imabaz,class iceb_u_str *kodirovka,class iceb_u_str *fromnsi,class iceb_u_str *fromdoc,GtkWidget *wpredok);

void admin_makebaz(const char *host,const char *parol)
{
class iceb_u_str imabaz("");
class iceb_u_str kodirovka("");
class iceb_u_str fromnsi("");
class iceb_u_str fromdoc("");

if(admin_makebaz_m(&imabaz,&kodirovka,&fromnsi,&fromdoc,NULL) != 0)
 return;
 
admin_makebaz_r(imabaz.ravno(),host,parol,kodirovka.ravno(),fromnsi.ravno(),fromdoc.ravno(),NULL);


}
