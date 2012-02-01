/*$Id: iceb_salorksl.c,v 1.7 2011-02-21 07:36:07 sasa Exp $*/
/*08.11.2009	19.12.2003	Белых А.И.	iceb_salorksl.c
Вывод сальдо при расчете сальдо по контрагенту
*/
#include <stdio.h>
#include        "iceb_libbuh.h"


void iceb_salorksl(double dd, double kk,double ddn,double kkn,
double debm,double krem,
FILE *ff1,FILE *ffact)
{

double brr=dd+ddn;
double krr=kk+kkn;
fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");

if(debm != 0. || krem != 0.)
 {
  fprintf(ff1,"\
%*s:%12s",iceb_u_kolbait(37,gettext("Оборот за месяц")),gettext("Оборот за месяц"),iceb_u_prnbr(debm));

  fprintf(ff1," %12s\n",iceb_u_prnbr(krem));
  if(ffact != NULL)
   fprintf(ffact,"%*s %10.2f %10.2f| |\n",iceb_u_kolbait(31,gettext("Оборот за месяц")),
   gettext("Оборот за месяц"),debm,krem);
 }
fprintf(ff1,"%*s:%12s",iceb_u_kolbait(37,gettext("Оборот за период")),
gettext("Оборот за период"),iceb_u_prnbr(dd));

fprintf(ff1," %12s\n",iceb_u_prnbr(kk));

if(ffact != NULL)
 fprintf(ffact,"%*s %10.2f %10.2f| |\n",iceb_u_kolbait(31,gettext("Оборот за период")),
 gettext("Оборот за период"),dd,kk);

fprintf(ff1,"%*s:%12s",iceb_u_kolbait(37,gettext("Сальдо раз-нутое")),
gettext("Сальдо раз-нутое"),iceb_u_prnbr(brr));
fprintf(ff1," %12s\n",iceb_u_prnbr(brr));

if(ffact != NULL)
 fprintf(ffact,"%*s %10.2f %10.2f| |\n",iceb_u_kolbait(31,gettext("Сальдо раз-нутое")),
 gettext("Сальдо раз-нутое"),brr,krr);

if(dd+ ddn >= kk+kkn)
 {
  brr=(dd+ddn)-(kk+kkn);
  fprintf(ff1,"%*s:%12s\n",iceb_u_kolbait(37,gettext("Сальдо свернутое")),
  gettext("Сальдо свернутое"),iceb_u_prnbr(brr));

  if(ffact != NULL)
   fprintf(ffact,"%*s %10.2f %10s| |\n",iceb_u_kolbait(31,gettext("Сальдо свернутое")),
   gettext("Сальдо свернутое"),brr," ");
 }
else
 {
  brr=(kk+kkn)-(dd+ddn);
  fprintf(ff1,"%*s:%12s %12s\n",iceb_u_kolbait(37,gettext("Сальдо свернутое")),
  gettext("Сальдо свернутое")," ",iceb_u_prnbr(brr));


  if(ffact != NULL)
   fprintf(ffact,"%*s %10s %10.2f| |\n",iceb_u_kolbait(31,gettext("Сальдо свернутое")),
   gettext("Сальдо свернутое")," ",brr);

 }
}
