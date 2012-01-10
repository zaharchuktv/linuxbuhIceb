/*$Id: l_ukrspdok_p.c,v 1.8 2011-02-21 07:35:53 sasa Exp $*/
/*11.03.2008	21.02.2008	Белых А.И.	l_ukrspdok_p.c
Ввод реквизитов поиска документов
*/
#include "buhg_g.h"
#include "l_ukrspdok.h"

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
  E_KONTR,
  E_VIDKOM,
  E_NOMER_PRIK,
  E_DATA_PRIK,
  E_NOMER_AO,
  E_DATA_AO,
  KOLENTER  
 };

class ukrspdok_p_data
 {
  public:
  class ukrspdok_rek  *rpoi;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  short kl_shift;
  
  
  ukrspdok_p_data() //Конструктор
   {
    kl_shift=0;
  
   }

  void read_rek()
   {
    rpoi->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK]))));
    rpoi->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN]))));
    rpoi->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK]))));
    rpoi->kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR]))));
    rpoi->vidkom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VIDKOM]))));
    rpoi->nomer_prik.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMER_PRIK]))));
    rpoi->data_prik.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_PRIK]))));
    rpoi->nomer_ao.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMER_AO]))));
    rpoi->data_ao.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_AO]))));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rpoi->clear();
   }
 };

gboolean   ukrspdok_p_key_press(GtkWidget *widget,GdkEventKey *event,class ukrspdok_p_data *data);
void    ukrspdok_p_vvod(GtkWidget *widget,class ukrspdok_p_data *data);
void  ukrspdok_p_knopka(GtkWidget *widget,class ukrspdok_p_data *data);

void  ukrspdok_v_e_knopka(GtkWidget *widget,class ukrspdok_p_data *data);

extern char *name_system;
extern SQL_baza bd;

int l_ukrspdok_p(class ukrspdok_rek *datap,GtkWidget *wpredok)
{
ukrspdok_p_data data;
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(ukrspdok_p_key_press),&data);

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
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rpoi->nomdok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(ukrspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rpoi->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);


sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(ukrspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rpoi->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KONTR]),"clicked",GTK_SIGNAL_FUNC(ukrspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KONTR]),(gpointer)E_KONTR);
tooltips_enter[E_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KONTR],data.knopka_enter[E_KONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rpoi->kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);

sprintf(strsql,"%s (,,)",gettext("Вид командировки"));
data.knopka_enter[E_VIDKOM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_VIDKOM]), data.knopka_enter[E_VIDKOM], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_VIDKOM]),"clicked",GTK_SIGNAL_FUNC(ukrspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_VIDKOM]),(gpointer)E_VIDKOM);
tooltips_enter[E_VIDKOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_VIDKOM],data.knopka_enter[E_VIDKOM],gettext("Выбор вида командировки"),NULL);

data.entry[E_VIDKOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_VIDKOM]), data.entry[E_VIDKOM], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VIDKOM]), "activate",GTK_SIGNAL_FUNC(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VIDKOM]),data.rpoi->vidkom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VIDKOM]),(gpointer)E_VIDKOM);


sprintf(strsql,"%s (,,)",gettext("Номер приказа"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_PRIK]), label, FALSE, FALSE,1);

data.entry[E_NOMER_PRIK] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_PRIK]), data.entry[E_NOMER_PRIK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMER_PRIK]), "activate",GTK_SIGNAL_FUNC(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_PRIK]),data.rpoi->nomer_prik.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMER_PRIK]),(gpointer)E_NOMER_PRIK);


sprintf(strsql,"%s",gettext("Дата приказа"));
data.knopka_enter[E_DATA_PRIK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PRIK]), data.knopka_enter[E_DATA_PRIK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_PRIK]),"clicked",GTK_SIGNAL_FUNC(ukrspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_PRIK]),(gpointer)E_DATA_PRIK);
tooltips_enter[E_DATA_PRIK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_PRIK],data.knopka_enter[E_DATA_PRIK],gettext("Выбор даты"),NULL);

