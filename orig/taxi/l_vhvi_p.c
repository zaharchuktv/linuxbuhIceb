/*$Id: l_vhvi_p.c,v 1.8 2011-02-21 07:36:20 sasa Exp $*/
/*02.09.2006	13.12.2005	Белых А.И.	l_vhvi_p.c
Ввод и корректировка реквизитов поиска
*/
#include <unistd.h>
#include "i_rest.h"
#include "l_vhvi.h"

enum
{
 FK2,
 FK4,
 FK10,
 KOL_F_KL
};

enum
{
 E_DATAN,
 E_VREMN,
 E_DATAK,
 E_VREMK,
 E_PODR,
 E_KODKART,
 E_TIPZ,
 KOLENTER
};

class l_vhvi_p_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *entry[KOLENTER];
  GtkWidget *opt;
  short     kl_shift; //0-отжата 1-нажата  

  int       voz;
       
  class l_vhvi_rp *poi;
  iceb_u_str kodkl;
  iceb_u_spisok menu_kodgr;
  int           vib_kodgr;
      
  l_vhvi_p_data()
   {
    vib_kodgr=0;
    voz=0;
    kl_shift=0;
   }
  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
  
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    poi->clear_data();
   }
 };


gboolean   l_vhvi_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_vhvi_p_data *data);
void  l_vhvi_p_knopka(GtkWidget *widget,class l_vhvi_p_data *data);
void    l_vhvi_p_vvod(GtkWidget *widget,class l_vhvi_p_data *data);
void  l_vhvi_p_v_e_knopka(GtkWidget *widget,class l_vhvi_p_data *data);

extern SQL_baza	bd;
extern char *name_system;


int l_vhvi_p(class l_vhvi_rp *datap,GtkWidget *wpredok)
{
char strsql[300];

class l_vhvi_p_data data;
data.poi=datap;
printf("l_vhvi_p\n");
    
data.kl_shift=0;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Просмотр движения людей по подразделениям"));


gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_vhvi_p_key_press),&data);


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

GtkWidget *label;
label=gtk_label_new (gettext("Просмотр движения людей по подразделениям"));
gtk_box_pack_start (GTK_BOX (vbox),label,FALSE, FALSE, 0);

for(int i=0 ; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0 ; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];


sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(l_vhvi_p_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(l_vhvi_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.poi->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

sprintf(strsql,"%s (%s)",gettext("Время начала"),gettext("ч.м.с"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_VREMN]), label, FALSE, FALSE, 0);

data.entry[E_VREMN] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[E_VREMN]), data.entry[E_VREMN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VREMN]), "activate",GTK_SIGNAL_FUNC(l_vhvi_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREMN]),data.poi->vremn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VREMN]),(gpointer)E_VREMN);


sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(l_vhvi_p_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(l_vhvi_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.poi->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);

sprintf(strsql,"%s (%s)",gettext("Время конца"),gettext("ч.м.с"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_VREMK]), label, FALSE, FALSE, 0);

data.entry[E_VREMK] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[E_VREMK]), data.entry[E_VREMK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VREMK]), "activate",GTK_SIGNAL_FUNC(l_vhvi_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREMK]),data.poi->vremk.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VREMK]),(gpointer)E_VREMK);


sprintf(strsql,"%s (,,)",gettext("Код клиента"));
data.knopka_enter[E_KODKART]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODKART]), data.knopka_enter[E_KODKART], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODKART]),"clicked",GTK_SIGNAL_FUNC(l_vhvi_p_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODKART]),(gpointer)E_KODKART);
tooltips_enter[E_KODKART]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODKART],data.knopka_enter[E_KODKART],gettext("Выбор клиента"),NULL);

data.entry[E_KODKART] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODKART]), data.entry[E_KODKART], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODKART]), "activate",GTK_SIGNAL_FUNC(l_vhvi_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKART]),data.poi->kodkart.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODKART]),(gpointer)E_KODKART);

sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(l_vhvi_p_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(l_vhvi_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.poi->podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);

sprintf(strsql,"%s (+/-/#)",gettext("Тип записи"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_TIPZ]), label, FALSE, FALSE, 0);

data.entry[E_TIPZ] = gtk_entry_new_with_max_length (1);
gtk_box_pack_start (GTK_BOX (hbox[E_TIPZ]), data.entry[E_TIPZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TIPZ]), "activate",GTK_SIGNAL_FUNC(l_vhvi_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TIPZ]),data.poi->tipz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TIPZ]),(gpointer)E_TIPZ);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_vhvi_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(l_vhvi_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без поиска нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_vhvi_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

printf("l_vhvi_p end\n");

return(data.voz);



}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_vhvi_p_v_e_knopka(GtkWidget *widget,class l_vhvi_p_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_DATAN:
    if(iceb_calendar(&data->poi->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->poi->datan.ravno());
    return;  

  case E_DATAK:
    if(iceb_calendar(&data->poi->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->poi->datak.ravno());
    return;  

  case E_KODKART:
    if(l_klientr(1,&kod,data->window) == 0)
      data->poi->kodkart.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKART]),data->poi->kodkart.ravno());
    return;  

  case E_PODR:

    if(rpod_l(1,&kod,&naim,data->window) == 0)
      data->poi->podr.z_plus(kod.ravno());
      
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->poi->podr.ravno());
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_vhvi_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_vhvi_p_data *data)
{
//char  bros[300];

//printf("l_vhvi_p_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");

    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(TRUE);
//    return(FALSE);

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
void  l_vhvi_p_knopka(GtkWidget *widget,class l_vhvi_p_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
naim.plus("");
kod.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek();
    
    gtk_widget_destroy(data->window);
    data->voz=1;  //С поиском
    
    return;  


  case FK4:
    data->clear_rek();
    return;


  case FK10:
    data->read_rek();
    data->voz=0; //Без поиска
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_vhvi_p_vvod(GtkWidget *widget,class l_vhvi_p_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_vhvi_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->poi->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATAK:
    data->poi->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KODKART:
    data->poi->kodkart.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PODR:
    data->poi->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_VREMN:
    data->poi->vremn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_VREMK:
    data->poi->vremk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_TIPZ:
    data->poi->tipz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
