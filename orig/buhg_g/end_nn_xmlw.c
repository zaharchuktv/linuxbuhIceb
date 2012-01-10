/*$Id: end_nn_xmlw.c,v 1.4 2011-08-29 07:13:43 sasa Exp $*/
/*28.07.2011	13.10.2006	Белых А.И.	end_nn_xmlw.c
Концовка файла с налоговой накладной в формате xml
*/
#include <stdio.h>
#include "buhg_g.h"

extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/

void end_nn_xmlw(int metka_nds,
double isuma_bnds,
double sumkor,
double suma_voz_tar,
double suma_nds_v,
double procent_nds,
const char *kodop,
const char *imaf_nastr,
FILE *ff_xml)  
{
if(metka_nds == 0)
 fprintf(ff_xml,"  <R01G7>%.2f</R01G7>\n",isuma_bnds);
if(metka_nds == 1)
 fprintf(ff_xml,"  <R01G8>%.2f</R01G8>\n",isuma_bnds);
if(metka_nds == 2)
 fprintf(ff_xml,"  <R01G9>%.2f</R01G9>\n",isuma_bnds);
if(metka_nds == 3)
 fprintf(ff_xml,"  <R01G10>%.2f</R01G10>\n",isuma_bnds);

fprintf(ff_xml,"  <R01G11>%.2f</R01G11>\n",isuma_bnds);
 

double suma_dok=isuma_bnds+sumkor+suma_voz_tar;

double suma_nds=0.;
if(metka_nds == 0)
 {
  if(suma_nds_v == 0.)
    suma_nds=suma_dok*procent_nds/100.;
  else
   suma_nds=suma_nds_v;
  suma_nds=iceb_u_okrug(suma_nds,okrg1);
 }
 
if(metka_nds == 0)
  fprintf(ff_xml,"  <R03G7>%.2f</R03G7>\n",suma_nds);
if(metka_nds == 1)
  fprintf(ff_xml,"  <R03G8>%.2f</R03G8>\n",suma_nds);
if(metka_nds == 2)
  fprintf(ff_xml,"  <R03G9>%.2f</R03G9>\n",suma_nds);
if(metka_nds == 3)
  fprintf(ff_xml,"  <R03G10S>%.2f</R03G10S>\n",suma_nds);

fprintf(ff_xml,"  <R03G11>%.2f</R03G11>\n",suma_nds);

double suma_s_nds=suma_dok+suma_nds;
double suma_k_oplate=suma_dok+suma_nds+suma_voz_tar;


if(metka_nds == 0)
  fprintf(ff_xml,"  <R04G7>%.2f</R04G7>\n",suma_s_nds);
if(metka_nds == 1)
  fprintf(ff_xml,"  <R04G8>%.2f</R04G8>\n",suma_s_nds);
if(metka_nds == 2)
  fprintf(ff_xml,"  <R04G9>%.2f</R04G9>\n",suma_s_nds);
if(metka_nds == 3)
  fprintf(ff_xml,"  <R04G10>%.2f</R04G10>\n",suma_s_nds);

fprintf(ff_xml,"  <R04G11>%.2f</R04G11>\n",suma_k_oplate);

class iceb_u_str fio("");
iceb_poldan("Исполнитель",&fio,"matnast.alx",NULL);
if(fio.getdlinna() <= 1)
  fio.new_plus(iceb_getfioop(NULL));

fprintf(ff_xml,"  <H10G1S>%s</H10G1S>\n",fio.ravno_filtr_xml());

if(kodop[0] != '\0' && imaf_nastr[0] != '\0')
 {
  class iceb_u_str punkt_zak("");
  char naim_nast[512];
  sprintf(naim_nast,"Пункт закона освобождения от НДС для операции %s",kodop);

  iceb_poldan(naim_nast,&punkt_zak,imaf_nastr,NULL);
  if(punkt_zak.getdlinna() > 1)
    fprintf(ff_xml,"  <R003G10S>%s</R003G10S>\n",punkt_zak.ravno_filtr_xml());
 } 


fprintf(ff_xml," </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");


}
