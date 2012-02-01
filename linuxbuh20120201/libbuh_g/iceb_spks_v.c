/*$Id: iceb_spks_v.c,v 1.12 2011-02-21 07:36:08 sasa Exp $*/
/*17.01.2008	18.12.2003	Белых А.И.	iceb_spks_v.c
Меню для расчета сальдо по контрагенту и всем счетам
*/
#include  "iceb_libbuh.h"
#include  "iceb_spks.h"

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

gboolean   iceb_spks_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_spks_data *data);
void  iceb_spks_v_knopka(GtkWidget *widget,class iceb_spks_data *data);
void    iceb_spks_v_vvod(GtkWidget *widget,class iceb_spks_data *data);
void iceb_spks_v_clear(class iceb_spks_data *data);
int    iceb_spks_v_prov(class iceb_spks_data *data);
void  iceb_spks_v_radio0(GtkWidget *widget,class iceb_spks_data *data);
void  iceb_spks_v_radio1(GtkWidget *widget,class iceb_spks_data *data);

extern char *name_system;

int     iceb_spks_v(class iceb_spks_data *data,
iceb_u_str *zag,   //Заголовок меню
GtkWidget *window) //Окно на фоне которого появляется наше меню
{
char strsql[512];


data->window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal( GTK_WINDOW(data->window) ,TRUE ); 
gtk_window_set_position( GTK_WINDOW(data->window),ICEB_POS_CENTER);
gtk_window_set_modal( GTK_WINDOW(data->window) ,TRUE ); 

sprintf(strsql,"%s %s",name_system,gettext("Расчет сальдо по контрагенту."));
gtk_window_set_title(GTK_WINDOW(data->window),strsql);
gtk_signal_connect(GTK_OBJECT(data->window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data->window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data->window),"key_press_event",GTK_SIGNAL_FUNC(iceb_spks_v_key_press),data);

if(window != NULL)
 { 
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data->window),GTK_WINDOW(window));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data->window),TRUE);
 }

GtkWidget *label=gtk_label_new(iceb_u_toutf(zag->ravno()));

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox2 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox4 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox_rb = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data->window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox_rb);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Дата начала"));
data->entry[E_DATAN] = gtk_entry_new_with_max_length (sizeof(data->datn)-1);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data->entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data->entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(iceb_spks_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),iceb_u_toutf(data->datn));
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_DATAN]),(gpointer)E_DATAN);

label=gtk_label_new(gettext("Дата конца"));
data->entry[E_DATAK] = gtk_entry_new_with_max_length (sizeof(data->datk)-1);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data->entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data->entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(iceb_spks_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),iceb_u_toutf(data->datk));
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_DATAK]),(gpointer)E_DATAK);

sprintf(strsql,"%s",gettext("Счет для сверки"));

label=gtk_label_new(strsql);
data->entry[E_SHETA] = gtk_entry_new_with_max_length (sizeof(data->shets)-1);
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox3), data->entry[E_SHETA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data->entry[E_SHETA]), "activate",GTK_SIGNAL_FUNC(iceb_spks_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETA]),iceb_u_toutf(data->shets));
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_SHETA]),(gpointer)E_SHETA);



GtkWidget *knopka[KOL_PFK];
GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Расчет"));
knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],knopka[PFK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(iceb_spks_v_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F4 %s",gettext("Очистить"));
knopka[PFK4]=gtk_button_new_with_label(strsql);
tooltips[PFK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK4],knopka[PFK4],gettext("Очистить меню от введенной информации."),NULL);
gtk_signal_connect(GTK_OBJECT(knopka[PFK4]),"clicked",GTK_SIGNAL_FUNC(iceb_spks_v_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(knopka[PFK4]),(gpointer)PFK4);

sprintf(strsql,"F10 %s",gettext("Выход."));
knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],knopka[PFK10],gettext("Выйти из меню без получения отчета."),NULL);
gtk_signal_connect(GTK_OBJECT(knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(iceb_spks_v_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), knopka[PFK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_show_all(data->window);
gtk_main();

return(data->metka_voz);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_spks_v_knopka(GtkWidget *widget,class iceb_spks_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
/*g_print("iceb_spks_v_knopka knop=%d\n",*knop);*/

switch (knop)
 {
  case PFK2:
    if(iceb_spks_v_prov(data) == 0)
      data->metka_voz=1;

    return;  

  case PFK4:
    iceb_spks_v_clear(data);
    return;
   
  case PFK10:
    gtk_widget_destroy(data->window);
    data->metka_voz=0;
    return;
 }
}
 
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_spks_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_spks_data *data)
{
//char  bros[512];

//printf("vzaktaxi_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    if(iceb_spks_v_prov(data) == 0)
      data->metka_voz=1;
    return(TRUE);
    
  case GDK_F4:
    iceb_spks_v_clear(data);
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_widget_destroy(data->window);
    data->metka_voz=0;

    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

//    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    iceb_spks_v_vvod(GtkWidget *widget,class iceb_spks_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kontr_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    strncpy(data->datn,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))),sizeof(data->datn)-1);
    break;

  case E_DATAK:
    strncpy(data->datk,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))),sizeof(data->datk)-1);
    break;

  case E_SHETA:
    strncpy(data->shets,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))),sizeof(data->shets)-1);
    break;

 }

enter++;
if(enter >= KOLENTER)
 enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/************************************/
/*Чтение введенной информации и проверка на ошибки*/
/**************************************************/
//Если вернули 0 - все впорядке

int    iceb_spks_v_prov(class iceb_spks_data *data)
{
for(int i=0; i < KOLENTER; i++)
  gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->datn,&dk,&mk,&gk,data->datk,data->window) != 0)
  return(1);

gtk_widget_destroy(data->window);

return(0);

}
/****************************/
/*Очистка меню              */
/*****************************/

void iceb_spks_v_clear(class iceb_spks_data *data)
{

data->clear_zero();

gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),iceb_u_toutf(data->datn));
gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),iceb_u_toutf(data->datk));
gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETA]),iceb_u_toutf(data->shets));

gtk_widget_grab_focus(data->entry[0]);

}
