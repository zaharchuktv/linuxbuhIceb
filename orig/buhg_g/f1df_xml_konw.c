/*$Id: f1df_xml_konw.c,v 1.10 2011-08-29 07:13:43 sasa Exp $*/
/*15.04.2009	23.04.2007	Белых А.И.	f1df_xml_konw.c
Концовка xml файла формы 1ДФ
*/

#include "buhg_g.h"

void f1df_xml_konw(double isumadn,
double isumad,
double isumann,
double isuman,
int nomer_xml_str,
int kolr,
int klls,
const char *naim_kontr,
const char *telef_kontr,
int TYP,
FILE *ff_xml,
GtkWidget *wpredok)
{
fprintf(ff_xml,"  <R01G03A>%.2f</R01G03A>\n",isumadn);
fprintf(ff_xml,"  <R01G03>%.2f</R01G03>\n",isumad);
fprintf(ff_xml,"  <R01G04A>%.2f</R01G04A>\n",isumann);
fprintf(ff_xml,"  <R01G04>%.2f</R01G04>\n",isuman);

fprintf(ff_xml,"  <R02G01I>%d</R02G01I>\n",nomer_xml_str);
fprintf(ff_xml,"  <R02G02I>%d</R02G02I>\n",kolr);
fprintf(ff_xml,"  <R02G03I>%d</R02G03I>\n",klls);

char bros[512];
char fioruk[100];
char telef[100];
  
class iceb_u_str imaf_nzar("zarnast.alx");

iceb_poldan("Табельный номер руководителя",bros,imaf_nzar.ravno(),wpredok);
char inn[20];
char strsql[512];
SQL_str row1;
class SQLCURSOR curr;
memset(fioruk,'\0',sizeof(fioruk));
memset(inn,'\0',sizeof(inn));
memset(telef,'\0',sizeof(telef));
if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
   {
    sprintf(strsql,"Не знайдено табельного номера %s керівника !",bros);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    strncpy(fioruk,row1[0],sizeof(fioruk)-1);
    strncpy(inn,row1[1],sizeof(inn)-1);
    strncpy(telef,row1[2],sizeof(telef)-1);
   }
 } 
if(inn[0] != '\0')
  fprintf(ff_xml,"  <HKBOS>%s</HKBOS>\n",inn);
if(fioruk[0] != '\0')
  fprintf(ff_xml,"  <HBOS>%s</HBOS>\n",iceb_u_filtr_xml(fioruk));
if(telef[0] != '\0')
  fprintf(ff_xml,"  <HTELBOS>%s</HTELBOS>\n",iceb_u_filtr_xml(telef));

bros[0]='\0';
iceb_poldan("Табельный номер бухгалтера",bros,imaf_nzar.ravno(),wpredok);
char fio[512];
memset(fio,'\0',sizeof(fio));
memset(inn,'\0',sizeof(inn));
memset(telef,'\0',sizeof(telef));

if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
   {
    sprintf(strsql,"Не знайдено табельного номера %s головного бухгалтера!",bros);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    strncpy(fio,row1[0],sizeof(fio)-1);
    strncpy(inn,row1[1],sizeof(inn)-1);
    strncpy(telef,row1[2],sizeof(telef)-1);
   }
 }
if(inn[0] != '\0')
  fprintf(ff_xml,"  <HKBUH>%s</HKBUH>\n",inn);
if(fio[0] != '\0')
  fprintf(ff_xml,"  <HBUH>%s</HBUH>\n",iceb_u_filtr_xml(fio));
if(telef[0] != '\0')
  fprintf(ff_xml,"  <HTELBUH>%s</HTELBUH>\n",iceb_u_filtr_xml(telef));

short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

fprintf(ff_xml,"  <HFILL>%02d%02d%04d</HFILL>\n",dt,mt,gt);


if(TYP == 1)
 {
  if(naim_kontr[0] != '\0')
    fprintf(ff_xml,"  <HFO>%s<HFO>\n",iceb_u_filtr_xml(naim_kontr));
  if(telef_kontr[0] != '\0')
    fprintf(ff_xml,"  <HTELFO>%s<HTELFO>\n",iceb_u_filtr_xml(telef_kontr));
 }

fprintf(ff_xml," </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");
fclose(ff_xml);

}
