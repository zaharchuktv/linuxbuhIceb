/*$Id: f1df_xml_zagw.c,v 1.12 2011-08-29 07:13:43 sasa Exp $*/
/*24.03.2011	23.04.2007	Белых А.И.	f1df_xml_zagw.c
Заголовок файла xml формы 1ДФ 
*/
#include "buhg_g.h"


void f1df_xml_zagw(int kvrt,
int god,
const char *kod,
int TYP,
int metka_oth,
int kolih_v_htate,
int kolih_po_sovm,
const char *naim_kontr,
char *imaf_xml,
FILE **ff_xml,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str imaf_nf1df("zarsdf1df.alx");

char naim_ni[512];
char kodni[56];
memset(kodni,'\0',sizeof(kodni));
iceb_poldan("Код налоговой инспекции",kodni,imaf_nf1df.ravno(),wpredok);

memset(naim_ni,'\0',sizeof(naim_ni));
iceb_poldan("Название налоговой инспекции",naim_ni,imaf_nf1df.ravno(),wpredok);

class iceb_u_str adres("");
sprintf(strsql,"select adres from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 adres.new_plus(row[0]);


short mn=0;
short mk=0;
short dk=31;
int period_type=0;
if(kvrt == 1)
 {
  mn=1;
  mk=3;
 }
if(kvrt == 2)
 {
  mn=4;
  mk=6;
 }
if(kvrt == 3)
 {
  mn=7;
  mk=9;
 }
if(kvrt == 4)
 {
  mn=10;
  mk=12;
 }
class iceb_rnfxml_data rek_fil_xml;
iceb_rnfxml(&rek_fil_xml,wpredok); /*читаем настройки для xml файла*/
if(iceb_openxml(1,mn,god,dk,mk,god,imaf_xml,"J05","001","03",1,&period_type,&rek_fil_xml,ff_xml,wpredok) != 0)
 return;

rnn_sap_xmlw(mn,god,mk,"J05","001",3,1,"J0500103.XSD",period_type,&rek_fil_xml,*ff_xml);

fprintf(*ff_xml," <DECLARBODY>\n");
fprintf(*ff_xml,"  <HTIN>%s</HTIN>\n",kod);
fprintf(*ff_xml,"  <HPAGES>1</HPAGES>\n");
/*******
if(TYP == 0)
 fprintf(*ff_xml,"  <HJ>1</HJ>\n");
else
 fprintf(*ff_xml,"  <HF>1</HF>\n");
************/
if(TYP == 1)
 fprintf(*ff_xml,"  <HF>1</HF>\n");
else
 fprintf(*ff_xml,"  <HF>1</HF>\n");

if(metka_oth == 0)
 fprintf(*ff_xml,"  <HZ>1</HZ>\n");
else
 fprintf(*ff_xml,"  <HZ></HZ>\n");

if(metka_oth == 1)
 fprintf(*ff_xml,"  <HZN>1</HZN>\n");
else
 fprintf(*ff_xml,"  <HZN></HZN>\n");

if(metka_oth == 2)
 fprintf(*ff_xml,"  <HZU>1</HZU>\n");
else
 fprintf(*ff_xml,"  <HZU></HZU>\n");

fprintf(*ff_xml,"  <HNAME>%s</HNAME>\n",iceb_u_filtr_xml(naim_kontr));
fprintf(*ff_xml,"  <HLOC>%s</HLOC>\n",iceb_u_filtr_xml(adres.ravno()));
fprintf(*ff_xml,"  <HTINSTI>%s</HTINSTI>\n",kodni);
fprintf(*ff_xml,"  <HSTI>%s</HSTI>\n",iceb_u_filtr_xml(naim_ni));
//fprintf(*ff_xml,"  <HZP>%d</HZP>\n",kvrt);
fprintf(*ff_xml,"  <HZKV>%d</HZKV>\n",kvrt);
fprintf(*ff_xml,"  <HZY>%d</HZY>\n",god);
fprintf(*ff_xml,"  <R00G01I>%d</R00G01I>\n",kolih_v_htate);
fprintf(*ff_xml,"  <R00G02I>%d</R00G02I>\n",kolih_po_sovm);
}
