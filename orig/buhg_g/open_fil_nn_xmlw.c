/*$Id: open_fil_nn_xmlw.c,v 1.4 2011-08-29 07:13:43 sasa Exp $*/
/*23.07.2011	13.10.2006	Белых А.И.	open_fil_nn_xmlw.c
Открытие файла для налогового документа в фармате xml
*/
#include <stdlib.h>
#include <errno.h>
#include "buhg_g.h"

extern class xml_rek_nn_data rek_zag_nn;
extern iceb_u_str kat_for_nal_nak; //Каталог для налоговых накладных

int open_fil_nn_xmlw(char *imaf_xml,int pnd,int tipz,
short mn,short gn,
char *imaf,
class iceb_rnfxml_data *rek_zag_nn,
FILE **ff_xml,
GtkWidget *wpredok)
{

sprintf(imaf_xml,"%s/%.4s%010d%3.3s%3.3s%2.2s100%05d%02d%04d.xml",
kat_for_nal_nak.ravno(),
rek_zag_nn->kod_dpi,
atoi(rek_zag_nn->kod_edrpuo),
"J12",
"010",
"03",
pnd,
mn,gn);

if((*ff_xml = fopen(imaf_xml,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_xml,"",errno,wpredok);
  return(1);
 }

sprintf(imaf,"J1201003.xsd");

return(0);

}
