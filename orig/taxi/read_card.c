/*$Id: read_card.c,v 1.18 2011-02-21 07:36:21 sasa Exp $*/
/*17.04.2005	05.03.2005	Белых А.И.	read_card.c
Окно с сообщение о чтении магнитной карты
*/
#include <errno.h>
//#include <iceb_d_libdev.h>
#include <unistd.h>
#include "i_rest.h"

enum
{
  FK10,
  KOL_F_KL
};

class read_card_data
 {

  public:

  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *knopka[KOL_F_KL];
  guint     timer;
  
  short prkk;
  int voz;
  
  char *cod;
  int time_out;
  
 };

gint read_card1(class read_card_data *data);

gboolean   read_card_key_press(GtkWidget *widget,GdkEventKey *event,class read_card_data *data);
void  read_card_v_knopka(GtkWidget *widget,class read_card_data *data);
#ifdef READ_CARD
int read_dev_card(tdcon  td_server,iceb_u_str *kod_kart,int,GtkWidget *wpredok);
#endif
void  read_card_knopka(GtkWidget *widget,class read_card_data *data);
void read_card_t(class read_card_data *data);

extern char *name_system;
extern iceb_u_str shrift_ravnohir;
#ifdef READ_CARD
extern tdcon           td_server; 
#endif 
void read_card(int time_out,char *cod,GtkWidget *wpredok)
{
char strsql[500];
class read_card_data data;

data.cod=cod;
data.time_out=time_out;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Чтение магнитной карточки"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
//gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(read_card_key_press),&data);

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

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

GtkWidget *label=gtk_label_new(gettext("Поднесите магнитную карточку к устройству чтения"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox),data.knopka[FK10], TRUE, TRUE, 0);
GtkTooltips *tooltips5=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips5,data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(read_card_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

gtk_widget_grab_focus(data.knopka[FK10]);

gtk_widget_show_all(data.window);

//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна


//  data->timer_read=gtk_timeout_add(vrem,(GtkFunction)zapzaktaxi,(gpointer)data);
data.timer=gtk_timeout_add(100,(GtkFunction)read_card_t,&data);

//gtk_idle_add((GtkFunction)read_card1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
void read_card_t(class read_card_data *data)
{
#ifdef READ_CARD
printf("read_card_t\n");
char kod_kart[30];
iceb_refresh();
int i=0;
if((i=iceb_read_card(td_server,kod_kart,sizeof(kod_kart),data->time_out,data->window)) < 0)
 {
  gtk_timeout_remove(data->timer);
  gtk_widget_destroy(data->window);
  data->voz=1;
  return;
 }    
if(i > 0)
 return;
 
strcpy(data->cod,kod_kart);
gtk_timeout_remove(data->timer);
gtk_widget_destroy(data->window);
data->voz=0;
return;
#endif
}

/******************************************/
/******************************************/

gint read_card1(class read_card_data *data)
{

#ifdef READ_CARD
iceb_refresh();
int i=0;
char kod_kart[30];

for(int kk=0; kk < 5 ; kk++)
 {
iceb_refresh();
  if((i=iceb_read_card(td_server,kod_kart,sizeof(kod_kart),data->time_out,data->window)) < 0)
   {
    gtk_timeout_remove(data->timer);
    gtk_widget_destroy(data->window);

    return(FALSE);
   }    
  if(i > 0)
   {
    strcpy(data->cod,kod_kart);
    break;
   }  
 }


gtk_widget_destroy(data->window);
#endif
return(FALSE);
}
/*****************************************/
/*****************************************/

void  read_card_knopka(GtkWidget *widget,class read_card_data *data)
{
iceb_u_str kk;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {


    case FK10:
//    printf("read_card_knopka F10\n");
    gtk_timeout_remove(data->timer);
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
