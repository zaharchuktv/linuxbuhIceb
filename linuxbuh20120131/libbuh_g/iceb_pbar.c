/*$Id: iceb_pbar.c,v 1.16 2011-01-13 13:49:59 sasa Exp $*/
/*05.03.2009	31.10.2003	Белых А.И.	iceb_pbar.c
Формирование строки процента выполнения
*/
#include  "iceb_libbuh.h"

void    iceb_pbar(GtkWidget *pbar,int kolstr,gfloat kolstr1)
{
if(pbar != NULL)
 {
  static int proc_zap;
  if(kolstr1 == 1)
   proc_zap=0; 
  int proc=(int)(kolstr1*100)/kolstr;
  if(proc_zap == proc)
   return;
  proc_zap=proc;
/**************/
  char strsql[512];
  sprintf(strsql,"%d%%",proc);

  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(pbar),strsql);
/****************/
  double pp=(double)proc;  

  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pbar),pp/100.);
 }
while(gtk_events_pending())
 gtk_main_iteration();

}
