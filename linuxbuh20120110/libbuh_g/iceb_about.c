/*$Id: iceb_about.c,v 1.5 2011-02-21 07:36:06 sasa Exp $*/
/*03.01.2011	14.04.2010	Белых А.И.	iceb_about.c
получение информации о системе
*/
#include        "iceb_libbuh.h"

extern char *version;

/*************************/

enum
 {
  VIHOD,
  KOLKNOP
 };

class glmenu_data
 {
  public:
  
  GtkWidget *window;
  GtkWidget *knopka[KOLKNOP];
  short     nomkn;

  glmenu_data() // Конструктор
   {
    nomkn=-1;
   }
 };

gboolean   iceb_aboutz_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data);
void  iceb_aboutz_knopka(GtkWidget *widget,class glmenu_data *data);


void iceb_aboutz(GtkWidget *wpredok)
{

class glmenu_data data;


data.window=gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

gtk_window_set_title(GTK_WINDOW(data.window),gettext("Разработчик"));

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_aboutz_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_vbox_new(FALSE,2);
gtk_container_add(GTK_CONTAINER(data.window),vbox);




//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
gtk_widget_realize(data.window);
//gtk_widget_show(data.window);


//GtkStyle *style;
//GdkPixmap *pixmap;
//GdkBitmap *mask;
//GtkWidget *pixmapWidget;

//style = gtk_widget_get_style(data.window);
//pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],
//                                    (gchar **)port_xpm);
//pixmapWidget=gtk_pixmap_new(pixmap,mask);
//gtk_container_add(GTK_CONTAINER(vbox),pixmapWidget);



//char bros[512];

//sprintf(bros,"%s:",gettext("LinuxBuh.RU"));
//sprintf(bros,"%s:",gettext(version));
//GtkWidget *name=gtk_label_new(iceb_u_toutf(bros));
//GtkWidget *version1=gtk_label_new(iceb_u_toutf(bros));
//name=gtk_label_new(gettext("Название программы"));
//gtk_box_pack_start((GtkBox*)vbox,name,TRUE,TRUE,0);
//version1=gtk_label_new(gettext("Версия программы"));
//gtk_box_pack_start((GtkBox*)vbox,version1,TRUE,TRUE,0);


printf("%s\n",__FUNCTION__);

GtkWidget *about=gtk_about_dialog_new();
gtk_signal_connect(GTK_OBJECT(about),"response",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
//g_signal_connect_swapped(about,"response",G_CALLBACK(gtk_widget_destroy),about);

gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(about),"iceBw");
gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about),version);
gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about),"http://iceb.com.ua");
gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about),gettext("Бухгалтерский учёт"));
//gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about),"Copyright \302\251 Aleksandr Belykh");
gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about),"Copyright © Aleksandr Belykh");



const gchar *avtor[]={"Aleksandr Belykh","<sasa@ukrpost.ua>","http://www.iceb.vc.ukrtel.net","tel:+380676654526"};
 
gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about),avtor);


gtk_dialog_run(GTK_DIALOG(about));

gtk_box_pack_start((GtkBox*)vbox,about,TRUE,TRUE,0);

data.knopka[VIHOD]=gtk_button_new_with_label(gettext("Выход"));
//tooltips[VIHOD]=gtk_tooltips_new();
//gtk_tooltips_set_tip(tooltips[VIHOD],data.knopka[VIHOD],gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[VIHOD]),"clicked",GTK_SIGNAL_FUNC(iceb_aboutz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[VIHOD]),(gpointer)VIHOD);
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[VIHOD], TRUE, TRUE,0);



gtk_widget_grab_focus(data.knopka[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_aboutz_knopka(GtkWidget *widget,class glmenu_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("glkni_v_knopka knop=%d\n",knop);
data->nomkn=knop;
gtk_widget_destroy(data->window);
data->window=NULL;

}



/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_aboutz_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data)
{

switch(event->keyval)
 {

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[VIHOD]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

//    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}



