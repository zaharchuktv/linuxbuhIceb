/*$Id: iceb_l_opmup_p.c,v 1.11 2011-01-13 13:49:59 sasa Exp $*/
/*29.02.2008	05.05.2004	Белых А.И.	iceb_l_opmup_p.c
Меню для ввода реквизитов поиска 
*/
#include "iceb_libbuh.h"
#include "iceb_l_opmup.h"

class iceb_l_opmup_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class opmup_rek *rk;
  
  iceb_l_opmup_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
     }
   }
 };

void opmup_p_clear(class iceb_l_opmup_p_data *data);
void    opmup_p_vvod(GtkWidget *widget,class iceb_l_opmup_p_data *data);
void  opmup_p_knopka(GtkWidget *widget,class iceb_l_opmup_p_data *data);
gboolean   opmup_p_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_opmup_p_data *data);

int iceb_l_opmup_p(class opmup_rek *rek_poi,GtkWidget *wpredok)
{
class iceb_l_opmup_p_data data;
char strsql[512];
data.rk=rek_poi;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(opmup_p_key_press),&data);

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
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(opmup_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk->kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new_with_max_length (80);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_NAIM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(opmup_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk->naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);





//GtkWidget *knopka[KOL_PFK];
GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(opmup_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[PFK4]=gtk_button_new_with_label(strsql);
tooltips[PFK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK4],data.knopka[PFK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK4]),"clicked",GTK_SIGNAL_FUNC(opmup_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK4]),(gpointer)PFK4);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Поиск не выполнять"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(opmup_p_knopka),&data);
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
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   opmup_p_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_opmup_p_data *data)
{
//char  bros[512];

//printf("vopmup_key_press\n");
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
void  opmup_p_knopka(GtkWidget *widget,class iceb_l_opmup_p_data *data)
{
//char bros[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("opmup_p_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case PFK4:
    opmup_p_clear(data);
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

void    opmup_p_vvod(GtkWidget *widget,class iceb_l_opmup_p_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("opmup_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->rk->kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIM:
    data->rk->naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }
enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Очистка меню от введенной информации*/
/**************************************/
void opmup_p_clear(class iceb_l_opmup_p_data *data)
{

data->rk->clear_zero();
for(int i=0; i< KOLENTER; i++)
 gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");

gtk_widget_grab_focus(data->entry[0]);

}