data.entry[E_DATA_PRIK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PRIK]), data.entry[E_DATA_PRIK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_PRIK]), "activate",GTK_SIGNAL_FUNC(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_PRIK]),data.rpoi->data_prik.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_PRIK]),(gpointer)E_DATA_PRIK);


sprintf(strsql,"%s (,,)",gettext("Номер авансового отчёта"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_AO]), label, FALSE, FALSE,1);

data.entry[E_NOMER_AO] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_AO]), data.entry[E_NOMER_AO], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMER_AO]), "activate",GTK_SIGNAL_FUNC(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_AO]),data.rpoi->nomer_ao.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMER_AO]),(gpointer)E_NOMER_AO);


sprintf(strsql,"%s",gettext("Дата авансового отчёта"));
data.knopka_enter[E_DATA_AO]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_AO]), data.knopka_enter[E_DATA_AO], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_AO]),"clicked",GTK_SIGNAL_FUNC(ukrspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_AO]),(gpointer)E_DATA_AO);
tooltips_enter[E_DATA_AO]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_AO],data.knopka_enter[E_DATA_AO],gettext("Выбор даты"),NULL);

data.entry[E_DATA_AO] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_AO]), data.entry[E_DATA_AO], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_AO]), "activate",GTK_SIGNAL_FUNC(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_AO]),data.rpoi->data_ao.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_AO]),(gpointer)E_DATA_AO);





GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(ukrspdok_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(ukrspdok_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(ukrspdok_p_knopka),&data);
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
void  ukrspdok_v_e_knopka(GtkWidget *widget,class ukrspdok_p_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("ukrspdok_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rpoi->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rpoi->datan.ravno());
      
    return;  

  case E_DATA_PRIK:

    if(iceb_calendar(&data->rpoi->data_prik,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_PRIK]),data->rpoi->data_prik.ravno());
      
    return;  

  case E_DATA_AO:

    if(iceb_calendar(&data->rpoi->data_ao,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_AO]),data->rpoi->data_ao.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rpoi->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rpoi->datak.ravno());
      
    return;  

  case E_KONTR:

    iceb_vibrek(0,"Kontragent",&data->rpoi->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rpoi->kontr.ravno_toutf());

    return;  


  case E_VIDKOM:
    if(l_ukrvk(1,&kod,&naim,data->window) == 0)
      data->rpoi->vidkom.z_plus(kod.ravno());    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VIDKOM]),data->rpoi->vidkom.ravno_toutf());
    return;  
    
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ukrspdok_p_key_press(GtkWidget *widget,GdkEventKey *event,class ukrspdok_p_data *data)
{

//printf("ukrspdok_p_key_press\n");
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
void  ukrspdok_p_knopka(GtkWidget *widget,class ukrspdok_p_data *data)
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

    if(data->rpoi->data_prik.getdlinna() > 1)
     if(data->rpoi->data_prik.prov_dat() != 0)
      {
       iceb_menu_soob(gettext("Не правильно введена дата приказа!"),data->window);
       return;
      }     

    if(data->rpoi->data_ao.getdlinna() > 1)
     if(data->rpoi->data_ao.prov_dat() != 0)
      {
       iceb_menu_soob(gettext("Не правильно введена дата авансового отчёта!"),data->window);
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

void    ukrspdok_p_vvod(GtkWidget *widget,class ukrspdok_p_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("ukrspdok_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_NOMDOK:
    data->rpoi->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMER_PRIK:
    data->rpoi->nomer_prik.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  
  case E_NOMER_AO:
    data->rpoi->nomer_ao.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAN:
    data->rpoi->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATA_PRIK:
    data->rpoi->data_prik.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATA_AO:
    data->rpoi->data_ao.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->rpoi->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KONTR:
    data->rpoi->kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VIDKOM:
    data->rpoi->vidkom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;

if(enter >= KOLENTER)
enter=0;


gtk_widget_grab_focus(data->entry[enter]);
 
}
