/*$Id: iceb_l_plansh_p.c,v 1.14 2011-02-21 07:36:07 sasa Exp $*/
/*29.02.2008	31.12.2003	Белых А.И.	iceb_l_plansh_p.c
Меню для ввода реквизитов поиска 
*/
#include "iceb_libbuh.h"
#include "iceb_l_plansh.h"

enum
 {
  E_SHET,
  E_NAIM,
  E_KOD_SUBBAL,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };
class iceb_l_plansh_p
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class  plansh_rek *rk;
  
  iceb_l_plansh_p() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->kod_subbal.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_SUBBAL]))));
    rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    rk->naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM]))));
   }
  void clear_data()
   {
    rk->clear_zero();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void    plansh_p_vvod(GtkWidget *widget,class iceb_l_plansh_p *data);
void  plansh_p_knopka(GtkWidget *widget,class iceb_l_plansh_p *data);
gboolean   plansh_p_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_plansh_p *data);

void  iceb_l_plansh_p_e_knopka(GtkWidget *widget,class iceb_l_plansh_p *data);

int iceb_l_plansh_p(class plansh_rek *rek_poi,GtkWidget *wpredok)
{
char strsql[512];
class iceb_l_plansh_p data;
data.rk=rek_poi;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(plansh_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }




GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox2 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Номер счета"));
data.entry[E_SHET] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(plansh_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

label=gtk_label_new(gettext("Наименование счета"));
data.entry[E_NAIM] = gtk_entry_new_with_max_length (80);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_NAIM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(plansh_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk->naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

sprintf(strsql,"%s (,,)",gettext("Код суббаланса"));
data.knopka_enter[E_KOD_SUBBAL]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox3), data.knopka_enter[E_KOD_SUBBAL], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_SUBBAL]),"clicked",GTK_SIGNAL_FUNC(iceb_l_plansh_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_SUBBAL]),(gpointer)E_KOD_SUBBAL);
GtkTooltips *tooltips_enter=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter,data.knopka_enter[E_KOD_SUBBAL],gettext("Выбор кода суббаланса"),NULL);

data.entry[E_KOD_SUBBAL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_KOD_SUBBAL], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_SUBBAL]), "activate",GTK_SIGNAL_FUNC(plansh_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_SUBBAL]),data.rk->kod_subbal.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_SUBBAL]),(gpointer)E_KOD_SUBBAL);




GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(plansh_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[PFK4]=gtk_button_new_with_label(strsql);
tooltips[PFK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK4],data.knopka[PFK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK4]),"clicked",GTK_SIGNAL_FUNC(plansh_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK4]),(gpointer)PFK4);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Поиск не выполнять"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(plansh_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

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
void  iceb_l_plansh_p_e_knopka(GtkWidget *widget,class iceb_l_plansh_p *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_KOD_SUBBAL:
    if(iceb_l_subbal(1,&kod,&naim,data->window) == 0)
     data->rk->kod_subbal.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_SUBBAL]),data->rk->kod_subbal.ravno_toutf());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   plansh_p_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_plansh_p *data)
{
//char  bros[512];

//printf("vplansh_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK2]),"clicked");
    return(FALSE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK4]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK10]),"clicked");
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
void  plansh_p_knopka(GtkWidget *widget,class iceb_l_plansh_p *data)
{
//char bros[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
/*g_print("plansh_p_knopka knop=%d\n",*knop);*/

switch (knop)
 {
  case PFK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case PFK4:
    data->clear_data();
    return;  


  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    plansh_p_vvod(GtkWidget *widget,class iceb_l_plansh_p *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("plansh_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SHET:
    data->rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIM:
    data->rk->naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOD_SUBBAL:
    data->rk->kod_subbal.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }
enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
