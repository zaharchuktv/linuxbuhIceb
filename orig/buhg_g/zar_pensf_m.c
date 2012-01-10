/*$Id: zar_pensf_m.c,v 1.10 2011-02-21 07:35:59 sasa Exp $*/
/*15.01.2007	15.01.2007	Белых А.И.	zar_pensf_m.c
Меню для расчёта данных для перегруза в программу Пенсионного фонда Украины
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "zar_pensf.h"
enum
 {
  E_MESN,
  E_MESK,
  E_GOD,
  E_TABNOM,
  E_DATAD,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class zar_pensf_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class zar_pensf_rek *rk;
  
  zar_pensf_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->mesn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_MESN]))));
    rk->mesk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_MESK]))));
    rk->god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GOD]))));
    rk->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM]))));
    rk->datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAD]))));
   }
  
  void clear_rek()
   {

    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);

   }
 };


gboolean   zar_pensf_m_key_press(GtkWidget *widget,GdkEventKey *event,class zar_pensf_m_data *data);
void  zar_pensf_m_knopka(GtkWidget *widget,class zar_pensf_m_data *data);
void    zar_pensf_m_vvod(GtkWidget *widget,class zar_pensf_m_data *data);


void  zar_pensf_m_e_knopka(GtkWidget *widget,class zar_pensf_m_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int zar_pensf_m(class zar_pensf_rek *rek,GtkWidget *wpredok)
{

class zar_pensf_m_data data;
char strsql[512];
iceb_u_str kikz;
data.rk=rek;
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",name_system,gettext("Распечатать справки для Пенсионного фонда Украины"));
label=gtk_label_new(gettext("Распечатать справки для Пенсионного фонда Украины"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_pensf_m_key_press),&data);

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
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 1);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE, 1);



gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 1);

GtkTooltips *tooltips_enter[KOLENTER];



sprintf(strsql,"%s (%s)",gettext("Месяц начала"),gettext("м"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_MESN]), label,FALSE, FALSE, 0);

data.entry[E_MESN] = gtk_entry_new_with_max_length (7);
gtk_box_pack_start (GTK_BOX (hbox[E_MESN]), data.entry[E_MESN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_MESN]), "activate",GTK_SIGNAL_FUNC(zar_pensf_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MESN]),data.rk->mesn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_MESN]),(gpointer)E_MESN);


sprintf(strsql,"%s (%s)",gettext("Месяц конца"),gettext("м"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_MESK]), label,FALSE, FALSE, 0);

data.entry[E_MESK] = gtk_entry_new_with_max_length (7);
gtk_box_pack_start (GTK_BOX (hbox[E_MESK]), data.entry[E_MESK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_MESK]), "activate",GTK_SIGNAL_FUNC(zar_pensf_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MESK]),data.rk->mesk.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_MESK]),(gpointer)E_MESK);

sprintf(strsql,"%s (%s)",gettext("Год"),gettext("г"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label,FALSE, FALSE, 0);

data.entry[E_GOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOD]), "activate",GTK_SIGNAL_FUNC(zar_pensf_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.rk->god.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOD]),(gpointer)E_GOD);


sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_TABNOM]),"clicked",GTK_SIGNAL_FUNC(zar_pensf_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_TABNOM]),(gpointer)E_TABNOM);
tooltips_enter[E_TABNOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_TABNOM],data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"),NULL);

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TABNOM]), "activate",GTK_SIGNAL_FUNC(zar_pensf_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.rk->tabnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TABNOM]),(gpointer)E_TABNOM);

sprintf(strsql,"%s (%s)",gettext("Дата документа"),gettext("д.м.г"));
data.knopka_enter[E_DATAD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.knopka_enter[E_DATAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAD]),"clicked",GTK_SIGNAL_FUNC(zar_pensf_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAD]),(gpointer)E_DATAD);
tooltips_enter[E_DATAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAD],data.knopka_enter[E_DATAD],gettext("Выбор табельного номера"),NULL);

data.entry[E_DATAD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.entry[E_DATAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAD]), "activate",GTK_SIGNAL_FUNC(zar_pensf_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAD]),data.rk->tabnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAD]),(gpointer)E_DATAD);





GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчёт"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(zar_pensf_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(zar_pensf_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(zar_pensf_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  zar_pensf_m_e_knopka(GtkWidget *widget,class zar_pensf_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_DATAD:
    iceb_calendar(&data->rk->datad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->rk->datad.ravno_toutf());
    return;


  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->rk->tabnom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->rk->tabnom.ravno_toutf());
    return;
  
    
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_pensf_m_key_press(GtkWidget *widget,GdkEventKey *event,class zar_pensf_m_data *data)
{
//char  bros[512];

//printf("zar_pensf_m_key_press\n");
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
void  zar_pensf_m_knopka(GtkWidget *widget,class zar_pensf_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zar_pensf_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:

    data->read_rek();

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

    
  case FK4:
    data->clear_rek();
    return;

      
  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    zar_pensf_m_vvod(GtkWidget *widget,class zar_pensf_m_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zar_pensf_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_MESN:
    data->rk->mesn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_MESK:
    data->rk->mesk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_TABNOM:
    data->rk->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATAD:
    data->rk->datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_GOD:
    data->rk->god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Проверка правильности ввода данных*/
/***********************************/
int zar_pensf_prov(class zar_pensf_m_data *data)
{
if(data->rk->datad.getdlinna() > 1)
 if(data->rk->datad.prov_dat() != 0)
  {
   iceb_menu_soob(gettext("Неправильно введена дата документа !"),data->window);
   return(1);
  }

if(data->rk->mesn.ravno_atoi() < 1 || data->rk->mesn.ravno_atoi() > 12)
 {
  iceb_menu_soob(gettext("Неправильно ввели месяц начала расчёта !"),data->window);
  return(1);
 }

if(data->rk->mesk.ravno_atoi() < 1 || data->rk->mesk.ravno_atoi() > 12)
 {
  iceb_menu_soob(gettext("Неправильно ввели месяц конца расчёта !"),data->window);
  return(1);
 }

if(data->rk->mesn.ravno_atoi() > data->rk->mesk.ravno_atoi())
 {
  iceb_menu_soob(gettext("Месяц начала больше месяца конца !"),data->window);
  return(1);
 }


return(0);

}



