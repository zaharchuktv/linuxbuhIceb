/*$Id: iceb_mariq_vs.c,v 1.10 2011-02-21 07:36:07 sasa Exp $*/
/*17.01.2008	23.12.2004	Белых А.И.	iceb_mariq_vs.c
Ввод суммы в кассовый регистратор
*/
#include "iceb_libbuh.h"

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
  E_VIDNAL,
  KOLENTER  
 };

class iceb_mariq_vs_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  iceb_u_str suma;
  iceb_u_str datak;
  struct KASSA *kasr;
  short metka;  //0-внесение денег в кассу 1-выдача денег из кассы
  int vidnal; //0-c ПДВ 1-без ПДВ
  
  iceb_mariq_vs_data() //Конструктор
   {
    vidnal=0;  
    kl_shift=0;
    voz=1;
    suma.new_plus("");
    datak.new_plus("");
    
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER-1; i++)
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


gboolean   iceb_mariq_vs_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_mariq_vs_data *data);
void    iceb_mariq_vs_v_vvod(GtkWidget *widget,class iceb_mariq_vs_data *data);
void  iceb_mariq_vs_v_knopka(GtkWidget *widget,class iceb_mariq_vs_data *data);

GtkWidget *vidnal(int *nomerstr);

extern char *name_system;

int iceb_mariq_vs(int metka, //0-внесение денег в кассу 1-выдача из кассы денег 
struct KASSA *kasr)
{

class iceb_mariq_vs_data data;
data.kasr=kasr;
data.metka=metka;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(metka == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Выдача из кассы клиенту."));
if(metka == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Внесение в кассу клиентом."));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_mariq_vs_v_key_press),&data);

GtkWidget *label=NULL;
if(metka == 1)
 label=gtk_label_new(gettext("Выдача из кассы клиенту."));
if(metka == 0)
 label=gtk_label_new(gettext("Внесение в кассу клиентом."));

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



label=gtk_label_new(gettext("Сумма"));
data.entry[E_SUMA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(iceb_mariq_vs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);

label=gtk_label_new(gettext("Нaлог"));
gtk_box_pack_start (GTK_BOX (hbox[E_VIDNAL]), label, FALSE, FALSE, 0);

GtkWidget *menu_vidnal = vidnal(&data.vidnal);
gtk_box_pack_start (GTK_BOX (hbox[E_VIDNAL]), menu_vidnal, TRUE, TRUE, 0);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод суммы в кассовый регистратор"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(iceb_mariq_vs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(iceb_mariq_vs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(iceb_mariq_vs_v_knopka),&data);
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

gboolean   iceb_mariq_vs_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_mariq_vs_data *data)
{

//printf("iceb_mariq_vs_v_key_press\n");
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
void  iceb_mariq_vs_v_knopka(GtkWidget *widget,class iceb_mariq_vs_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
/*g_print("iceb_mariq_vs_v_knopka knop=%d\n",*knop);*/

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
//    printf("%s %d\n",data->suma.ravno(),data->vidnal);     
    if(iceb_mariq_zvs(data->metka,data->suma.ravno_atof(),data->vidnal,data->kasr,data->window) != 0)
     return;

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

void    iceb_mariq_vs_v_vvod(GtkWidget *widget,class iceb_mariq_vs_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//printf("iceb_mariq_vs_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SUMA:
    data->suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }
/****************
enter++;
  
if(enter >= KOLENTER)
 enter=0;

gtk_widget_grab_focus(data->entry[enter]);
******************/
}

/*****************************/
/*Меню выбора налога     */
/*****************************/

GtkWidget *vidnal(int *nomerstr)
{
int nomer=0;
GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();

GtkWidget *item;
item = gtk_menu_item_new_with_label (iceb_u_toutf(gettext("С НДС")));
gtk_signal_connect (GTK_OBJECT (item), "activate",GTK_SIGNAL_FUNC(iceb_get_menu), nomerstr);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);

gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (iceb_u_toutf(gettext("Без НДС")));
gtk_signal_connect (GTK_OBJECT (item), "activate",GTK_SIGNAL_FUNC(iceb_get_menu), nomerstr);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);

gtk_option_menu_set_history(GTK_OPTION_MENU(opt),*nomerstr);
return(opt);
}
