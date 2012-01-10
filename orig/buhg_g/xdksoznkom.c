/*$Id: xdksoznkom.c,v 1.8 2011-02-21 07:35:59 sasa Exp $*/
/*13.04.2006	13.04.2006	Белых А.И.	xdksoznkom.c
Подпрограмма создания файла с для ввода комментария к новому платежному документа
*/
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
#include "dok4w.h"

extern REC rec;

void xdksoznkom(char *imaf,GtkWidget *wpredok)
{
FILE            *ff2;
char            shban[20]; /*Банковский счет*/
double		smp;
extern double   smp0,smp1;
char		MD[3][80];
char            bros[512];
int		metkas=0;

/*Создаем комментарий*/
sprintf(imaf,"pl%d.tmp",getpid());
if((ff2 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

memset(MD,0,sizeof(MD));
memset(shban,'\0',sizeof(shban));


iceb_poldan("Банковский счет",shban,"nastdok.alx",wpredok);

iceb_poldan("Сумма без НДС в платежку",bros,"nastdok.alx",wpredok);
metkas=0;
if(iceb_u_SRAV(bros,"Вкл",1) == 0)
  metkas=1;


if(shban[0] != '\0')
 {
  sprintf(MD[0],"N1");
  smp=smp0;
  if(iceb_u_SRAV(rec.mfo.ravno(),rec.mfo1.ravno(),0) != 0)
    smp=smp1;
  if(smp == 0)
    sprintf(MD[2],gettext("N2 За расчетное обслуживание"));
  else
    sprintf(MD[2],"%s %.2f",gettext("N2 За расчетное обслуживание"),smp);
  
 }
if(metkas == 1 && rec.sumd != 0.)
  sprintf(MD[1],"%s-%.2fгрн. ",gettext("Сумма без НДС"),rec.sumd-rec.nds);

if(rec.nds > 0.009   )
 {
  sprintf(bros,"%s-%.2fгрн.",gettext("НДС"),rec.nds);
  strcat(MD[1],bros);
 }

fprintf(ff2,"%s\n%s\n%s\n\n\n\
#----------------------------------------------------------------------------\n\
#%s\n",
MD[0],MD[1],MD[2],
gettext("Введите назначение платежа и другую необходимую информацию."));

fclose(ff2);
}
