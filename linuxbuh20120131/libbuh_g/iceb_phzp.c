/*$Id: iceb_phzp.c,v 1.10 2011-02-21 07:36:07 sasa Exp $*/
/*17.01.2008	23.12.2004	Белых А.И.	iceb_phzp.c
Печать отчёта с кассового регистратора за период
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
  E_DATAN,
  E_DATAK,
  KOLENTER  
 };

class iceb_phzp_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  iceb_u_str datan;
  iceb_u_str datak;
  struct KASSA *kasr;
  iceb_phzp_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    datan.new_plus("");
    datak.new_plus("");
    
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
    datan.new_plus("");
    datak.new_plus("");
   }
 };


gboolean   iceb_phzp_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_phzp_data *data);
void    iceb_phzp_v_vvod(GtkWidget *widget,class iceb_phzp_data *data);
void  iceb_phzp_v_knopka(GtkWidget *widget,class iceb_phzp_data *data);

extern char *name_system;
//extern SQL_baza bd;

int iceb_phzp(struct KASSA *kasr)
{


class iceb_phzp_data data;
data.kasr=kasr;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Распечетать отчётный чек за период"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_phzp_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Распечетать отчётный чек за период"));

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



label=gtk_label_new(gettext("Дата начала"));
data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(iceb_phzp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

label=gtk_label_new(gettext("Дата конца"));
data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(iceb_phzp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(iceb_phzp_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(iceb_phzp_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(iceb_phzp_v_knopka),&data);
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

gboolean   iceb_phzp_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_phzp_data *data)
{

//printf("iceb_phzp_v_key_press\n");
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
void  iceb_phzp_v_knopka(GtkWidget *widget,class iceb_phzp_data *data)
{
char strsql[512];
short dn,mn,gn;
short dk,mk,gk;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
/*g_print("iceb_phzp_v_knopka knop=%d\n",*knop);*/

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(iceb_rsdatp(&dn,&mn,&gn,data->datan.ravno(),&dk,&mk,&gk,data->datak.ravno(),data->window) != 0)
     return;
     

    sprintf(strsql,"FSTART: MARIA_DATE_REPORT\n\
%s\n\
%s\n\
%d\n\
%d\n\
%d\n\
%d\n\
%d\n\
%d\n\
FEND:\n",
    data->kasr->prodavec.ravno(), data->kasr->parol, dn, mn, gn, dk, mk, gk);  

    iceb_cmd(data->kasr, strsql,data->window);






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

void    iceb_phzp_v_vvod(GtkWidget *widget,class iceb_phzp_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("iceb_phzp_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter++;
  
if(enter >= KOLENTER)
 enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
