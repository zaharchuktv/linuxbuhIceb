/*$Id: rest_sptov_m.c,v 1.7 2011-02-21 07:36:21 sasa Exp $*/
/*04.10.2007	04.10.2007	Белых А.И.	rest_sptov_m.c
Меню для задания даты до которой нужно пометить товары как списанные
Помечаются только оплаченные документы
*/
#include <stdlib.h>
#include <unistd.h>
#include "i_rest.h"
enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAOPL,
  E_VREMOPL,
  KOLENTER
 };


class  rest_sptov_m_data
 {
  public:
  class iceb_u_str dataopl;
  class iceb_u_str vremopl;
  time_t vrem_spis;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       voz;  
  //Конструктор
  rest_sptov_m_data()
   {
    vrem_spis=0;
    dataopl.plus("");
    vremopl.plus("");
    voz=0;
    kl_shift=0;
    window=NULL;
   }      
  void read_rek()
   {
    for(int i=0; i < KOLENTER ; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
 };

gboolean   rest_sptov_m_key_press(GtkWidget *widget,GdkEventKey *event,class rest_sptov_m_data *data);
void  rest_sptov_m_knopka(GtkWidget *widget,class rest_sptov_m_data *data);
void    rest_sptov_m_vvod(GtkWidget *widget,class rest_sptov_m_data *data);
int restdok_pk(char *god,char *shet,GtkWidget*);

extern SQL_baza	bd;
extern char *name_system;


int  rest_sptov_m(time_t *vremspis,GtkWidget *wpredok)
{
char strsql[300];
rest_sptov_m_data data;
iceb_u_str nadpis;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Списание товаров"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rest_sptov_m_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
 
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++) 
 hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);
nadpis.new_plus(gettext("Списание товаров по оплаченным документам"));
nadpis.ps_plus(gettext("Введите дату и время до которых нужно списать товари"));

GtkWidget *label=gtk_label_new(nadpis.ravno_toutf());

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox),label);
for(int i=0; i < KOLENTER; i++) 
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

short nomer=0;
sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("д.м.г"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_DATAOPL] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[nomer]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[nomer++]), data.entry[E_DATAOPL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAOPL]), "activate",GTK_SIGNAL_FUNC(rest_sptov_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAOPL]),data.dataopl.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAOPL]),(gpointer)E_DATAOPL);

sprintf(strsql,"%s (%s)",gettext("Время"),gettext("ч.м.с"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_VREMOPL] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[nomer]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[nomer++]), data.entry[E_VREMOPL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VREMOPL]), "activate",GTK_SIGNAL_FUNC(rest_sptov_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREMOPL]),data.vremopl.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VREMOPL]),(gpointer)E_VREMOPL);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Списание"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать списание"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(rest_sptov_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(rest_sptov_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

*vremspis=data.vrem_spis;

return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rest_sptov_m_key_press(GtkWidget *widget,GdkEventKey *event,class rest_sptov_m_data *data)
{
//char  bros[300];

printf("rest_sptov_m_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
//    printf("Нажата клавиша F10\n");
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rest_sptov_m_knopka(GtkWidget *widget,class rest_sptov_m_data *data)
{
short d,m,g;
short has,min,sek;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("rest_sptov_m_knopka knop=%d\n",knop);

switch ((gint)knop)
 {
  case FK2:
    data->read_rek();
    if(iceb_u_rsdat(&d,&m,&g,data->dataopl.ravno(),1) != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
      return;
     }
    if(iceb_u_rstime(&has,&min,&sek,data->vremopl.ravno()) != 0)
     {
      iceb_menu_soob(gettext("Не правильно введено время !"),data->window);
      return;
     }
    data->vrem_spis=iceb_u_datetime_sec(d,m,g,has,min,sek);
    
    gtk_widget_destroy(data->window);

    data->voz=0;
    return;  


  case FK10:
//    printf("Нажата кнопка F10\n");
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    rest_sptov_m_vvod(GtkWidget *widget,class rest_sptov_m_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("rest_sptov_m_vvod enter=%d\n",enter);
switch (enter)
 {
  case E_DATAOPL:
    data->dataopl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VREMOPL:
    data->vremopl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
