/*$Id: l_sptbn1_p.c,v 1.8 2011-02-21 07:35:53 sasa Exp $*/
/*16.06.2008	16.06.2008	Белых А.И.	l_sptbn1_p.c
Меню для ввода реквизитов поиска
*/
#include "buhg_g.h"
#include "l_sptbn.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_FIO,
  E_INN,
  E_METKA_NKS,
  KOLENTER  
 };

class l_sptbn1_p_data
 {
  public:
  class l_sptbn_p *rpoi;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *opt;
  short kl_shift;
  
  
  l_sptbn1_p_data() //Конструктор
   {
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
    rpoi->clear_rek();
   }
 };

gboolean   l_sptbn1_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_sptbn1_p_data *data);
void    l_sptbn1_p_vvod(GtkWidget *widget,class l_sptbn1_p_data *data);
void  l_sptbn1_p_knopka(GtkWidget *widget,class l_sptbn1_p_data *data);

extern char *name_system;
extern SQL_baza bd;

int l_sptbn1_p(class l_sptbn_p *datap,GtkWidget *wpredok)
{
l_sptbn1_p_data data;
data.rpoi=datap;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Поиск"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_sptbn1_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Поиск"));

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

sprintf(strsql,"%s",gettext("Фамилия"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_FIO] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FIO]), "activate",GTK_SIGNAL_FUNC(l_sptbn1_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rpoi->fio.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FIO]),(gpointer)E_FIO);

sprintf(strsql,"%s",gettext("Идентификационный номер"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_INN] = gtk_entry_new_with_max_length (60);
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.entry[E_INN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_INN]), "activate",GTK_SIGNAL_FUNC(l_sptbn1_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INN]),data.rpoi->inn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_INN]),(gpointer)E_INN);

sprintf(strsql,"%s (+/-)",gettext("Наличие карт-счёта"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_METKA_NKS] = gtk_entry_new_with_max_length (1);
gtk_box_pack_start (GTK_BOX (hbox[E_METKA_NKS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_METKA_NKS]), data.entry[E_METKA_NKS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_METKA_NKS]), "activate",GTK_SIGNAL_FUNC(l_sptbn1_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_METKA_NKS]),data.rpoi->inn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_METKA_NKS]),(gpointer)E_METKA_NKS);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_sptbn1_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(l_sptbn1_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_sptbn1_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();
//printf("l_l_sptbn1_p-metka_poi=%d\n",data.rpoi->metka_poi);

return(0);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_sptbn1_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_sptbn1_p_data *data)
{

//printf("l_sptbn1_p_key_press\n");
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
void  l_sptbn1_p_knopka(GtkWidget *widget,class l_sptbn1_p_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->rpoi->metka_poi=1;
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->rpoi->metka_poi=0;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_sptbn1_p_vvod(GtkWidget *widget,class l_sptbn1_p_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_sptbn1_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_FIO:
    data->rpoi->fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_INN:
    data->rpoi->inn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_METKA_NKS:
    data->rpoi->metka_nks.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;

if(enter >= KOLENTER)
enter=0;


gtk_widget_grab_focus(data->entry[enter]);
 
}
