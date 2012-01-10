/*$Id: zar_rsocw.c,v 1.5 2011-04-14 16:09:36 sasa Exp $*/
/*04.09.2006	09.11.2004	Белых А.И.	zar_rsocw.c
Расчет суммы на соц-страх
*/
#include        "buhg_g.h"

extern float    procsoc; /*Процент отчисления в соц-страх*/
extern float    procsoc1; /*Процент отчисления в соц-страх*/
//extern float    prog_min_tek; //Прожиточный минимум тукущий
extern double   ogrzp[2]; /*Ограничение на начисление пенсионного отчисления*/
extern double   okrg; /*Округление*/

double zar_rsocw(double soc_suma,float prog_min_tek,FILE *ff_prot)
{


double koef=0.;
double nalog=0.;

if(soc_suma < prog_min_tek)
 {
  if(procsoc != 0.)
     koef=procsoc/100.;  
  if(ff_prot != NULL)
    fprintf(ff_prot,"%.2f < %.2f(прожиточный минимум) -> %.2f%%\n",soc_suma,prog_min_tek,koef);
 }
else
 {
  if(procsoc1 != 0.)
     koef=procsoc1/100.;  
  if(ff_prot != NULL)
    fprintf(ff_prot,"%.2f >= %.2f(прожиточный минимум) -> %.2f%%\n",soc_suma,prog_min_tek,koef);
 }

if(soc_suma <= ogrzp[1])
 {
  nalog=soc_suma*koef;
  if(ff_prot != NULL)
      fprintf(ff_prot,"*%s:%.2f %.2f-%.2f%% %s %.2f %s %.2f\n",
      "Соц-страх с суммы",soc_suma,nalog,koef,
      gettext("Сумма"),soc_suma,
      gettext("Ограничение"), ogrzp[1]);
 }
else
 {
  nalog=koef*ogrzp[1];
  if(ff_prot != NULL)
       fprintf(ff_prot,"Превышение. %.2f > %.2f/%.2f\n",soc_suma,ogrzp[1],koef);
 }      

nalog=iceb_u_okrug(nalog,okrg)*(-1);

return(nalog);
}
