/* $Id: rasshetzw.c,v 1.11 2011-04-14 16:09:36 sasa Exp $ */
/*22.12.2010	15.09.2000	Белых А.И.	rasshetzw.c
Расчет начислений и удержаний заработной платы
*/
#include        <errno.h>
#include        "buhg_g.h"

void     zvaniew(int,short,short,int,const char *nah_only,FILE*,GtkWidget*);
void	zarindexw(int,short,short,int,const char *nah_only,FILE*,GtkWidget*);
void zarindexvw(int tn,short mp,short gp,int podr,const char *nah_only,FILE *ff_prot,GtkWidget *wpredok);
double   dolhnw(int,short,short,int,const char*,const char *nah_only,FILE*,GtkWidget*);
void     zarkmdw(int tn,short mp,short gp,int podr,const char *nah_only,FILE *ff,GtkWidget*);
double   podoh1w(int,short,short,int,const char *uder_only,FILE*,GtkWidget*);
void     procentw(int,short,short,int,const char *nah_only,FILE*,GtkWidget*);
void     dopdominw(int,short,short,int,double,const char *nah_only,FILE*,GtkWidget*);
double   bolnw(int,short,short,int,const char *uder_only,FILE*,GtkWidget*);
double   boln1w(int,short,short,int,const char *uder_only,FILE*,GtkWidget*);
void     profsw(int,short,short,int,const char *uder_only,FILE*,GtkWidget*);
void     kreditw(int,short,short,int,const char *uder_only,FILE*,GtkWidget*);
void     alimw(int,short,short,int,const char *uder_only,FILE*,GtkWidget*);
void     zaruozw(int,short,short,int,const char *uder_only,FILE*,GtkWidget*);
void     zarvpnw(int,short,short,int,const char *uder_only,FILE*,GtkWidget*);
void zardznw(int tn,short mp,short gp,int podr,const char *nah_only,FILE *ff,GtkWidget *wpredok);

extern short	kodindex;  /*Код индексации*/

void rasshetzw(int tabn,short mp,short gp,int kp,
int prn,  //1-только начисления 2-только удержания 3-все
const char *nah_only,
const char *uder_only,
FILE *ff_prot,
GtkWidget *wpredok)
{
double		koefpv;
/*
printw("rasshetz-%d %d.%d %d %d %d\n",tabn,mp,gp,sovm,kp,prn);
refresh();
*/
if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\nТабельный номер %d\n",tabn);
  fprintf(ff_prot,"Код начисления:%s\nКод удержания:%s\n",nah_only,uder_only);
 }

if(prn == 1 || prn == 3)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"%s\n",gettext("Протокол расчета начислений"));

  koefpv=dolhnw(tabn,mp,gp,kp,"",nah_only,ff_prot,wpredok); /*Должносной оклад*/

  zvaniew(tabn,mp,gp,kp,nah_only,ff_prot,wpredok); /*За звание*/

  procentw(tabn,mp,gp,kp,nah_only,ff_prot,wpredok); /*Процент*/

  zarindexw(tabn,mp,gp,kp,nah_only,ff_prot,wpredok); /*Расчет индексации*/

  zarindexvw(tabn,mp,gp,kp,nah_only,ff_prot,wpredok); /*индексация на невовремя выплаченную зарплату*/

  dopdominw(tabn,mp,gp,kp,koefpv,nah_only,ff_prot,wpredok); /*Расчет начисления до минимальной зарплаты*/
  
  zarkmdw(tabn,mp,gp,kp,nah_only,ff_prot,wpredok); //Расчёт командировочных
 
  zardznw(tabn,mp,gp,kp,nah_only,ff_prot,wpredok); /*Расчёт доплаты до средней зарплаты в командировке с учётом надбавки за награды*/
    
 }


if(prn == 2 || prn == 3)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"%s\n",gettext("Протокол расчета удержаний"));


  if(zapmasw(tabn,mp,gp,wpredok) == 0)
   {
    /*читаем их здесь так как при пересчёте подоходного для больничного они перечитываются на нужные месяца*/
    /*чтение величины минимальной зарплаты и прожиточного минимума*/
    zar_read_tnw(1,mp,gp,ff_prot,wpredok); 

    podoh1w(tabn,mp,gp,kp,uder_only,ff_prot,wpredok); /*Расчет подоходного налога,пенсионных,соцстраха*/


//    bolnw(tabn,mp,gp,kp,uder_only,ff_prot,wpredok); /*Перерасчет подоходного из за больничных*/
        
    zarvpnw(tabn,mp,gp,kp,uder_only,ff_prot,wpredok); /*возврат подоходного налога*/

    profsw(tabn,mp,gp,kp,uder_only,ff_prot,wpredok); /*Профсоюзный взнос*/

    kreditw(tabn,mp,gp,kp,uder_only,ff_prot,wpredok);  /*кредит */

    alimw(tabn,mp,gp,kp,uder_only,ff_prot,wpredok);    /*алименты*/

    zaruozw(tabn,mp,gp,kp,uder_only,ff_prot,wpredok); //удержание однодневного зароботка
    /* перерасчёт больничного делаем здесь так как будут перечитаны настройки по месяцам расчёта*/
    if(iceb_u_sravmydat(1,mp,gp,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
      bolnw(tabn,mp,gp,kp,uder_only,ff_prot,wpredok); /*Перерасчет подоходного из за больничных*/
    else        
      boln1w(tabn,mp,gp,kp,uder_only,ff_prot,wpredok); /*Перерасчет подоходного из за больничных*/

   }
 }
}
