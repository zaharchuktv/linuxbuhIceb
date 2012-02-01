/*$Id: iceb_menu_mv.c,v 1.17 2011-02-21 07:36:07 sasa Exp $*/
/*27.04.2010	07.01.2004	Белых А.И.	iceb_menu_mv.c
Стандартное меню выбора
Возвращаем номер нажатой кнопки или -1 если кнопка не выбрана

*/
#include "iceb_libbuh.h"
class iceb_u_str iceb_menu_mv_str1("");
class iceb_u_str iceb_menu_mv_str2("");

gboolean menu_mv_event_delete(GtkWidget *window,GdkEvent *event,class vibknop_data *data);

extern char *name_system;

int  iceb_menu_mv(class iceb_u_str *titl,class iceb_u_str *zagolovok,class iceb_u_spisok *punkt_menu,
int nomer, //Номер строки в меню куда надо установить фокус
GtkWidget *wpredok)
{
//printf("iceb_menu_mv\n");
vibknop_data data;
char bros[512];
GdkColor color;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


sprintf(bros,"%s %s",name_system,titl->ravno_toutf());

gtk_window_set_title (GTK_WINDOW (data.window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data.window),0);
gtk_window_set_default_size(GTK_WINDOW(data.window),367,550); //горизонталь-вертикаль

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(menu_mv_event_delete),&data);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_vihod),&data.knopka);

GtkWidget *vbox=gtk_vbox_new(FALSE,0);
gtk_container_add(GTK_CONTAINER(data.window),vbox);

GtkWidget *hbox_hap=gtk_hbox_new(FALSE,0);
gtk_box_pack_start(GTK_BOX(vbox), hbox_hap, FALSE, FALSE,10);

GtkWidget *label=gtk_label_new(zagolovok->ravno_toutf());

gtk_box_pack_start(GTK_BOX(hbox_hap),label,FALSE,FALSE,10);
PangoFontDescription *font_pango=pango_font_description_from_string("Sans 16");
gtk_widget_modify_font(label,font_pango);
pango_font_description_free(font_pango);

/*разделитель между шапкой и кнопками*/
GtkWidget *event_razd=gtk_event_box_new();
gtk_box_pack_start(GTK_BOX(vbox),event_razd,FALSE,FALSE,0);
gdk_color_parse("#E7E7E8",&color);
gtk_widget_modify_bg(event_razd,GTK_STATE_NORMAL,&color);
gtk_widget_set_usize(GTK_WIDGET(event_razd),-1,25);

GtkWidget *hbox_razd=gtk_hbox_new(FALSE,0);

gtk_container_add(GTK_CONTAINER(event_razd),hbox_razd);

label=gtk_label_new(iceb_menu_mv_str1.ravno_toutf());
gtk_box_pack_start(GTK_BOX(hbox_razd), label, FALSE, FALSE,10);

font_pango=pango_font_description_from_string("Sans 10");
gtk_widget_modify_font(label,font_pango);


GtkWidget *hbox_knop=gtk_hbox_new(FALSE,0);
gtk_box_pack_start(GTK_BOX(vbox), hbox_knop, TRUE, TRUE,10);

GtkWidget *vbox_knop=gtk_vbox_new(FALSE,0);
gtk_box_pack_start(GTK_BOX(hbox_knop), vbox_knop, FALSE, FALSE,10);


int kolknop=punkt_menu->kolih();
GtkWidget *knopka[kolknop+1];
int nom;
for(nom=0; nom < kolknop; nom++)
 {
  knopka[nom]=gtk_button_new_with_label(punkt_menu->ravno_toutf(nom));
  gtk_button_set_alignment(GTK_BUTTON(knopka[nom]),0.,0.); /*Левое выравнивание текста*/
  gtk_box_pack_start(GTK_BOX(vbox_knop),knopka[nom],FALSE,FALSE,0);
  gtk_signal_connect(GTK_OBJECT(knopka[nom]),"clicked",GTK_SIGNAL_FUNC(iceb_menu_vibknop),&data);
  gtk_object_set_user_data(GTK_OBJECT(knopka[nom]),(gpointer)nom);
 }
knopka[nom]=gtk_button_new_with_label(gettext("Выход"));
gtk_button_set_alignment(GTK_BUTTON(knopka[nom]),0.,0.); /*Левое выравнивание текста*/
gtk_box_pack_end(GTK_BOX(vbox_knop),knopka[nom],FALSE,FALSE,0);
gtk_signal_connect(GTK_OBJECT(knopka[nom]),"clicked",GTK_SIGNAL_FUNC(iceb_menu_vibknop),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[nom]),(gpointer)nom);


/*завершающая строка*/
GtkWidget *event_end=gtk_event_box_new();
gtk_box_pack_end(GTK_BOX(vbox),event_end,FALSE,FALSE,0);
gdk_color_parse("#000000",&color);
gtk_widget_modify_bg(event_end,GTK_STATE_NORMAL,&color);
gtk_widget_set_usize(GTK_WIDGET(event_end),-1,25);

GtkWidget *hbox_end=gtk_hbox_new(FALSE,0);

gtk_container_add(GTK_CONTAINER(event_end),hbox_end);
label=gtk_label_new(iceb_menu_mv_str2.ravno_toutf());
gtk_box_pack_start(GTK_BOX(hbox_end), label, FALSE, FALSE,10);
gdk_color_parse("white",&color);
gtk_widget_modify_fg(label,GTK_STATE_NORMAL,&color);
gtk_widget_modify_font(label,font_pango);




gtk_widget_grab_focus(knopka[nomer]);




gtk_widget_show_all(data.window);

gdk_color_parse("white",&color);
gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);

pango_font_description_free(font_pango);

gtk_main();

if(data.knopka == kolknop)
  data.knopka=-1;

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
  
return(data.knopka);

}
/****************************/
/*Выход по delete_event     */
/*****************************/
gboolean menu_mv_event_delete(GtkWidget *window,GdkEvent *event,class vibknop_data *data)
{
 gtk_widget_destroy(data->window);
 data->knopka=-1;
 return(FALSE); 
}
