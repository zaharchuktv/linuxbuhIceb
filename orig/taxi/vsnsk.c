/*$Id: vsnsk.c,v 1.8 2011-02-21 07:36:21 sasa Exp $*/
/*09.02.2006	09.02.2006	Белых А.И.	vsnsk.c
Ввод суммы на счёт клиента (безналичный)
*/
#include "i_rest.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SUMA,
  E_KOMENT,
  KOLENTER  
 };

class vsnsk_vs_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_suma;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  iceb_u_str suma;
  iceb_u_str koment;
  iceb_u_str kodkl;
  
  vsnsk_vs_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    suma.new_plus("");
    kodkl.new_plus("");        
    koment.plus("");
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER-1; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    suma.new_plus("");
    gtk_widget_grab_focus(entry[0]);
   
   }
 };


gboolean   vsnsk_vs_v_key_press(GtkWidget *widget,GdkEventKey *event,class vsnsk_vs_data *data);
void    vsnsk_vs_v_vvod(GtkWidget *widget,class vsnsk_vs_data *data);
void  vsnsk_vs_v_knopka(GtkWidget *widget,class vsnsk_vs_data *data);
void  vsnsk_v_e_knopka(GtkWidget *widget,class vsnsk_vs_data *data);


extern char *name_system;
extern short metka_skreen; //0-сенсорный экран не используется 1-используется
extern int   nomer_kas;  //номер кассы
extern uid_t           kod_operatora;

int vsnsk()
{
class vsnsk_vs_data data;


if(iceb_mous_klav(gettext("Введите код клиента"),&data.kodkl,20,0,0,1,NULL) != 0)
 return(1);
//Проверяем код клиента
char strsql[300];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select fio from Taxiklient where kod='%s'",data.kodkl.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,NULL) != 1)
 {
  iceb_menu_soob(gettext("Не найдено код клиента !"),NULL);
  return(1);
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(metka_skreen == 1)
  gtk_widget_set_usize(data.window,-1,400);

iceb_u_str repl;
repl.plus(gettext("Ввод сумы на счёт клиента"));
sprintf(strsql,"%s %s",name_system,gettext("Ввод сумы на счёт клиента"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vsnsk_vs_v_key_press),&data);

GtkWidget *label=NULL;


repl.ps_plus(data.kodkl.ravno());
repl.plus(" ");
repl.plus(row[0]);
label=gtk_label_new(repl.ravno_toutf());

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop=gtk_hbox_new (FALSE, 0);


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

data.knopka_suma=gtk_button_new_with_label(gettext("Сумма"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.knopka_suma, FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_suma),"clicked",GTK_SIGNAL_FUNC(vsnsk_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_suma),(gpointer)E_SUMA);


data.entry[E_SUMA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(vsnsk_vs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);

data.knopka_suma=gtk_button_new_with_label(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.knopka_suma, FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_suma),"clicked",GTK_SIGNAL_FUNC(vsnsk_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_suma),(gpointer)E_KOMENT);


data.entry[E_KOMENT] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(vsnsk_vs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод суммы в кассовый регистратор"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(vsnsk_vs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(vsnsk_vs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(vsnsk_vs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vsnsk_vs_v_key_press(GtkWidget *widget,GdkEventKey *event,class vsnsk_vs_data *data)
{

//printf("vsnsk_vs_v_key_press\n");
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
void  vsnsk_vs_v_knopka(GtkWidget *widget,class vsnsk_vs_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
//    printf("%s %d\n",data->suma.ravno(),data->vidnal);     

    zap_v_kas(nomer_kas,data->kodkl.ravno(),data->suma.ravno_atof(),1,"",0,0,0,0,kod_operatora,data->koment.ravno());

    
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vsnsk_vs_v_vvod(GtkWidget *widget,class vsnsk_vs_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//printf("vsnsk_vs_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SUMA:
    data->suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOMENT:
    data->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }
enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);

}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vsnsk_v_e_knopka(GtkWidget *widget,class vsnsk_vs_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_SUMA:

    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_SUMA]),"activate");

    iceb_mous_klav(gettext("Сумма"),&data->suma,20,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA]),data->suma.ravno_toutf());

    return;  

  case E_KOMENT:

    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_KOMENT]),"activate");

    iceb_mous_klav(gettext("Введите коментарий"),&data->koment,20,1,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOMENT]),data->koment.ravno_toutf());

    return;  


 }
}


