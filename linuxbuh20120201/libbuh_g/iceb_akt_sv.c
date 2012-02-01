/*$Id: iceb_akt_sv.c,v 1.7 2011-02-21 07:36:06 sasa Exp $*/
/*05.11.2009	26.01.2007	Белых А.И.	iceb_akt_sv.c
Распечатка информации по акту сверки
*/
#include "iceb_libbuh.h"

void iceb_akt_sv_it(double deb_start,double kre_start,double deb_oborot,double kre_oborot,double deb_ob_mes,double kre_ob_mes,FILE *ff);


void iceb_akt_sv(class akt_sverki *akt_svr,FILE *ff,FILE *ff_bi,GtkWidget *wpredok)
{
iceb_zagsh(akt_svr->shet,ff,wpredok);
iceb_zagsh(akt_svr->shet,ff_bi,wpredok);

//iceb_hasv(akt_svr->shet,ff,wpredok);
//iceb_hasv(akt_svr->shet,ff_bi,wpredok);

if(akt_svr->start_saldo_deb > akt_svr->start_saldo_kre)
 {
  fprintf(ff,"%02d.%02d.%4d %-*.*s %10.2f %10s| |\n",
  akt_svr->dn,akt_svr->mn,akt_svr->gn,
  iceb_u_kolbait(20,gettext("Сальдо начальное")),iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
  akt_svr->start_saldo_deb," ");

  fprintf(ff_bi,"%02d.%02d.%4d %-*.*s %10.2f %10s| |\n",
  akt_svr->dn,akt_svr->mn,akt_svr->gn,
  iceb_u_kolbait(20,gettext("Сальдо начальное")),iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
  akt_svr->start_saldo_deb," ");
 }
else
 {
  fprintf(ff,"%02d.%02d.%4d %-*.*s %10s %10.2f| |\n",
  akt_svr->dn, akt_svr->mn, akt_svr->gn,
  iceb_u_kolbait(20,gettext("Сальдо начальное")),iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
  " ",akt_svr->start_saldo_kre);

  fprintf(ff_bi,"%02d.%02d.%4d %-*.*s %10s %10.2f| |\n",
  akt_svr->dn, akt_svr->mn, akt_svr->gn,
  iceb_u_kolbait(20,gettext("Сальдо начальное")),iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
  " ",akt_svr->start_saldo_kre);
 }
fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");
fprintf(ff_bi,"\
------------------------------------------------------ ---------------------------------------------------\n");
short d,m,g;
short mz=0,gz=0;
double deb_oborot=0.;
double kre_oborot=0.;
double deb_ob_mes=0.;
double kre_ob_mes=0.;
double deb,kre;
 
for(int ii=0; ii < akt_svr->data_nomd.kolih(); ii++)
 {
  iceb_u_rsdat(&d,&m,&g,akt_svr->data_nomd.ravno(ii),1);
  if(mz != m || gz != g)
   {
    if(mz != 0)
       iceb_akt_sv_it(akt_svr->start_saldo_deb,akt_svr->start_saldo_kre,deb_oborot,kre_oborot,deb_ob_mes,kre_ob_mes,ff);
    deb_ob_mes=kre_ob_mes=0.;    
    mz=m;
    gz=g;
   }     
  
  deb=akt_svr->suma_deb.ravno(ii);
  kre=akt_svr->suma_kre.ravno(ii);
  
  fprintf(ff,"%02d.%02d.%4d %-*.*s %10.2f %10.2f| |\n",
  d,m,g,
  iceb_u_kolbait(20,akt_svr->koment.ravno(ii)),iceb_u_kolbait(20,akt_svr->koment.ravno(ii)),akt_svr->koment.ravno(ii),
  deb,kre);

  fprintf(ff_bi,"%02d.%02d.%4d %-*.*s %10.2f %10.2f| |\n",
  d,m,g,
  iceb_u_kolbait(20,akt_svr->koment.ravno(ii)),iceb_u_kolbait(20,akt_svr->koment.ravno(ii)),akt_svr->koment.ravno(ii),
  deb,kre);

  fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");
  fprintf(ff_bi,"\
------------------------------------------------------ ---------------------------------------------------\n");
  deb_oborot+=deb;
  kre_oborot+=kre;
  deb_ob_mes+=deb;
  kre_ob_mes+=kre;
 }

iceb_akt_sv_it(akt_svr->start_saldo_deb,akt_svr->start_saldo_kre,deb_oborot,kre_oborot,deb_ob_mes,kre_ob_mes,ff);
iceb_akt_sv_it(akt_svr->start_saldo_deb,akt_svr->start_saldo_kre,deb_oborot,kre_oborot,deb_ob_mes,kre_ob_mes,ff_bi);

}
/*********************/
/*Выдача итога*/
/*********************/

void iceb_akt_sv_it(double deb_start,double kre_start,
double deb_oborot,double kre_oborot,
double deb_ob_mes,double kre_ob_mes,
FILE *ff)
{

double brr=deb_start+deb_oborot;
double krr=kre_start+kre_oborot;

if(deb_ob_mes != 0. || kre_ob_mes != 0.)
 {
  fprintf(ff,"%*s %10.2f %10.2f| |\n",
  iceb_u_kolbait(31,gettext("Оборот за месяц")),gettext("Оборот за месяц"),deb_ob_mes,kre_ob_mes);
 }

fprintf(ff,"%*s %10.2f %10.2f| |\n",
iceb_u_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),deb_oborot,kre_oborot);


fprintf(ff,"%*s %10.2f %10.2f| |\n",
iceb_u_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),brr,krr);

if(brr >= krr)
 {
  brr=brr-krr;
  fprintf(ff,"%*s %10.2f %10s| |\n",
  iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),brr," ");
 }
else
 {
  brr=krr-brr;
  fprintf(ff,"%*s %10s %10.2f| |\n",
  iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",brr);
 }
}
