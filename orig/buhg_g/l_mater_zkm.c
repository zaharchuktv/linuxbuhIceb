/*$Id: l_mater_zkm.c,v 1.10 2011-02-21 07:35:53 sasa Exp $*/
/*12.11.2008	19.05.2004	Белых А.И.	l_mater_zkm.c
Замена одного кода материолла на другой с удалением первого
*/
#include <unistd.h>
#include <stdlib.h>
#include "buhg_g.h"


class mater_zkm_data
 {
  public:
  
  GtkWidget *window;
  GtkWidget *label;
  
  short      prohod;
  iceb_u_str kodm; 
  iceb_u_str kodmz; 
    
  mater_zkm_data()
   {
    prohod=0;
   }

 };

gint mater_zkm1(class mater_zkm_data *data);

extern SQL_baza	bd;
extern char *name_system;

void  l_mater_zkm(const char *kodm,GtkWidget *wpredok)
{
mater_zkm_data data;

data.kodm.plus(kodm);

iceb_u_str repl;
repl.plus(gettext("Замена одного кода на другой с удалением первого"));

repl.ps_plus(gettext("Код материалла"));
repl.plus(":");
repl.plus(kodm);
repl.ps_plus(gettext("Введите код материалла на который нужно заменить"));

if(iceb_menu_vvod1(&repl,&data.kodmz,20,wpredok) != 0)
  return;

if(data.kodmz.getdlinna() <= 1)
 return;

char strsql[512];
sprintf(strsql,"select kodm from Material where kodm=%s",data.kodmz.ravno());
if(iceb_sql_readkey(strsql,wpredok) != 1)
 {
  repl.new_plus(gettext("Не найден код материалла"));
  repl.plus(" ");
  repl.plus(data.kodmz.ravno());
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);
  return;
 }
if(l_mater_v_provkor(data.kodmz.ravno_atoi(),atoi(kodm),wpredok) != 0)
 return;

if(iceb_parol(0,wpredok) != 0)
  return;

/************************************/
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_default_size (GTK_WINDOW  (data.window),300,-1);


sprintf(strsql,"%s %s",name_system,gettext("Замена одного кода на другой с удалением первого"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);
gtk_container_add(GTK_CONTAINER(data.window), vbox);

repl.new_plus(gettext("Ждите !!!"));
GtkWidget *label=gtk_label_new(repl.ravno_toutf());
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

repl.new_plus(gettext("Замена одного кода на другой с удалением первого"));

label=gtk_label_new(repl.ravno_toutf());
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

gtk_widget_show_all(vbox);


gtk_idle_add((GtkFunction)mater_zkm1,&data);

gtk_widget_show_all(data.window);
//gtk_widget_realize(data.window);
//gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
}

/****************************************************/

gint mater_zkm1(class mater_zkm_data *data)
{
char strsql[1024];

mater_kkvt(data->kodm.ravno_atoi(),data->kodmz.ravno_atoi(),data->window);

if(mater_pvu(1,data->kodm.ravno(),data->window) != 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

sprintf(strsql,"delete from Material where kodm = %s",data->kodm.ravno());
printf("%s\n",strsql);
if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);


gtk_widget_destroy(data->window);

return(FALSE);

}
