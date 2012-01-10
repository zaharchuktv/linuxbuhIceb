/*$Id: itogizw.c,v 1.6 2011-02-21 07:35:52 sasa Exp $*/
/*06.09.2006	06.09.2006	Белых А.И.	itogizw.c
Вывод итогов расчета
*/

#include "buhg_g.h"


void itogizw(double inah, //Итого начислено
double iu, //Итого удержано
double ius, //Итого удержано без учета сальдо
double sal,  //Сальдо на начало месяца
double inahb, //Итого начислено бюджет
double iub, //Итого удержано бюджет
double iusb, //Итого удержано без учета сальдо бюджет
double salb,  //Сальдо на начало месяца бюджет
FILE *kaw,FILE *kawk1,FILE *kawk2,FILE *ffkor2s)
{
double          prom;
char            st[512],st1[512],st0[512],st2[512];
char		bros[512];
char		str[1024];
double		bbr,bbr1;
double          bbprom=0.;


sprintf(str,"\
%-*s- %14s",iceb_u_kolbait(25,gettext("Сальдо на начало месяца")),gettext("Сальдо на начало месяца"),iceb_u_prnbr(sal));

if(inahb != 0. || iub != 0. || iusb != 0. || salb != 0.)
 {
  sprintf(bros," %8.2f %8.2f",sal-salb,salb);
  strcat(str,bros);

 }

fprintf(kaw,"\n%s\n",str);

strfilw(str,str,1,ffkor2s);

if(kawk1 != NULL)
  fprintf(kawk1,"\n%s\n",str);

if(kawk2 != NULL)
 fprintf(kawk2,"\n\n");

sprintf(bros,"\
%-*s- %14s",
iceb_u_kolbait(25,gettext("Итого начислено")),gettext("Итого начислено"),
iceb_u_prnbr(inah));

sprintf(str,"\
%-*s- %14s",
iceb_u_kolbait(25,gettext("Итого начислено")),gettext("Итого начислено"),
iceb_u_prnbr(inah));

if(inahb != 0. || iub != 0. || iusb != 0. || salb != 0.)
 {
  sprintf(st1," %8.2f %8.2f",inah-inahb,inahb);
  strcat(bros,st1);
  strcat(str,st1);
 }

strfilw(bros,str,0,kaw);
strfilw(bros,str,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",bros);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

prom=iu*(-1);

sprintf(bros,"\
%-*s- %14s",
iceb_u_kolbait(25,gettext("Итого удержано")),gettext("Итого удержано"),
iceb_u_prnbr(prom));

prom=ius*(-1);
sprintf(str,"\
%-*s- %14s",
iceb_u_kolbait(25,gettext("Итого удержано")),gettext("Итого удержано"),
iceb_u_prnbr(prom));

if(inahb != 0. || iub != 0. || iusb != 0. || salb != 0.)
 {
  sprintf(st1," %8.2f %8.2f",(iu-iub)*-1,iub*-1);
  strcat(bros,st1);
  sprintf(st1," %8.2f %8.2f",(ius-iusb)*-1,iusb*-1);
  strcat(str,st1);
 }

strfilw(bros,str,0,kaw);
strfilw(bros,bros,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",bros);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

bbr=inah+iu+sal;
if(bbr < 0.)
 sprintf(bros,"\
%-*s- %14s",
 iceb_u_kolbait(25,gettext("Сальдо конечное")),gettext("Сальдо конечное"),iceb_u_prnbr(bbr));
else
 sprintf(bros,"\
%-*s- %14s",
  iceb_u_kolbait(25,gettext("К выдаче")),gettext("К выдаче"),iceb_u_prnbr(bbr));
if(inahb != 0. || iub != 0. || iusb != 0. || salb != 0.)
 {
  bbprom=inahb+iub+salb;
  sprintf(st1," %8.2f %8.2f",bbr-bbprom,bbprom);
  strcat(bros,st1);
 }


bbr1=inah+ius;

if(bbr1 < 0.)
 sprintf(str,"\
%-*s- %14s",
 iceb_u_kolbait(25,gettext("Сальдо конечное")),gettext("Сальдо конечное"),iceb_u_prnbr(bbr1));
else
  sprintf(str,"\
%-*s- %14s",
  iceb_u_kolbait(25,gettext("К выдаче")),gettext("К выдаче"),iceb_u_prnbr(bbr1));

if(inahb != 0. || iub != 0. || iusb != 0. || salb != 0.)
 {
  bbprom=inahb+iusb;
  sprintf(st1," %8.2f %8.2f",bbr1-bbprom,bbprom);
  strcat(str,st1);
 }

strfilw(bros,str,0,kaw);
strfilw(bros,bros,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",bros);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

if(inahb != 0. || iub != 0. || iusb != 0. || salb != 0.)
 {
  sprintf(str,"%42s %*.*s %*.*s",
  " ",
  iceb_u_kolbait(8,gettext("Хозрасчет")),iceb_u_kolbait(8,gettext("Хозрасчет")),gettext("Хозрасчет"),
  iceb_u_kolbait(8,gettext("Бюджет")),iceb_u_kolbait(8,gettext("Бюджет")),gettext("Бюджет"));
  
  strfilw(str,str,0,kaw);
  strfilw(str,str,0,ffkor2s);
  
  if(kawk1 != NULL)
    fprintf(kawk1,"%s\n",str);
  if(kawk2 != NULL)
    fprintf(kawk2,"%s\n",str);
 }

/*Преобразуем в символьную строку и выводим*/
memset(st,'\0',sizeof(st));
iceb_u_preobr(bbr,st,0);

memset(st0,'\0',sizeof(st0));
iceb_u_preobr(bbr1,st0,0);
/**********
memset(st1,'\0',sizeof(st1));
iceb_u_slstr(st,62,st1);

memset(st2,'\0',sizeof(st2));
iceb_u_slstr(st0,62,st2);
*************/
sprintf(st1,"%-*.*s",iceb_u_kolbait(62,st),iceb_u_kolbait(62,st),st);
sprintf(st2,"%-*.*s",iceb_u_kolbait(62,st0),iceb_u_kolbait(62,st0),st0);

strfilw(st1,st2,1,kaw);
strfilw(st1,st1,1,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"\n%s\n",str);
if(kawk2 != NULL)
 fprintf(kawk2,"\n%s\n",st0);
if(iceb_u_strlen(st)> 62)
 {
  sprintf(st1,"%s",iceb_u_adrsimv(62,st));
  sprintf(st2,"%s",iceb_u_adrsimv(62,st0));
  strfilw(st1,st2,0,kaw);
  strfilw(st1,st2,0,ffkor2s);
  if(kawk1 != NULL)
   fprintf(kawk1,"%s\n",st1);
  if(kawk2 != NULL)
   fprintf(kawk2,"%s\n",st2);
 }
memset(st,'\0',sizeof(st));

sprintf(str,"\
------------------------------------------------------------");
strfilw(str,str,0,kaw);
strfilw(str,str,0,ffkor2s);
if(kawk1 != NULL)
 fprintf(kawk1,"%s\n",str);
if(kawk2 != NULL)
 fprintf(kawk2,"%s\n",str);

}

