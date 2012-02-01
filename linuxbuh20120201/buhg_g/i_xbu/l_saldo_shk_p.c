/*$Id: l_saldo_shk_p.c,v 1.12 2011-02-21 07:35:53 sasa Exp $*/
/*31.12.2003	31.12.2003	Белых А.И.	l_saldo_shk_p.c
Меню для ввода реквизитов поиска 
*/
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "l_saldo_shk.h"

enum
 {
  E_GOD,
  E_SHET,
  E_KODKONTR,
  E_DEBET,
  E_KREDIT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };


class saldo_shkp_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class  saldo_shk_poi *rk;

  saldo_shkp_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GOD]))));
    rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    rk->kodkontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODKONTR]))));
    rk->debet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DEBET]))));
    rk->kredit.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KREDIT]))));
   }
 };




void saldo_shk_p_clear(class saldo_shkp_data *data);
void    saldo_shk_p_vvod(GtkWidget *widget,class saldo_shkp_data *data);
void  saldo_shk_p_knopka(GtkWidget *widget,class saldo_shkp_data *data);
gboolean   saldo_shk_p_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_shkp_data *data);

void  l_saldo_shkp_v_e_knopka(GtkWidget *widget,class saldo_shkp_data *data);

extern char *name_system;

int l_saldo_shk_p(class saldo_shk_poi *rkp,GtkWidget *wpredok)
{
char strsql[512];
class saldo_shkp_data data;
data.rk=rkp;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Пoиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(saldo_shk_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }





GtkWidget *vbox = gtk_vbox_new (FALSE,1);
GtkWidget *hbox1 = gtk_hbox_new (FALSE,1);
GtkWidget *hbox2 = gtk_hbox_new (FALSE,1);
GtkWidget *hbox3 = gtk_hbox_new (FALSE,1);
GtkWidget *hbox4 = gtk_hbox_new (FALSE,1);
GtkWidget *hbox5 = gtk_hbox_new (FALSE,1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox5);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

GtkWidget *label=gtk_label_new(gettext("Год"));
data.entry[E_GOD] = gtk_entry_new_with_max_length (5);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_GOD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOD]), "activate",GTK_SIGNAL_FUNC(saldo_shk_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.rk->god.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOD]),(gpointer)E_GOD);


sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox2), data.knopka_enter[E_SHET], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(l_saldo_shkp_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_SHET], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(saldo_shk_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);


sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KODKONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox3), data.knopka_enter[E_KODKONTR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODKONTR]),"clicked",GTK_SIGNAL_FUNC(l_saldo_shkp_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODKONTR]),(gpointer)E_KODKONTR);
tooltips_enter[E_KODKONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODKONTR],data.knopka_enter[E_KODKONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_KODKONTR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_KODKONTR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODKONTR]), "activate",GTK_SIGNAL_FUNC(saldo_shk_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKONTR]),data.rk->kodkontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODKONTR]),(gpointer)E_KODKONTR);

label=gtk_label_new(gettext("Дебет"));
data.entry[E_DEBET] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_DEBET], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DEBET]), "activate",GTK_SIGNAL_FUNC(saldo_shk_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEBET]),data.rk->debet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DEBET]),(gpointer)E_DEBET);


label=gtk_label_new(gettext("Кредит"));
data.entry[E_KREDIT] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox5), data.entry[E_KREDIT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KREDIT]), "activate",GTK_SIGNAL_FUNC(saldo_shk_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KREDIT]),data.rk->kredit.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KREDIT]),(gpointer)E_KREDIT);




GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(saldo_shk_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(saldo_shk_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Поиск не выполнять"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(saldo_shk_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

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
void  l_saldo_shkp_v_e_knopka(GtkWidget *widget,class saldo_shkp_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_SHET:

    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno_toutf());

    return;  

  case E_KODKONTR:

    iceb_vibrek(0,"Kontragent",&data->rk->kodkontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKONTR]),data->rk->kodkontr.ravno_toutf());

    return;  


 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   saldo_shk_p_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_shkp_data *data)
{

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
    data->kl_shift=1;
    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldo_shk_p_knopka(GtkWidget *widget,class saldo_shkp_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek();
    data->voz=0;
    data->rk->metka_poi=1;
    gtk_widget_destroy(data->window);
    
    return;  

  case FK4:
    saldo_shk_p_clear(data);
    return;  


  case FK10:
    data->voz=1;
    data->rk->metka_poi=0;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    saldo_shk_p_vvod(GtkWidget *widget,class saldo_shkp_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("saldo_shk_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_GOD:
    data->rk->god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SHET:
    data->rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KODKONTR:
    data->rk->kodkontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DEBET:
    data->rk->debet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KREDIT:
    data->rk->kredit.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Очистка меню от введенной информации*/
/**************************************/
void saldo_shk_p_clear(class saldo_shkp_data *data)
{

data->rk->clear_zero();
for(int i=0; i< KOLENTER; i++)
 gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");

gtk_widget_grab_focus(data->entry[0]);

}
