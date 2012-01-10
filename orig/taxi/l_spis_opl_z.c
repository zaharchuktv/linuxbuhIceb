/*$Id: l_spis_opl_z.c,v 1.6 2011-01-13 13:50:09 sasa Exp $*/
/*17.04.2006	27.01.2006	Белых А.И.	l_spis_opl_z.c
Запуск режима просмотра списка оплат
*/
#include "i_rest.h"
#include "l_spis_opl.h"

int l_spis_opl_p(class l_spis_opl_rek *datap,GtkWidget *wpredok);
int l_spis_opl(class l_spis_opl_rek *datap,int,GtkWidget *wpredok);

iceb_u_str l_spis_opl_rek::datan;
iceb_u_str l_spis_opl_rek::datak;
iceb_u_str l_spis_opl_rek::vremn;
iceb_u_str l_spis_opl_rek::vremk;
iceb_u_str l_spis_opl_rek::kassa;
iceb_u_str l_spis_opl_rek::podr;
iceb_u_str l_spis_opl_rek::koment;
iceb_u_str l_spis_opl_rek::nomdok;
iceb_u_str l_spis_opl_rek::suma;
iceb_u_str l_spis_opl_rek::datad;
iceb_u_str l_spis_opl_rek::kodkl;
short l_spis_opl_rek::metka_nal;
short l_spis_opl_rek::metka_bnal;
short l_spis_opl_rek::metka_spis;

void l_spis_opl_z(int metka_ud_zap,//0-просмотр без возможности удаления 1-с возможность удалять записи
GtkWidget *wpredok)
{
class l_spis_opl_rek datap;

if(datap.datan.getdlinna() == 0)
  datap.clear_data();
  
for(;;)
 {
  if(l_spis_opl_p(&datap,wpredok) != 0)
   return;

  l_spis_opl(&datap,metka_ud_zap,wpredok);
 }
}
