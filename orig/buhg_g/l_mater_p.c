/*$Id: l_mater_p.c,v 1.16 2011-01-13 13:49:50 sasa Exp $*/
/*05.05.2004	05.05.2004	Белых А.И.	l_mater_p.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "l_mater.h"
enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

enum
 {
  E_KOD,
  E_GRUP,
  E_NAIM,
  E_CENA,
  E_MCENA,
  E_EI,
  E_KRAT,
  E_KODTAR,
  E_FAS,
  E_HTRIX,
  E_ARTIK,
  E_KRIOST,
  KOLENTER  
 };

class  mater_data_poi
 {
  public:
  mater_rek *poisk;

  GtkWidget *window;
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_pv[KOL_PFK];
  short     kl_shift; //0-отжата 1-нажата  
 
 };
 
void mater_p_clear(class mater_data_poi *data);
void    mater_p_vvod(GtkWidget *widget,class mater_data_poi *data);
void  mater_p_knopka(GtkWidget *widget,class mater_data_poi *data);
gboolean   mater_p_key_press(GtkWidget *widget,GdkEventKey *event,class mater_data_poi *data);

void l_mater_p(class  mater_rek *data_poi,GtkWidget *wpredok)
{
char strsql[512];
class  mater_data_poi data;
data.poisk=data_poi;

data.kl_shift=0;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(mater_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }



GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.poisk->kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

sprintf(strsql,"%s (,,)",gettext("Код группы"));
label=gtk_label_new(strsql);
data.entry[E_GRUP] = gtk_entry_new_with_max_length (80);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP]), data.entry[E_GRUP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUP]), "activate",GTK_SIGNAL_FUNC(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP]),data.poisk->grupa.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUP]),(gpointer)E_GRUP);

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new_with_max_length (80);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.poisk->naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

label=gtk_label_new(gettext("Цена"));
data.entry[E_CENA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENA]), "activate",GTK_SIGNAL_FUNC(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.poisk->cena.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENA]),(gpointer)E_CENA);


label=gtk_label_new(gettext("Цена введена"));
gtk_box_pack_start (GTK_BOX (hbox[E_MCENA]), label, FALSE, FALSE, 0);

GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();

GtkWidget *item;
item = gtk_menu_item_new_with_label (" ");
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(mater_get_pm0),&data.poisk->mcena);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("С НДС"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(mater_get_pm1),&data.poisk->mcena);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Без НДС"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(mater_get_pm2),&data.poisk->mcena);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);

gtk_option_menu_set_history(GTK_OPTION_MENU(opt),data.poisk->mcena);

gtk_box_pack_start (GTK_BOX (hbox[E_MCENA]), opt, FALSE, FALSE, 0);



sprintf(strsql,"%s (,,)",gettext("Единица измерения"));
label=gtk_label_new(strsql);
data.entry[E_EI] = gtk_entry_new_with_max_length (80);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_EI]), "activate",GTK_SIGNAL_FUNC(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.poisk->ei.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_EI]),(gpointer)E_EI);

label=gtk_label_new(gettext("Кратность"));
data.entry[E_KRAT] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KRAT]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KRAT]), data.entry[E_KRAT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KRAT]), "activate",GTK_SIGNAL_FUNC(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KRAT]),data.poisk->krat.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KRAT]),(gpointer)E_KRAT);

label=gtk_label_new(gettext("Код тары"));
data.entry[E_KODTAR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTAR]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTAR]), data.entry[E_KODTAR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODTAR]), "activate",GTK_SIGNAL_FUNC(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODTAR]),data.poisk->kodtar.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODTAR]),(gpointer)E_KODTAR);

label=gtk_label_new(gettext("Фасовка"));
data.entry[E_FAS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_FAS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FAS]), data.entry[E_FAS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FAS]), "activate",GTK_SIGNAL_FUNC(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FAS]),data.poisk->fas.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FAS]),(gpointer)E_FAS);

label=gtk_label_new(gettext("Штрих код"));
data.entry[E_HTRIX] = gtk_entry_new_with_max_length (49);
gtk_box_pack_start (GTK_BOX (hbox[E_HTRIX]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_HTRIX]), data.entry[E_HTRIX], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HTRIX]), "activate",GTK_SIGNAL_FUNC(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HTRIX]),data.poisk->htrix.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HTRIX]),(gpointer)E_HTRIX);

label=gtk_label_new(gettext("Артикул"));
data.entry[E_ARTIK] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_ARTIK]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_ARTIK]), data.entry[E_ARTIK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ARTIK]), "activate",GTK_SIGNAL_FUNC(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ARTIK]),data.poisk->artik.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ARTIK]),(gpointer)E_ARTIK);

label=gtk_label_new(gettext("Критичный остаток"));
data.entry[E_KRIOST] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KRIOST]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KRIOST]), data.entry[E_KRIOST], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KRIOST]), "activate",GTK_SIGNAL_FUNC(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KRIOST]),data.poisk->kriost.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KRIOST]),(gpointer)E_KRIOST);

//GtkWidget *knopka[KOL_PFK];
GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka_pv[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka_pv[PFK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka_pv[PFK2]),"clicked",GTK_SIGNAL_FUNC(mater_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_pv[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka_pv[PFK4]=gtk_button_new_with_label(strsql);
tooltips[PFK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK4],data.knopka_pv[PFK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka_pv[PFK4]),"clicked",GTK_SIGNAL_FUNC(mater_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_pv[PFK4]),(gpointer)PFK4);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka_pv[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka_pv[PFK10],gettext("Поиск не выполнять"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka_pv[PFK10]),"clicked",GTK_SIGNAL_FUNC(mater_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_pv[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka_pv[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka_pv[PFK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka_pv[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   mater_p_key_press(GtkWidget *widget,GdkEventKey *event,class mater_data_poi *data)
{
//char  bros[512];

//printf("vmater_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka_pv[PFK2]),"clicked");
    return(FALSE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka_pv[PFK4]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka_pv[PFK10]),"clicked");
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
void  mater_p_knopka(GtkWidget *widget,class mater_data_poi *data)
{
//char bros[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case PFK2:
    for(int i=0; i < KOLENTER; i++)
     {
      if(i == E_MCENA)
        continue;
      gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");
     }
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->poisk->metka_poi=1;
    return;  

  case PFK4:
    data->poisk->clear_zero();
    return;  


  case PFK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->poisk->metka_poi=0;

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    mater_p_vvod(GtkWidget *widget,class mater_data_poi *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("mater_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->poisk->kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUP:
    data->poisk->grupa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NAIM:
    data->poisk->naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_CENA:
    data->poisk->cena.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    enter+=1;
    break;

  case E_EI:
    data->poisk->ei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KRAT:
    data->poisk->krat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODTAR:
    data->poisk->kodtar.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_FAS:
    data->poisk->fas.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_HTRIX:
    data->poisk->htrix.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_ARTIK:
    data->poisk->artik.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KRIOST:
    data->poisk->kriost.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
void mater_p_clear(class mater_data_poi *data)
{

data->poisk->clear_zero();
for(int i=0; i< KOLENTER; i++)
 { 
  if(i == E_MCENA)
    continue;
  gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");
 }

gtk_widget_grab_focus(data->entry[0]);

}
/**********************/
/*Читаем              */
/**********************/

void   mater_get_pm0(GtkWidget *widget,int *data)
{
*data=0;
printf("mater_get_pm0-%d\n",*data);
}
void   mater_get_pm1(GtkWidget *widget,int *data)
{
*data=1;
printf("mater_get_pm1-%d\n",*data);
}
void   mater_get_pm2(GtkWidget *widget,int *data)
{
*data=2;
printf("mater_get_pm2-%d\n",*data);
}
