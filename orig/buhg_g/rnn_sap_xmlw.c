/*$Id: rnn_sap_xmlw.c,v 1.10 2011-08-29 07:13:44 sasa Exp $*/
/*06.05.2008	13.10.2006	Белых А.И.	rnn_sap_xmlw.c
Шапка документов в xml формате
*/
#include <time.h>
#include "buhg_g.h"


void rnn_sap_xmlw(short mn,short gn,short mk,const char *tip_dok,const char *podtip,int type_ver,int pnd,const char *imaf,
int period_type, //1-месяц, 2 квартал 3 полугодие 4-9 месяцев 5-год
class iceb_rnfxml_data *rek_zag_nn,
FILE *ff_xml)
{
time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

fprintf(ff_xml,"<?xml version=\"1.0\" encoding=\"%s\"?>\n",rek_zag_nn->kod_tabl);
fprintf(ff_xml,"<!--this XML file generated by iceBw %s %02d.%02d.%d %02d:%02d:%02d -->\n",
VERSION,
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
bf->tm_hour,bf->tm_min,bf->tm_sec);
fprintf(ff_xml,"<DECLAR xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"%s\">\n",imaf);
fprintf(ff_xml," <DECLARHEAD>\n");
fprintf(ff_xml,"  <TIN>%s</TIN>\n",rek_zag_nn->kod_edrpuo);
fprintf(ff_xml,"  <C_DOC>%s</C_DOC>\n",tip_dok);
fprintf(ff_xml,"  <C_DOC_SUB>%s</C_DOC_SUB>\n",podtip);
fprintf(ff_xml,"  <C_DOC_VER>%d</C_DOC_VER>\n",type_ver);
fprintf(ff_xml,"  <C_DOC_TYPE>0</C_DOC_TYPE>\n");
fprintf(ff_xml,"  <C_DOC_CNT>%d</C_DOC_CNT>\n",pnd);
fprintf(ff_xml,"  <C_REG>%s</C_REG>\n",rek_zag_nn->kod_oblasti);
fprintf(ff_xml,"  <C_RAJ>%s</C_RAJ>\n",rek_zag_nn->kod_admin_r);
fprintf(ff_xml,"  <PERIOD_MONTH>%d</PERIOD_MONTH>\n",mk);
fprintf(ff_xml,"  <PERIOD_TYPE>%d</PERIOD_TYPE>\n",period_type);
fprintf(ff_xml,"  <PERIOD_YEAR>%04d</PERIOD_YEAR>\n",gn);
fprintf(ff_xml,"  <C_DOC_STAN>1</C_DOC_STAN>\n"); /*1-Звітний 2-новий звітний 3-уточнюючий*/
fprintf(ff_xml,"  <D_FILL>%02d%02d%04d</D_FILL>\n",rek_zag_nn->dt,rek_zag_nn->mt,rek_zag_nn->gt);
fprintf(ff_xml,"  <SOFTWARE>iceBw %s</SOFTWARE>\n",VERSION);
fprintf(ff_xml," </DECLARHEAD>\n");

}
