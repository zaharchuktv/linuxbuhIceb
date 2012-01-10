/*$Id: iceb_fotoraz.c,v 1.5 2011-02-21 07:36:07 sasa Exp $*/
/*02.09.2007	02.09.2007	Белых А.И.	iceb_fotoraz.c
Фотография разработчика
*/
#include        "../knopki/port.xpm"
#include        "iceb_libbuh.h"

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

gboolean   iceb_fotoraz_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data);
void  iceb_fotoraz_knopka(GtkWidget *widget,class glmenu_data *data);


void iceb_fotoraz(GtkWidget *wpredok)
{

class glmenu_data data;


data.window=gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

gtk_window_set_title(GTK_WINDOW(data.window),gettext("Разработчик"));

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_fotoraz_key_press),&data);

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


GtkStyle *style;
GdkPixmap *pixmap;
GdkBitmap *mask;
GtkWidget *pixmapWidget;

style = gtk_widget_get_style(data.window);
pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],
                                    (gchar **)port_xpm);
pixmapWidget=gtk_pixmap_new(pixmap,mask);
gtk_container_add(GTK_CONTAINER(vbox),pixmapWidget);


char bros[512];
sprintf(bros,"%s:",gettext("Разработчик"));


GtkWidget *label=gtk_label_new(iceb_u_toutf(bros));

gtk_box_pack_start((GtkBox*)vbox,label,TRUE,TRUE,0);

label=gtk_label_new(gettext("Белых Александр Иванович"));

gtk_box_pack_start((GtkBox*)vbox,label,TRUE,TRUE,0);

data.knopka[VIHOD]=gtk_button_new_with_label(gettext("Выход"));
//tooltips[VIHOD]=gtk_tooltips_new();
//gtk_tooltips_set_tip(tooltips[VIHOD],data.knopka[VIHOD],gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[VIHOD]),"clicked",GTK_SIGNAL_FUNC(iceb_fotoraz_knopka),&data);
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
void  iceb_fotoraz_knopka(GtkWidget *widget,class glmenu_data *data)
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

gboolean   iceb_fotoraz_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data)
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

