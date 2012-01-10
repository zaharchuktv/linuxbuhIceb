/*$Id: f1df_xml_strw.c,v 1.6 2011-03-28 06:55:46 sasa Exp $*/
/*24.03.2011	23.04.2007	Белых А.И.	f1df_xml_strw.c
Вывод строки в xml файл формы 1ДФ
*/
#include <ctype.h>
#include <stdio.h>

void f1df_xml_strw(int *nomer_xml_str,
char *inn,
double sumadn,
double sumad,
double sumann,
double suman,
const char *priz,
const char *data11,
const char *data21,
const char *lgotach,
const char *metka_zap,
FILE *ff_xml)
{
*nomer_xml_str+=1;
if(inn[0] != '\0')
 fprintf(ff_xml,"  <T1RXXXXG02 ROWNUM=\"%d\">%s</T1RXXXXG02>\n",*nomer_xml_str,inn);
fprintf(ff_xml,"  <T1RXXXXG03A ROWNUM=\"%d\">%.2f</T1RXXXXG03A>\n",*nomer_xml_str,sumadn);
fprintf(ff_xml,"  <T1RXXXXG03 ROWNUM=\"%d\">%.2f</T1RXXXXG03>\n",*nomer_xml_str,sumad);
fprintf(ff_xml,"  <T1RXXXXG04A ROWNUM=\"%d\">%.2f</T1RXXXXG04A>\n",*nomer_xml_str,sumann);
fprintf(ff_xml,"  <T1RXXXXG04 ROWNUM=\"%d\">%.2f</T1RXXXXG04>\n",*nomer_xml_str,suman);
if(priz[0] != '\0')
 fprintf(ff_xml,"  <T1RXXXXG05 ROWNUM=\"%d\">%s</T1RXXXXG05>\n",*nomer_xml_str,priz);
if(data11[0] != '\0')
 fprintf(ff_xml,"  <T1RXXXXG06D ROWNUM=\"%d\">%s</T1RXXXXG06D>\n",*nomer_xml_str,data11);
if(data21[0] != '\0')
 fprintf(ff_xml,"  <T1RXXXXG07D ROWNUM=\"%d\">%s</T1RXXXXG07D>\n",*nomer_xml_str,data21);

if(isdigit(lgotach[0]) != 0)  /*Значит символ число*/
  fprintf(ff_xml,"  <T1RXXXXG08 ROWNUM=\"%d\">%s</T1RXXXXG08>\n",*nomer_xml_str,lgotach);
if(metka_zap[0] != '\0')
fprintf(ff_xml,"  <T1RXXXXG09 ROWNUM=\"%d\">%s</T1RXXXXG09>\n",*nomer_xml_str,metka_zap);



}
