/*$Id: l_klientr_bl.c,v 1.7 2011-02-21 07:36:20 sasa Exp $*/
/*15.09.2007	15.09.2007	Белых А.И.	l_klientr_bl.c
Меню для блокировки карточек клиентов
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
 E_DATABL,
 E_KODGR,
 G_KOLENTER
};

class l_klientr_bl_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[G_KOL_F_KL];
  GtkWidget *knopka_enter[G_KOLENTER];
  GtkWidget *entry[G_KOLENTER];
  GtkWidget *opt;
  short     kl_shift; //0-отжата 1-нажата  

  iceb_u_str databl;
  iceb_u_str kodgr;
  int voz;
      
  l_klientr_bl_data()
   {
    kl_shift=0;
    voz=0;
    databl.plus("");
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

void    l_klientr_bl_vvod(GtkWidget *widget,class l_klientr_bl_data *data);
void  l_klientr_bl_knopka(GtkWidget *widget,class l_klientr_bl_data *data);
gboolean   l_klientr_bl_key_press(GtkWidget *widget,GdkEventKey *event,class l_klientr_bl_data *data);
void  l_klientr_bl_e_knopka(GtkWidget *widget,class l_klientr_bl_data *data);


extern SQL_baza	bd;
extern char *name_system;


int l_klientr_bl(class iceb_u_str *databl,class iceb_u_str *grupa,GtkWidget *wpredok)
{
char strsql[500];
class l_klientr_bl_data data;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_klientr_bl_key_press),&data);

if(wpredok->window != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label;
sprintf(strsql,"%s",gettext("Блокировка карточек, по которым не было входов/выходов начиная с заданной даты"));

label=gtk_label_new(iceb_u_toutf(strsql));

sprintf(strsql,"%s %s",name_system,gettext("Блокировка неактивных карточек"));

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

sprintf(strsql,"%s",gettext("Дата"));
data.knopka_enter[E_DATABL]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATABL]), data.knopka_enter[E_DATABL], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATABL]),"clicked",GTK_SIGNAL_FUNC(l_klientr_bl_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATABL]),(gpointer)E_DATABL);
tooltips_enter[E_DATABL]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATABL],data.knopka_enter[E_DATABL],gettext("Выбор даты"),NULL);

data.entry[E_DATABL] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATABL]), data.entry[E_DATABL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATABL]), "activate",GTK_SIGNAL_FUNC(l_klientr_bl_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATABL]),data.databl.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATABL]),(gpointer)E_DATABL);


sprintf(strsql,"%s (,,)",gettext("Коды групп, карточки которых блокировать ненадо"));
data.knopka_enter[E_KODGR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.knopka_enter[E_KODGR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODGR]),"clicked",GTK_SIGNAL_FUNC(l_klientr_bl_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODGR]),(gpointer)E_KODGR);
tooltips_enter[E_KODGR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODGR],data.knopka_enter[E_KODGR],gettext("Выбор группы"),NULL);

data.entry[E_KODGR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.entry[E_KODGR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODGR]), "activate",GTK_SIGNAL_FUNC(l_klientr_bl_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODGR]),data.kodgr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODGR]),(gpointer)E_KODGR);




GtkTooltips *tooltips[G_KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Блокировка"));
data.knopka[GFK2]=gtk_button_new_with_label(strsql);
tooltips[GFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[GFK2],data.knopka[GFK2],gettext("Начать блокировку"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[GFK2]),"clicked",GTK_SIGNAL_FUNC(l_klientr_bl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[GFK2]),(gpointer)GFK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[GFK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[GFK10]=gtk_button_new_with_label(strsql);
tooltips[GFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[GFK10],data.knopka[GFK10],gettext("Завершение работы с меню"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[GFK10]),"clicked",GTK_SIGNAL_FUNC(l_klientr_bl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[GFK10]),(gpointer)GFK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[GFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok->window != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

if(data.voz == 0)
 {
  databl->new_plus(data.databl.ravno());
  grupa->new_plus(data.kodgr.ravno());
 }
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_klientr_bl_e_knopka(GtkWidget *widget,class l_klientr_bl_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("go_v_e_knopka knop=%d\n",knop);

switch ((gint)knop)
 {
  case E_DATABL:
    if(iceb_calendar(&data->databl,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATABL]),data->databl.ravno());
    return;  

  case E_KODGR:
    if(l_grkl(1,&kod,&naim,data->window) == 0)
      data->kodgr.z_plus(kod.ravno());

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGR]),data->kodgr.ravno_toutf());
    return;  

 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_klientr_bl_key_press(GtkWidget *widget,GdkEventKey *event,class l_klientr_bl_data *data)
{

//printf("l_klientr_bl_key_press\n");
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
void  l_klientr_bl_knopka(GtkWidget *widget,class l_klientr_bl_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case GFK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case GFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_klientr_bl_vvod(GtkWidget *widget,class l_klientr_bl_data *data)
{
char strsql[500];
SQL_str row;
class SQLCURSOR cur;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_klientr_bl_vvod enter=%d\n",enter);

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
  case E_DATABL:
    data->databl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }

enter+=1;
if(enter >= G_KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
