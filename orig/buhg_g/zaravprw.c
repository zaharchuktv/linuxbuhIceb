/* $Id: zaravprw.c,v 1.28 2011-03-15 09:28:17 sasa Exp $ */
/*12.03.2011	23.09.1998	Белых А.И.	zaravprw.c
Выполнение проводок для зароботной платы
*/
#include        "buhg_g.h"

void avtprozar(int tbn,short mp,short gp,FILE *ff_prot,GtkWidget *wpredok);
void zaravpr_oldw(int tbn,short mp,short gp,FILE *ff_prot,GtkWidget *wpredok);

extern SQL_baza bd;

void zaravprw(int tbn,short mp,short gp,FILE *ff_prot,GtkWidget *wpredok)
{
int metka_prov=0;
class iceb_u_str stroka("");
if(iceb_poldan("Автоматическое выполнение проводок с использованием таблицы",&stroka,"zarnast.alx",wpredok) == 0)
 if(iceb_u_SRAV(stroka.ravno(),"Вкл",1) == 0)
  metka_prov=1;

if(metka_prov == 1)
  zaravpr_oldw(tbn,mp,gp,ff_prot,wpredok);
else
 avtprozar(tbn,mp,gp,ff_prot,wpredok);

}