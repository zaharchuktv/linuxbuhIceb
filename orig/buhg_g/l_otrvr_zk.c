/*$Id: l_otrvr_zk.c,v 1.5 2011-02-21 07:35:53 sasa Exp $*/
/*08.05.2009	05.05.2009	Белых А.И.	l_otrvr_zk.c
Меню для ввода реквизитов поиска 
*/
#include <stdlib.h>
#include "buhg_g.h"
enum
 {
  E_SKOD,
  E_NKOD,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_otrvr_zk_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  class iceb_u_str skod;
  class iceb_u_str nkod;
  
  l_otrvr_zk_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();    
   }

  void read_rek()
   {
    skod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SKOD]))));
    nkod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NKOD]))));
   }

  void clear_data()
   {
    clear();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
  void clear()
   {
    skod.new_plus("");
    nkod.new_plus("");
   }
 };

void l_otrvr_zk_clear(class l_otrvr_zk_data *data);
void    l_otrvr_zk_vvod(GtkWidget *widget,class l_otrvr_zk_data *data);
void  l_otrvr_zk_knopka(GtkWidget *widget,class l_otrvr_zk_data *data);
gboolean   l_otrvr_zk_key_press(GtkWidget *widget,GdkEventKey *event,class l_otrvr_zk_data *data);
void  l_otrvr_zk_v_e_knopka(GtkWidget *widget,class l_otrvr_zk_data *data);

extern char      *name_system;

int l_otrvr_zk(class iceb_u_str *skod,class iceb_u_str *nkod,GtkWidget *wpredok)
{
class l_otrvr_zk_data data;
char strsql[512];


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_otrvr_zk_key_press),&data);

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

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Код который нужно заменить"));
data.knopka_enter[E_SKOD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SKOD]), data.knopka_enter[E_SKOD], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SKOD]),"clicked",GTK_SIGNAL_FUNC(l_otrvr_zk_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SKOD]),(gpointer)E_SKOD);
tooltips_enter[E_SKOD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SKOD],data.knopka_enter[E_SKOD],gettext("Выбор кода табеля"),NULL);

data.entry[E_SKOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_SKOD]), data.entry[E_SKOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKOD]), "activate",GTK_SIGNAL_FUNC(l_otrvr_zk_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKOD]),data.skod.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKOD]),(gpointer)E_SKOD);

sprintf(strsql,"%s",gettext("Код на который нужно заменить"));
data.knopka_enter[E_NKOD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NKOD]), data.knopka_enter[E_NKOD], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NKOD]),"clicked",GTK_SIGNAL_FUNC(l_otrvr_zk_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NKOD]),(gpointer)E_NKOD);
tooltips_enter[E_NKOD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_NKOD],data.knopka_enter[E_NKOD],gettext("Выбор кода табеля"),NULL);

data.entry[E_NKOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_NKOD]), data.entry[E_NKOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NKOD]), "activate",GTK_SIGNAL_FUNC(l_otrvr_zk_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NKOD]),data.nkod.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NKOD]),(gpointer)E_NKOD);


GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Замена кода"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_otrvr_zk_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(l_otrvr_zk_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Поиск не выполнять"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_otrvr_zk_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

if(data.voz == 0)
 {
  skod->new_plus(data.skod.ravno());
  nkod->new_plus(data.nkod.ravno());
 }
return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_otrvr_zk_v_e_knopka(GtkWidget *widget,class l_otrvr_zk_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_SKOD:
    if(l_zarvidtab(1,&kod,&naim,data->window) == 0)
     {
      data->skod.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKOD]),data->skod.ravno_toutf());
     }
    return;  

  case E_NKOD:
    if(l_zarvidtab(1,&kod,&naim,data->window) == 0)
     {
      data->nkod.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NKOD]),data->nkod.ravno_toutf());
     }
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_otrvr_zk_key_press(GtkWidget *widget,GdkEventKey *event,class l_otrvr_zk_data *data)
{
//char  bros[512];

//printf("vl_zarkateg_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(FALSE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
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
void  l_otrvr_zk_knopka(GtkWidget *widget,class l_otrvr_zk_data *data)
{
char strsql[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_otrvr_zk_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    sprintf(strsql,"select kod from Tabel where kod=%d",data->skod.ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код табеля"),data->skod.ravno_atoi());
      iceb_menu_soob(strsql,data->window);
      return;
     }        
    sprintf(strsql,"select kod from Tabel where kod=%d",data->nkod.ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код табеля"),data->nkod.ravno_atoi());
      iceb_menu_soob(strsql,data->window);
      return;
     }        
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_otrvr_zk_vvod(GtkWidget *widget,class l_otrvr_zk_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (enter)
 {
  case E_SKOD:
    data->skod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_SKOD]))));
    break;
  case E_NKOD:
    data->nkod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_NKOD]))));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
