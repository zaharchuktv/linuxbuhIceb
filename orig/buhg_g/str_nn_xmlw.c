/*$Id: str_nn_xmlw.c,v 1.5 2011-08-29 07:13:44 sasa Exp $*/
/*28.07.2011	13.10.2006	Белых А.И.	str_nn_xmlw.c
Выгрузка строки налоговой накладной в формате xml.
*/
#include "buhg_g.h"

extern class xml_rek_nn_data rek_zag_nn;

void str_nn_xmlw(int nomer,int metka_nds,const char *datop,double kolih,double cena,double suma,const char *ei,const char *naim_mat,FILE *ff_xml)
{

if(nomer == 1)
 if(datop[0] != '\0')
  fprintf(ff_xml,"  <RXXXXG2D ROWNUM=\"%d\">%s</RXXXXG2D>\n",nomer,datop);


fprintf(ff_xml,"  <RXXXXG3S ROWNUM=\"%d\">%s</RXXXXG3S>\n",nomer,iceb_u_filtr_xml(naim_mat));
fprintf(ff_xml,"  <RXXXXG4S ROWNUM=\"%d\">%s</RXXXXG4S>\n",nomer,iceb_u_filtr_xml(ei));
fprintf(ff_xml,"  <RXXXXG5 ROWNUM=\"%d\">%.10g</RXXXXG5>\n",nomer,kolih);
fprintf(ff_xml,"  <RXXXXG6 ROWNUM=\"%d\">%s</RXXXXG6>\n",nomer,iceb_prcn(cena));

if(metka_nds == 0)
  fprintf(ff_xml,"  <RXXXXG7 ROWNUM=\"%d\">%.2f</RXXXXG7>\n",nomer,suma);
if(metka_nds == 1)
  fprintf(ff_xml,"  <RXXXXG8 ROWNUM=\"%d\">%.2f</RXXXXG8>\n",nomer,suma);
if(metka_nds == 2)
  fprintf(ff_xml,"  <RXXXXG9 ROWNUM=\"%d\">%.2f</RXXXXG9>\n",nomer,suma);
if(metka_nds == 3)
  fprintf(ff_xml,"  <RXXXXG10 ROWNUM=\"%d\">%.2f</RXXXXG10>\n",nomer,suma);


}
/***************************************************************/
void str_nn_xmlw(int nomer,int metka_nds,const char *datop,char *kolih,double cena,double suma,const char *ei,const char *naim_mat,FILE *ff_xml)
{

if(nomer == 1)
 if(datop[0] != '\0')
  fprintf(ff_xml,"  <RXXXXG2D ROWNUM=\"%d\">%s</RXXXXG2D>\n",nomer,datop);


fprintf(ff_xml,"  <RXXXXG3S ROWNUM=\"%d\">%s</RXXXXG3S>\n",nomer,iceb_u_filtr_xml(naim_mat));
fprintf(ff_xml,"  <RXXXXG4S ROWNUM=\"%d\">%s</RXXXXG4S>\n",nomer,iceb_u_filtr_xml(ei));
fprintf(ff_xml,"  <RXXXXG5S ROWNUM=\"%d\">%s</RXXXXG5S>\n",nomer,kolih);
fprintf(ff_xml,"  <RXXXXG6 ROWNUM=\"%d\">%s</RXXXXG6>\n",nomer,iceb_prcn(cena));

if(metka_nds == 0)
  fprintf(ff_xml,"  <RXXXXG7 ROWNUM=\"%d\">%.2f</RXXXXG7>\n",nomer,suma);
if(metka_nds == 1)
  fprintf(ff_xml,"  <RXXXXG8 ROWNUM=\"%d\">%.2f</RXXXXG8>\n",nomer,suma);
if(metka_nds == 2)
  fprintf(ff_xml,"  <RXXXXG9 ROWNUM=\"%d\">%.2f</RXXXXG9>\n",nomer,suma);
if(metka_nds == 3)
  fprintf(ff_xml,"  <RXXXXG10 ROWNUM=\"%d\">%.2f</RXXXXG10>\n",nomer,suma);


}
