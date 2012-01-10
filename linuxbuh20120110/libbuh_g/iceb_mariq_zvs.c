/*$Id: iceb_mariq_zvs.c,v 1.8 2011-02-21 07:36:07 sasa Exp $*/
/*25.12.2004	24.12.2004	Белых А.И.	iceb_mariq_zvs.c
Запись суммы в кассу
Если вернули 0-записано
             1-нет
*/
#include "iceb_libbuh.h"


int iceb_mariq_zvs(int metka,double suma,int vidnal,
struct KASSA *kasr,
GtkWidget *wpredok)
{
char naim[512];
char strsql[500];

if( metka == 1)
 sprintf(naim,gettext("Выдача из кассы клиенту."));
if( metka == 0)
 sprintf(naim,gettext("Внесение в кассу клиентом."));


suma=iceb_u_okrug(suma,0.01);

double cena=suma;
double kolih=1;

sprintf(strsql,"FSTART: ADD_CHECK\n\
%s\n\
%.2f\n\
%.2f\n\
%.6g\n\
%d\n\
FEND:\n",
  naim,suma,cena,kolih, vidnal);  

if(iceb_cmd(kasr, strsql, wpredok) != 0)
  {
   sprintf(strsql,"FREE_CHECK:\n");
   iceb_cmd(kasr, strsql, wpredok);
   return(1); 
  }  

if( metka == 0)
  sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%s:\n\
FEND:\n",
 kasr->prodavec.ravno(),  kasr->parol, 1, gettext("Внести в кассу"));  

if( metka == 1)
  sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%s:\n\
FEND:\n",
 kasr->prodavec.ravno(),  kasr->parol, 1, gettext("Получить в кассе"));  

iceb_cmd(kasr, strsql, wpredok);

sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%.2f\n\
FEND:\n",
 kasr->prodavec.ravno(),  kasr->parol, 2, suma);  

iceb_cmd(kasr, strsql, wpredok);


double isuma=suma;
double sum_nal=suma;

double sum_tara=0.;
double sum_pchek=0.;
double sum_cred=0.;
int skl=1;

sprintf(strsql,"FSTART: MARIA_PRCHECK\n\
%s\n\
%s\n\
%d\n\
%d\n\
%.2f\n\
%.2f\n\
%.2f\n\
%.2f\n\
%.2f\n\
FEND:\n",
 kasr->prodavec.ravno(),  kasr->parol, skl,  metka, isuma, sum_tara, sum_pchek, sum_cred, sum_nal);

iceb_cmd(kasr, strsql, wpredok);

return(0);

}
