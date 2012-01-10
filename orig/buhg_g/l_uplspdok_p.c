/*$Id: l_uplspdok_p.c,v 1.7 2011-02-21 07:35:54 sasa Exp $*/
/*11.03.2008	21.02.2008	Белых А.И.	l_uplspdok_p.c
Ввод реквизитов поиска документов
*/
#include "buhg_g.h"
#include "l_uplspdok.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_NOMDOK,
  E_DATAN,
  E_DATAK,
  E_KOD_POD,
  E_KOD_VOD,
  E_KOD_AVT,
  KOLENTER  
 };

class uplspdok_p_data
 {
  public:
  class uplspdok_rek  *rpoi;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  short kl_shift;
  
  
  uplspdok_p_data() //Конструктор
   {
    kl_shift=0;
  
   }

  void read_rek()
   {
    rpoi->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK]))));
    rpoi->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN]))));
    rpoi->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK]))));
    rpoi->kod_pod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_POD]))));
    rpoi->kod_vod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_VOD]))));
    rpoi->kod_avt.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_AVT]))));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rpoi->clear();
   }
 };

gboolean   uplspdok_p_key_press(GtkWidget *widget,GdkEventKey *event,class uplspdok_p_data *data);
void    uplspdok_p_vvod(GtkWidget *widget,class uplspdok_p_data *data);
void  uplspdok_p_knopka(GtkWidget *widget,class uplspdok_p_data *data);

void  uplspdok_v_e_knopka(GtkWidget *widget,class uplspdok_p_data *data);

extern char *name_system;
extern SQL_baza bd;

int l_uplspdok_p(class uplspdok_rek *datap,GtkWidget *wpredok)
{
uplspdok_p_data data;
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uplspdok_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Поиск"));

GtkWidget *vbox = gtk_vbox_new (FALSE,1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE,1);

GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Номер документа"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE,1);

data.entry[E_NOMDOK] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(uplspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rpoi->nomdok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(uplspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(uplspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rpoi->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);


sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(uplspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(uplspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rpoi->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);

sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_KOD_POD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POD]), data.knopka_enter[E_KOD_POD], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_POD]),"clicked",GTK_SIGNAL_FUNC(uplspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_POD]),(gpointer)E_KOD_POD);
tooltips_enter[E_KOD_POD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_POD],data.knopka_enter[E_KOD_POD],gettext("Выбор подразделения"),NULL);

data.entry[E_KOD_POD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POD]), data.entry[E_KOD_POD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_POD]), "activate",GTK_SIGNAL_FUNC(uplspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_POD]),data.rpoi->kod_pod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_POD]),(gpointer)E_KOD_POD);

sprintf(strsql,"%s (,,)",gettext("Код водителя"));
data.knopka_enter[E_KOD_VOD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.knopka_enter[E_KOD_VOD], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_VOD]),"clicked",GTK_SIGNAL_FUNC(uplspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_VOD]),(gpointer)E_KOD_VOD);
tooltips_enter[E_KOD_VOD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_VOD],data.knopka_enter[E_KOD_VOD],gettext("Выбор водителя"),NULL);

data.entry[E_KOD_VOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.entry[E_KOD_VOD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_VOD]), "activate",GTK_SIGNAL_FUNC(uplspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_VOD]),data.rpoi->kod_vod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_VOD]),(gpointer)E_KOD_VOD);



sprintf(strsql,"%s",gettext("Код автомобиля"));
data.knopka_enter[E_KOD_AVT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.knopka_enter[E_KOD_AVT], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_AVT]),"clicked",GTK_SIGNAL_FUNC(uplspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_AVT]),(gpointer)E_KOD_AVT);
tooltips_enter[E_KOD_AVT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_AVT],data.knopka_enter[E_KOD_AVT],gettext("Выбор автомобиля"),NULL);

data.entry[E_KOD_AVT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.entry[E_KOD_AVT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_AVT]), "activate",GTK_SIGNAL_FUNC(uplspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_AVT]),data.rpoi->kod_avt.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_AVT]),(gpointer)E_KOD_AVT);




GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(uplspdok_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(uplspdok_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(uplspdok_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(0);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uplspdok_v_e_knopka(GtkWidget *widget,class uplspdok_p_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("uplspdok_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rpoi->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rpoi->datan.ravno());
      
    return;  


  case E_DATAK:

    if(iceb_calendar(&data->rpoi->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rpoi->datak.ravno());
      
    return;  

  case E_KOD_POD:
    if(l_uplpod(1,&kod,&naim,data->window) == 0)
     data->rpoi->kod_pod.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_POD]),data->rpoi->kod_pod.ravno_toutf());

    return;  


  case E_KOD_VOD:
    if(l_uplout(1,&kod,&naim,1,data->window) == 0)
      data->rpoi->kod_vod.z_plus(kod.ravno());    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VOD]),data->rpoi->kod_vod.ravno_toutf());
    return;  

  case E_KOD_AVT:
    if(l_uplavt(1,&kod,&naim,data->window) == 0)
      data->rpoi->kod_avt.z_plus(kod.ravno());    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_AVT]),data->rpoi->kod_avt.ravno_toutf());
    return;  
    
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uplspdok_p_key_press(GtkWidget *widget,GdkEventKey *event,class uplspdok_p_data *data)
{

//printf("uplspdok_p_key_press\n");
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
void  uplspdok_p_knopka(GtkWidget *widget,class uplspdok_p_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(data->rpoi->datan.getdlinna() > 1)
     if(data->rpoi->datan.prov_dat() != 0)
      {
       iceb_menu_soob(gettext("Не правильно введена дата начала !"),data->window);
       return;
      }     

    if(data->rpoi->datak.getdlinna() > 1)
     if(data->rpoi->datak.prov_dat() != 0)
      {
       iceb_menu_soob(gettext("Не правильно введена дата конца !"),data->window);
       return;
      }     

    data->rpoi->metka_poi=1;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    data->rpoi->metka_poi=0;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    uplspdok_p_vvod(GtkWidget *widget,class uplspdok_p_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("uplspdok_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_NOMDOK:
    data->rpoi->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOD_AVT:
    data->rpoi->kod_avt.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  

  case E_DATAN:
    data->rpoi->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->rpoi->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOD_POD:
    data->rpoi->kod_pod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOD_VOD:
    data->rpoi->kod_vod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;

if(enter >= KOLENTER)
enter=0;


gtk_widget_grab_focus(data->entry[enter]);
 
}
