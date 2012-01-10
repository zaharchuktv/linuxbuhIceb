/*$Id: l_klientr_vg.c,v 1.11 2011-02-21 07:36:20 sasa Exp $*/
/*16.08.2006	16.08.2006	Белых А.И.	l_klientr_vg.c
Ввод клиентов в группу
*/
#include "i_rest.h"
#include <unistd.h>
#include "l_klientr.h"
enum
{
 GFK2,
 GFK10,
 G_KOL_F_KL
};

enum
{
 E_KODGR,
 E_KODKL,
 G_KOLENTER
};

class l_klientr_vg_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[G_KOL_F_KL];
  GtkWidget *knopka_enter[G_KOLENTER];
  GtkWidget *entry[G_KOLENTER];
  GtkWidget *opt;
  short     kl_shift; //0-отжата 1-нажата  

  class  klientr_data *data_sp_kl;
       
  iceb_u_str kodkl;
  iceb_u_str kodgr;
  guint     timer;
      
  l_klientr_vg_data()
   {
    kl_shift=0;
    timer=0;
    kodkl.plus("");
    kodgr.plus("");
   }
  void read_rek()
   {
    for(int i=0; i < G_KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
  
  void clear_rek()
   {
    for(int i=0; i < G_KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };

void    l_klientr_vg_vvod(GtkWidget *widget,class l_klientr_vg_data *data);
void  l_klientr_vg_knopka(GtkWidget *widget,class l_klientr_vg_data *data);
gboolean   l_klientr_vg_key_press(GtkWidget *widget,GdkEventKey *event,class l_klientr_vg_data *data);
void  l_klientr_vg_e_knopka(GtkWidget *widget,class l_klientr_vg_data *data);

int l_klient_vg_zap(class l_klientr_vg_data *data);

extern SQL_baza	bd;
extern char *name_system;


void l_klientr_vg(class  klientr_data *data_sp_kl)
{
char strsql[500];
class l_klientr_vg_data data;
data.data_sp_kl=data_sp_kl;





data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_klientr_vg_key_press),&data);


if(data_sp_kl->window != NULL)
 {
  gdk_window_set_cursor(data_sp_kl->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(data_sp_kl->window));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label;
sprintf(strsql,"%s",gettext("Ввод клиентов в группу"));

label=gtk_label_new(iceb_u_toutf(strsql));

sprintf(strsql,"%s %s",name_system,gettext("Ввод клиентов в группу"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[G_KOLENTER+1];
for(int i=0 ; i < G_KOLENTER+1; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0 ; i < G_KOLENTER+1; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[G_KOLENTER];

sprintf(strsql,"%s",gettext("Код группы"));
data.knopka_enter[E_KODGR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.knopka_enter[E_KODGR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODGR]),"clicked",GTK_SIGNAL_FUNC(l_klientr_vg_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODGR]),(gpointer)E_KODGR);
tooltips_enter[E_KODGR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODGR],data.knopka_enter[E_KODGR],gettext("Выбор группы"),NULL);

data.entry[E_KODGR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.entry[E_KODGR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODGR]), "activate",GTK_SIGNAL_FUNC(l_klientr_vg_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODGR]),data.kodgr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODGR]),(gpointer)E_KODGR);

label=gtk_label_new(gettext("Код клиента"));
data.entry[E_KODKL] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.entry[E_KODKL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODKL]), "activate",GTK_SIGNAL_FUNC(l_klientr_vg_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKL]),data.kodkl.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODKL]),(gpointer)E_KODKL);

#ifdef READ_CARD
extern tdcon           td_server; 
if(td_server >= 0)
 {
  class iceb_read_card_enter_data card;
  card.window=data.window;
  card.entry=data.entry[E_KODKL];
  data.timer=card.timer=gtk_timeout_add(500,(GtkFunction)iceb_read_card_enter,&card);
 }
#endif


GtkTooltips *tooltips[G_KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[GFK2]=gtk_button_new_with_label(strsql);
tooltips[GFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[GFK2],data.knopka[GFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[GFK2]),"clicked",GTK_SIGNAL_FUNC(l_klientr_vg_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[GFK2]),(gpointer)GFK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[GFK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[GFK10]=gtk_button_new_with_label(strsql);
tooltips[GFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[GFK10],data.knopka[GFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[GFK10]),"clicked",GTK_SIGNAL_FUNC(l_klientr_vg_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[GFK10]),(gpointer)GFK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[GFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data_sp_kl->window != NULL)
  gdk_window_set_cursor(data_sp_kl->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_klientr_vg_e_knopka(GtkWidget *widget,class l_klientr_vg_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_KODGR:
    if(l_grkl(1,&kod,&naim,data->window) == 0)
     {
      data->kodgr.new_plus(kod.ravno());
      data->kodgr.plus(" ");
      data->kodgr.plus(naim.ravno());
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGR]),data->kodgr.ravno_toutf());
    return;  

 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_klientr_vg_key_press(GtkWidget *widget,GdkEventKey *event,class l_klientr_vg_data *data)
{

//printf("l_klientr_vg_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[GFK2]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[GFK10]),"clicked");
    return(FALSE);

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
void  l_klientr_vg_knopka(GtkWidget *widget,class l_klientr_vg_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case GFK2:
    data->read_rek();

    if(l_klient_vg_zap(data) != 0)
     {
      data->kodkl.new_plus("");
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKL]),data->kodkl.ravno_toutf());
     }
    else
     {
      klientr_create_list(data->data_sp_kl);
      gdk_window_set_cursor(data->data_sp_kl->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
      gtk_widget_grab_focus(data->entry[1]);
     }
    return;  


  case GFK10:
    if(data->timer != 0)
        gtk_timeout_remove(data->timer);
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_klientr_vg_vvod(GtkWidget *widget,class l_klientr_vg_data *data)
{
char strsql[500];
SQL_str row;
class SQLCURSOR cur;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_klientr_vg_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KODGR:
    data->kodgr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select kod,naik from Grupklient where kod='%s'",data->kodgr.ravno_dpp());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      data->kodgr.new_plus(row[0]); //читает много раз - чтобы наименование всё время не добавлялось
      data->kodgr.plus(" ");
      data->kodgr.plus(row[1]);
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGR]),data->kodgr.ravno_toutf());

    break;
  case E_KODKL:
    data->kodkl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }

enter+=1;
if(enter >= G_KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}

/***********************************/
/*Присвоение группы клиенту и если нет такого клиента ввод новой записи клиента с назначенной группой*/
/************************************/

int l_klient_vg_zap(class l_klientr_vg_data *data)
{
char strsql[500];
//Проверяем есть ли такая группа
sprintf(strsql,"select kod from Grupklient where kod='%s'",data->kodgr.ravno_dpp());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код группы"),data->kodgr.ravno_dpp());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
time_t vrem;
time(&vrem);

//Проверяем есть ли такой клиент
sprintf(strsql,"select fio from Taxiklient where kod='%s'",data->kodkl.ravno());
if(iceb_sql_readkey(strsql,data->window) > 0)
 {
  //клиент есть в базе
  sprintf(strsql,"update Taxiklient set grup='%s',ktoi=%d,vrem=%ld where kod='%s'",
  data->kodgr.ravno_dpp(),iceb_getuid(data->window),vrem,
  data->kodkl.ravno());
 }
else
 {
  
  sprintf(strsql,"insert into Taxiklient (kod,grup,ktoi,vrem) values ('%s','%s',%d,%ld)",
  data->kodkl.ravno(),data->kodgr.ravno_dpp(),iceb_getuid(data->window),vrem);
 }

return(iceb_sql_zapis(strsql,1,0,data->window));

}


