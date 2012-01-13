/*$Id: in_d12_itw.c,v 1.6 2011-01-13 13:49:50 sasa Exp $*/
/*11.11.2009	23.10.2007	Белых А.И.	in_d12_itw.c
Распечатка итога по счетам
*/

//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

void in_d12_itw(class iceb_u_spisok *sp_shet,class iceb_u_double *kol_shet,class iceb_u_double *sum_shet,FILE *ffipsh,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

char naim_shet[512];
double i_psh_kol=0.;
double i_psh_sum=0.;
for(int ii=0; ii < sp_shet->kolih(); ii++)
 {
  memset(naim_shet,'\0',sizeof(naim_shet));
  strcpy(naim_shet,sp_shet->ravno(ii));
  /*узнаём наименование счёта*/
  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shet->ravno(ii));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    strcat(naim_shet," ");
    strcat(naim_shet,row[0]);
   }
  

  fprintf(ffipsh,"%5d %-*.*s|%10.10g|    |%10.10g|          |    |          |          |    |          |\n",
  ii+1,
  iceb_u_kolbait(30,naim_shet),iceb_u_kolbait(30,naim_shet),naim_shet,
  kol_shet->ravno(ii),sum_shet->ravno(ii));

  if(iceb_u_strlen(naim_shet) > 30)
   fprintf(ffipsh,"%5s %s\n","",iceb_u_adrsimv(30,naim_shet));

  fprintf(ffipsh,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
   
  i_psh_kol+=kol_shet->ravno(ii);
  i_psh_sum+=sum_shet->ravno(ii);
 }

fprintf(ffipsh,"\
----------------------------------------------------------------------------------------------------------------------\n");
fprintf(ffipsh,"%*s %10.10g      %10.10g\n",
iceb_u_kolbait(36,"Разом"),"Разом",
i_psh_kol,i_psh_sum);


}
