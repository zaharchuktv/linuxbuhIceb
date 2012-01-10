/* $Id: amort23w.c,v 1.10 2011-02-21 07:35:51 sasa Exp $ */
/*10.12.2007    22.08.1997      Белых А.И.      amort23w.c
Расчет амортизации для группы 2 и 3
*/
#include <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <pwd.h>
#include        "buhg_g.h"
#include <unistd.h>

void  amort23b(int in,short gr,int podd,const char *hzt,double bs,double iz,double bs1,double iz1,double kof,double koff,GtkWidget*);


void amort23w(int in,int podd,
short kvrt, //Квартал
short gr,const char *hzt,FILE *ff_prot,
GtkWidget *wpredok)
{
char            bros[512];
double          bs=0.,iz=0.,bs1=0.,iz1=0.,iz11=0.;
double          koff=1,kof=1;
char            grp[10];
double		amort[4];
short		mr,dr1,mr1;
short		gr1;
short		i,i1;
SQL_str         row;
SQLCURSOR curr;
char		strsql[512];
time_t		vrem;
double          sbs=0.,siz=0.;
class iceb_u_str imaf("uosnast.alx");
//printw("%s\n",__FUNCTION__);
if(ff_prot != NULL)
 fprintf(ff_prot,"Расчёт амортизации для налогового учёта в целом по группе.\n%d %d %d %d %s\n",in,podd,kvrt,gr,hzt);
//printw("amort23-%d %d %d %d %s\n",in,podd,kvrt,gr,hzt);
//OSTANOV();
mr=0;
if(kvrt == 1)
  mr=1;
if(kvrt == 2)
  mr=4;
if(kvrt == 3)
  mr=7;
if(kvrt == 4)
  mr=10;

for(i=0; i < 4 ; i++)
 amort[i]=0.;

if(in < 0)
 {
  koff=1;
  char stroka[100];
  sprintf(stroka,"Поправочный коэффициент для группы %d",in*-1);
  if(iceb_poldan(stroka,bros,imaf.ravno(),wpredok) == 0)
   koff=iceb_u_atof(bros);
  
  sbs=0.;
  sprintf(stroka,"Стартовая балансовая стоимость для группы %d",in*-1);
  if(iceb_poldan(stroka,bros,imaf.ravno(),wpredok) == 0)
    sbs=atof(bros);
  siz=0.;
  sprintf(stroka,"Стартовый износ для группы %d",in*-1);
  if(iceb_poldan(stroka,bros,imaf.ravno(),wpredok) == 0)
    siz=atof(bros);
 }

dr1=1;
mr1=mr;
gr1=gr;
if(kvrt == 0) //Для бюджетных организаций
 {
  //Берем балансувую стоимость на конец года
  mr1=1;
  gr1++; 
 }

bs=sbs;
iz=siz;

if(in < 0)
  sprintf(grp,"%d",in*-1);

bsiz23w(grp,dr1,mr1,gr1,&bs1,&iz1,&iz11,ff_prot,wpredok);
iz1+=iz11;

/*Читаем коэффициент*/
kof=0.;
sprintf(strsql,"select kof from Uosgrup where kod='%s'",grp);

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
   sprintf(strsql,"%s %s !",gettext("Не найдено код группы"),grp);
   iceb_menu_soob(strsql,wpredok);
   return;
 }
else
 kof=atof(row[0]);

if(kvrt == 0)
 {
  amort23b(in,gr,podd,hzt,bs,iz,bs1,iz1,kof,koff,wpredok);
  return;
 }

sprintf(strsql,"delete from Uosamor where innom='%d' and god=%d and \
mes >=%d and mes <= %d",in,gr,mr,mr+2);

iceb_sql_zapis(strsql,1,0,wpredok);

/*
printw("\nbs=%.2f iz=%.2f bs1=%.2f iz1=%.2f us1.kof=%.2f koff=%.2f\n",
bs,iz,bs1,iz1,us1.kof,koff);
*/

class iceb_u_str menustr;

sprintf(strsql,"%s:%.2f %s:%.2f (%.2f/%.2f)",
gettext("Б.с."),
bs,
gettext("Износ"),
iz,bs1,iz1);

menustr.plus(strsql);

amort[0]=(bs+bs1-iz-iz1)*kof/100/12.*3*koff;

sprintf(strsql,"(%.2f+%.2f-%.2f-%.2f)*%.2f/100/12*3*%.2f=%.2f",
bs,bs1,iz,iz1,kof,koff,amort[0]);

if(amort[0] < 0.)
 amort[0]=0.;
 
menustr.ps_plus(strsql);

amort[0]=iceb_u_okrug(amort[0],0.01); /*Квартальная норма*/
sprintf(strsql,"%s: %.2f",
gettext("Квартальная норма"),
amort[0]);

menustr.ps_plus(strsql);

amort[1]=amort[0]/3;
amort[1]=iceb_u_okrug(amort[1],0.01);
sprintf(strsql,"%s: %.2f",
gettext("Месячная норма"),
amort[1]);

menustr.ps_plus(strsql);

amort[2]=amort[1];
amort[3]=amort[0]-amort[1]-amort[2];

sprintf(strsql,"%s: %.2f",
gettext("Амортизация последнего месяца квартала"),
amort[3]);

menustr.ps_plus(strsql);
iceb_menu_soob(&menustr,wpredok);

i1=1;
time(&vrem);
for(i=mr; i < mr+3 ; i++)
 {
  sprintf(strsql,"insert into Uosamor \
values (%d,%d,%d,%d,'%s','%d',%.2f,%d,%ld)",in,i,gr,podd,hzt,in*(-1),
  amort[i1++],iceb_getuid(wpredok),vrem);

  iceb_sql_zapis(strsql,1,0,wpredok);

 }


}
/***********************************/
/*Для бюджетных организаций*/
/****************************/
void  amort23b(int in,
short gr,
int podd,
const char *hzt,
double bs,
double iz,
double bs1,
double iz1,
double kof,
double koff,
GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"delete from Uosamor where innom='%d' and god=%d",in,gr);

iceb_sql_zapis(strsql,1,0,wpredok);

/*
printw("\nbs=%.2f iz=%.2f bs1=%.2f iz1=%.2f us1.kof=%.2f koff=%.2f\n",
bs,iz,bs1,iz1,us1.kof,koff);
*/

class iceb_u_str menustr;

sprintf(strsql,"%s:%.2f %s:%.2f (%.2f/%.2f)",
gettext("Б.с."),
bs,
gettext("Износ"),
iz,bs1,iz1);

menustr.plus(strsql);

double amort=(bs+bs1-iz-iz1)*kof/100*koff;

sprintf(strsql,"(%.2f+%.2f-%.2f-%.2f)*%.2f/100*%.2f=%.2f",
bs,bs1,iz,iz1,kof,koff,amort);

if(amort < 0.)
 amort=0.;
 
menustr.ps_plus(strsql);

amort=iceb_u_okrug(amort,1.); //Округляем до гривны
sprintf(strsql,"%s: %.2f",
gettext("Годовая норма"),
amort);

menustr.ps_plus(strsql);

iceb_menu_soob(&menustr,wpredok);

time_t vrem;
time(&vrem);

sprintf(strsql,"insert into Uosamor \
values (%d,%d,%d,%d,'%s','%d',%.2f,%d,%ld)",in,12,gr,podd,hzt,in*(-1),
amort,iceb_getuid(wpredok),vrem);

iceb_sql_zapis(strsql,1,0,wpredok);

}




