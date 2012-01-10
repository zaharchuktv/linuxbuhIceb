/*$Id: priv_k_kartw.c,v 1.4 2011-01-13 13:49:52 sasa Exp $*/
/*21.03.2005	19.02.2005	Белых А.И.	priv_k_kartw.c
Подпрограмма записи в документ материального учёта с привязкой к карточке
*/
#include <math.h>
#include "buhg_g.h"



void priv_k_kartw(short dk,short mk,short gk,
const char *nomd,
int sklad,
int kodm,
double kolih,
double cena,
const char *shetu,
const char *ei,
double ndss,
int mnds,
int voztar,
const char *nomzak,
int metka_priv,
const char *shet_sp, //счёт списания
GtkWidget *wpredok)
{
int tipz=2;

iceb_u_int nomkars;
iceb_u_double kolpriv;
int kolkar;  
if((kolkar=findkarw(dk,mk,gk,sklad,kodm,kolih,&nomkars,&kolpriv,metka_priv,cena,shetu,wpredok)) <= 0)
 {
  zapvdokw(dk,mk,gk,sklad,0,kodm,nomd,kolih,cena,ei,ndss,mnds,voztar,tipz,0,shet_sp,nomzak,wpredok);
 }
else
 {
  double kolihpi=0.;
  for(int sht=0; sht < kolkar; sht++)
   {
    int nomkar=nomkars.ravno(sht);
    double kolihp=kolpriv.ravno(sht);
    kolihpi+=kolihp;
    zapvdokw(dk,mk,gk,sklad,nomkar,kodm,nomd,kolihp,cena,ei,ndss,mnds,voztar,tipz,0,shet_sp,nomzak,wpredok);
   }
  if(fabs(kolih-kolihpi) > 0.000001)
    zapvdokw(dk,mk,gk,sklad,0,kodm,nomd,kolih-kolihpi,cena,ei,ndss,mnds,voztar,tipz,0,shet_sp,nomzak,wpredok);
 }

}
