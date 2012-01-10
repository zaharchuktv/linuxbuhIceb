/* $Id: podoh1w.c,v 1.4 2011-02-21 07:35:55 sasa Exp $ */
/*22.12.2010    01.12.1992      Белых А.И.      podoh1w.c
Подпрограмма расчета подоходного налога 
отчисления в пенсионный фонд и отчисления на безработицу
с общей суммы
Возвращаем сумму налога
*/
#include        <math.h>
#include        <pwd.h>
#include        <errno.h>
#include        "buhg_g.h"

extern short	metkarnb; //0-Расчет налогов для бюджета пропорциональный 1-последовательный
extern char	*shetb; /*Бюджетные счета*/

void  podoh1w(int tn, //Табельный номер*/
short mp,short gp,
int podr,
const char *uder_only,
FILE *ff_prot,
GtkWidget *wpredok)
{

if(metkarnb == 1 && shetb != NULL) //Последовательный расчет бюджет/хозрасчет
 if(ff_prot != NULL)
    fprintf(ff_prot,"Последовательный расчет\n");

if(iceb_u_sravmydat(1,mp,gp,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
 {
  //расчёт отчисления в фонд безработицы
  double		fbezrab=0.,fbezrabb=0.;
  fbezrw(tn,mp,gp,podr,&fbezrab,&fbezrabb,0,0,0.,uder_only,ff_prot,wpredok);

  //расчёт пенсионного отчисления
  double sumapo=0; //Общая сумма пенсионного отчисления только с зарплаты 
  double sumapob=0.; //Сумма пенсинного отчисления с бюджетных начислений только с зарплаты
  ras_pensw(tn,mp,gp,podr,&sumapo,&sumapob,0,0,0.,uder_only,ff_prot,wpredok);

  //расчёт отчисления на соц-страх
  double sumaos=0.; //Сумма отчисления на соц-страх только с зарплаты
  double sumaosb=0.; //Сумма отчисления на соц-страх бюджет только с зарплаты
  ras_socw(tn,mp,gp,podr,&sumaos,&sumaosb,0,0,0.,uder_only,ff_prot,wpredok);

  //расчёт подоходного налога
  ras_podohw(tn,mp,gp,podr,fbezrab,fbezrabb,sumapo,sumapob,sumaos,sumaosb,0,0,0.,uder_only,ff_prot,wpredok);
 }
else
 {
  double suma_esv=0.;
  double suma_esvb=0.;
  zaresvw(tn,mp,gp,podr,&suma_esv,&suma_esvb,0,0.,uder_only,ff_prot,wpredok);

  //расчёт подоходного налога
  ras_podoh1w(tn,mp,gp,podr,suma_esv,suma_esvb,0,0,0.,uder_only,ff_prot,wpredok);
 }

return;
}
