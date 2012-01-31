/*$Id: iceb_ponp.c,v 1.17 2011-02-21 07:36:07 sasa Exp $*/
/*11.11.2008	22.09.2003	Белых А.И.	iceb_ponp.c
Просмотр очереди на печать
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "iceb_libbuh.h"
#define   KOLKNOP 3

struct iceb_ponp_data
 {
  GtkWidget *knopka[KOLKNOP];
  GtkWidget *window;
  
 };

gboolean   iceb_ponp_key_press(GtkWidget *,GdkEventKey *,struct iceb_ponp_data *);
void       iceb_ponp_knopka(GtkWidget *,struct iceb_ponp_data *);

extern char *name_system;

void  iceb_ponp(const char* lpq,const char *lprm,GtkWidget *wpredok)
{
FILE *ff;
char imaf[32];
char bros[312];
iceb_ponp_data  data;

printf("%s\n",__FUNCTION__);
memset(&data,'\0',sizeof(data));

sprintf(imaf,"iceb_ponp%d.tmp",getpid());

sprintf(bros,"%s > %s",lpq,imaf);
printf("iceb_ponp-%s\n",bros);

system(bros);


if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_u_str SP;
  sprintf(bros,"%s: %d !",gettext("Ошибка открытия файла"),errno);
  SP.plus_ps(bros);
  SP.plus(imaf);
  iceb_menu_soob(&SP,NULL);
  return;
 }

unlink(imaf);

iceb_u_spisok SP(0);

while(fgets(bros,sizeof(bros),ff) != NULL)
 {
  printf("%s",bros);
  iceb_u_ps_minus(bros);
  SP.plus(bros);
 }
fclose(ff);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
//gtk_widget_set_usize(GTK_WIDGET(data.window), 300, 100);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal( GTK_WINDOW(data.window ) ,TRUE ); 

sprintf(bros,"%s %s",name_system,gettext("Просмотр очереди на печать."));
gtk_window_set_title(GTK_WINDOW(data.window),bros);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_ponp_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);
GtkWidget *hbox=gtk_hbox_new(FALSE, 2);
gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);
gtk_container_add(GTK_CONTAINER(data.window), vbox);
gtk_widget_show(vbox);

GtkWidget *scrolled_window=gtk_scrolled_window_new(NULL, NULL);

gtk_widget_set_usize(scrolled_window, 600, 250);

gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);

gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);

GtkWidget *gtklist=gtk_list_new();
gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW(scrolled_window),gtklist);

//gtk_signal_connect(GTK_OBJECT(gtklist),"selection_changed",GTK_SIGNAL_FUNC(sigh_print_selection),NULL);

GtkWidget *list_item;
int kolstrok=SP.kolih();

GList *dlist;
dlist=NULL;
for(int i=kolstrok; i >= 0 ; i--)
 {
  list_item=gtk_list_item_new_with_label(SP.ravno_toutf(i));
  dlist=g_list_prepend(dlist, list_item);
 }
gtk_list_append_items(GTK_LIST(gtklist), dlist);


GtkTooltips *tooltips[KOLKNOP];

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(hbox), data.knopka[2], TRUE, TRUE, 0);
tooltips[2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[2],data.knopka[2],gettext("Закрыть окно."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[2]), "clicked",GTK_SIGNAL_FUNC(iceb_ponp_knopka),&data);
gtk_widget_set_name(data.knopka[2],"10");
gtk_widget_grab_focus(data.knopka[2]);

gtk_widget_show_all(data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_ponp_knopka(GtkWidget *widget,struct iceb_ponp_data *data)
{
//char    bros[512];

gchar *knop=(gchar*)gtk_widget_get_name(widget);
//g_print("iceb_ponp_knopka knop=%s\n",knop);

switch (atoi(knop))
 {
  case 2:

    return;  

  case 10:
    gtk_widget_destroy(data->window);
    return;
 }

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_ponp_key_press(GtkWidget *widget,GdkEventKey *event,struct iceb_ponp_data *data)
{
//char  bros[512];

switch(event->keyval)
 {

  case GDK_F2:
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_widget_destroy(widget);
    return(FALSE);
 }

return(TRUE);

}
