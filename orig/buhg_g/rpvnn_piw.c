/*$Id: rpvnn_piw.c,v 1.7 2011-02-21 07:35:57 sasa Exp $*/
/*17.02.2011	30.08.2005	Белых А.И.	rpvnn_piw.c
Строка выдачи итога в реестре полученных налоговых накладных
*/
#include "buhg_g.h"

void rpvnn_piw(short dn,short mn,short gn,char *vsego,double *itogo,double *itogo_u,int *kol_strok,FILE *ff)
{
if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
 fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
else
 fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------\n");

if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
  fprintf(ff,"|%*s|%10.2f|",iceb_u_kolbait(79,vsego),vsego,itogo[0]);
else
  fprintf(ff,"|%*s|%10.2f|",iceb_u_kolbait(62,vsego),vsego,itogo[0]);

if(itogo[1] != 0.)
 fprintf(ff,"%9.2f|",itogo[1]);
else
 fprintf(ff,"%9s|","");

if(itogo[2] != 0.)
 fprintf(ff,"%8.2f|",itogo[2]);
else
 fprintf(ff,"%8s|","");


if(itogo[3] != 0.)
 fprintf(ff,"%9.2f|",itogo[3]);
else
 fprintf(ff,"%9s|","");

if(itogo[4] != 0.)
 fprintf(ff,"%8.2f|",itogo[4]);
else
 fprintf(ff,"%8s|","");


if(itogo[5] != 0.)
 fprintf(ff,"%9.2f|",itogo[5]);
else
 fprintf(ff,"%9s|","");

if(itogo[6] != 0.)
 fprintf(ff,"%8.2f|",itogo[6]);
else
 fprintf(ff,"%8s|","");


if(itogo[7] != 0.)
 fprintf(ff,"%9.2f|",itogo[7]);
else
 fprintf(ff,"%9s|","");

if(itogo[8] != 0.)
 fprintf(ff,"%8.2f|",itogo[8]);
else
 fprintf(ff,"%8s|","");

fprintf(ff,"\n");

if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
 fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
else
 fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------\n");

if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
 {
  const char *repl={"З них включено до уточнюючих розрахунків за звітний період"};
  fprintf(ff,"|%*s|%10.2f|",iceb_u_kolbait(79,repl),repl,itogo_u[0]);

  if(itogo_u[1] != 0.)
   fprintf(ff,"%9.2f|",itogo_u[1]);
  else
   fprintf(ff,"%9s|","");

  if(itogo_u[2] != 0.)
   fprintf(ff,"%8.2f|",itogo_u[2]);
  else
   fprintf(ff,"%8s|","");


  if(itogo_u[3] != 0.)
   fprintf(ff,"%9.2f|",itogo_u[3]);
  else
   fprintf(ff,"%9s|","");

  if(itogo_u[4] != 0.)
   fprintf(ff,"%8.2f|",itogo_u[4]);
  else
   fprintf(ff,"%8s|","");


  if(itogo_u[5] != 0.)
   fprintf(ff,"%9.2f|",itogo_u[5]);
  else
   fprintf(ff,"%9s|","");

  if(itogo_u[6] != 0.)
   fprintf(ff,"%8.2f|",itogo_u[6]);
  else
   fprintf(ff,"%8s|","");


  if(itogo_u[7] != 0.)
   fprintf(ff,"%9.2f|",itogo_u[7]);
  else
   fprintf(ff,"%9s|","");

  if(itogo_u[8] != 0.)
   fprintf(ff,"%8.2f|",itogo_u[8]);
  else
   fprintf(ff,"%8s|","");

  fprintf(ff,"\n");

   fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
 }
}
