/*$Id: l_spis_opl_p.c,v 1.15 2011-02-21 07:36:20 sasa Exp $*/
/*09.01.2007	27.01.2006	Белых А.И.	l_spis_opl_p.c
Реквизиты для просмотра списка оплат
*/
#include <unistd.h>
#include "i_rest.h"
#include "l_spis_opl.h"

enum
{
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
};

enum
 {
  E_DATAN,
  E_VREMN,
  E_DATAK,
  E_VREMK,

  E_KASSA,
  E_SUMA,
  E_DATAD,
  E_NOMDOK,
  E_KODKL,
  E_KOMENT,
  E_PODR,
  KOLENTER  
 };

class  spis_opl_p_data
 {
  public:
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopkap[KOL_PFK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton[2];
  GtkWidget *radiobutton2[2];
  GtkWidget *radiobutton3[2];
  
  class l_spis_opl_rek *rk; //Реквизиты поиска документов
  short kl_shift;
  int voz; //0-искать записи 1-нет
    
 };
 
void       spis_opl_p_radio0(GtkWidget *,class spis_opl_p_data *);
void       spis_opl_p_radio1(GtkWidget *,class spis_opl_p_data *);

void       spis_opl_p_radio2_0(GtkWidget *,class spis_opl_p_data *);
void       spis_opl_p_radio2_1(GtkWidget *,class spis_opl_p_data *);

void       spis_opl_p_radio3_0(GtkWidget *,class spis_opl_p_data *);
void       spis_opl_p_radio3_1(GtkWidget *,class spis_opl_p_data *);

gboolean   spis_opl_p_key_press(GtkWidget *widget,GdkEventKey *event,class spis_opl_p_data *data);
void  spis_opl_p_knopka(GtkWidget *widget,class spis_opl_p_data *data);
void  spis_opl_v_e_knopka(GtkWidget *widget,class spis_opl_p_data *data);
void    spis_opl_p_vvod(GtkWidget *widget,class spis_opl_p_data *data);

//extern SQL_baza	bd;
extern char *name_system;
extern class iceb_get_dev_data config_dev;


int l_spis_opl_p(class l_spis_opl_rek *datap,GtkWidget *wpredok)
{
printf("l_spis_opl_p\n");

class  spis_opl_p_data data;
data.rk=datap;

char strsql[300];

    
data.kl_shift=0;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));


gtk_window_set_title(GTK_WINDOW(data.window),strsql);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(spis_opl_p_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label=gtk_label_new(gettext("Поиск записей внесения средств и погашения счетов"));


GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
gtk_container_add (GTK_CONTAINER (vbox), label);

GtkWidget *hbox[KOLENTER];

for(int i=0 ; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);
GtkWidget *separator1=gtk_hseparator_new();
GtkWidget *separator2=gtk_hseparator_new();
GtkWidget *separator3=gtk_hseparator_new();
GtkWidget *separator4=gtk_hseparator_new();

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0 ; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), separator1);

//Вставляем радиокнопки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Оплату наличными - показывать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[0]), "clicked",GTK_SIGNAL_FUNC(spis_opl_p_radio0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));


data.radiobutton[1]=gtk_radio_button_new_with_label(group,gettext("Оплату наличными - не показывать"));
gtk_signal_connect(GTK_OBJECT(data.radiobutton[1]), "clicked",GTK_SIGNAL_FUNC(spis_opl_p_radio1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[1], TRUE, TRUE, 0);


gtk_container_add (GTK_CONTAINER (vbox), separator2);

data.radiobutton2[0]=gtk_radio_button_new_with_label(NULL,gettext("Безналичную оплату - показывать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[0]), "clicked",GTK_SIGNAL_FUNC(spis_opl_p_radio2_0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton20),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton2[0]));


data.radiobutton2[1]=gtk_radio_button_new_with_label(group,gettext("Безналичную оплату - не показывать"));
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[1]), "clicked",GTK_SIGNAL_FUNC(spis_opl_p_radio2_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);

gtk_container_add (GTK_CONTAINER (vbox), separator3);

data.radiobutton3[0]=gtk_radio_button_new_with_label(NULL,gettext("Списание - показывать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton3[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton3[0]), "clicked",GTK_SIGNAL_FUNC(spis_opl_p_radio3_0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton30),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton3[0]));


data.radiobutton3[1]=gtk_radio_button_new_with_label(group,gettext("Спиание - не показывать"));
gtk_signal_connect(GTK_OBJECT(data.radiobutton3[1]), "clicked",GTK_SIGNAL_FUNC(spis_opl_p_radio3_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton3[1], TRUE, TRUE, 0);

if(data.rk->metka_nal == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[0]),TRUE); //Устанавливем активной кнопку
if(data.rk->metka_nal == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[1]),TRUE); //Устанавливем активной кнопку

if(data.rk->metka_bnal == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[0]),TRUE); //Устанавливем активной кнопку
if(data.rk->metka_bnal == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[1]),TRUE); //Устанавливем активной кнопку

if(data.rk->metka_spis == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton3[0]),TRUE); //Устанавливем активной кнопку
if(data.rk->metka_spis == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton3[1]),TRUE); //Устанавливем активной кнопку

gtk_container_add (GTK_CONTAINER (vbox), separator4);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


data.knopka_enter[E_DATAN]=gtk_button_new_with_label(gettext("Дата начала"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(spis_opl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);


data.knopka_enter[E_VREMN]=gtk_button_new_with_label(gettext("Время начала"));
gtk_box_pack_start (GTK_BOX (hbox[E_VREMN]), data.knopka_enter[E_VREMN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_VREMN]),"clicked",GTK_SIGNAL_FUNC(spis_opl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_VREMN]),(gpointer)E_VREMN);

data.entry[E_VREMN] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[E_VREMN]), data.entry[E_VREMN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VREMN]), "activate",GTK_SIGNAL_FUNC(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREMN]),data.rk->vremn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VREMN]),(gpointer)E_VREMN);


data.knopka_enter[E_DATAK]=gtk_button_new_with_label(gettext("Дата конца"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(spis_opl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);


data.knopka_enter[E_VREMK]=gtk_button_new_with_label(gettext("Время конца"));
gtk_box_pack_start (GTK_BOX (hbox[E_VREMK]), data.knopka_enter[E_VREMK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_VREMK]),"clicked",GTK_SIGNAL_FUNC(spis_opl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_VREMK]),(gpointer)E_VREMK);

data.entry[E_VREMK] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[E_VREMK]), data.entry[E_VREMK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VREMK]), "activate",GTK_SIGNAL_FUNC(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREMK]),data.rk->vremk.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VREMK]),(gpointer)E_VREMK);

sprintf(strsql,"%s (,,)",gettext("Номер документа"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),"clicked",GTK_SIGNAL_FUNC(spis_opl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),(gpointer)E_NOMDOK);

data.entry[E_NOMDOK] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);

data.knopka_enter[E_DATAD]=gtk_button_new_with_label(gettext("Дата документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.knopka_enter[E_DATAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAD]),"clicked",GTK_SIGNAL_FUNC(spis_opl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAD]),(gpointer)E_DATAD);

data.entry[E_DATAD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.entry[E_DATAD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAD]), "activate",GTK_SIGNAL_FUNC(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAD]),data.rk->datad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAD]),(gpointer)E_DATAD);

sprintf(strsql,"%s (,,)",gettext("Код клиента"));
data.knopka_enter[E_KODKL]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.knopka_enter[E_KODKL], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODKL]),"clicked",GTK_SIGNAL_FUNC(spis_opl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODKL]),(gpointer)E_KODKL);

data.entry[E_KODKL] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.entry[E_KODKL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODKL]), "activate",GTK_SIGNAL_FUNC(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKL]),data.rk->kodkl.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODKL]),(gpointer)E_KODKL);


data.knopka_enter[E_KOMENT]=gtk_button_new_with_label(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.knopka_enter[E_KOMENT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOMENT]),"clicked",GTK_SIGNAL_FUNC(spis_opl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOMENT]),(gpointer)E_KOMENT);

data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk->koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);


sprintf(strsql,"%s (,,)",gettext("Подразделение"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(spis_opl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);

sprintf(strsql,"%s (,,)",gettext("Касса"));
data.knopka_enter[E_KASSA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.knopka_enter[E_KASSA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KASSA]),"clicked",GTK_SIGNAL_FUNC(spis_opl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KASSA]),(gpointer)E_KASSA);

data.entry[E_KASSA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.entry[E_KASSA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KASSA]), "activate",GTK_SIGNAL_FUNC(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KASSA]),data.rk->kassa.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KASSA]),(gpointer)E_KASSA);

sprintf(strsql,"%s",gettext("Сумма"));
data.knopka_enter[E_SUMA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.knopka_enter[E_SUMA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SUMA]),"clicked",GTK_SIGNAL_FUNC(spis_opl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SUMA]),(gpointer)E_SUMA);

data.entry[E_SUMA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.rk->suma.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);


GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopkap[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopkap[PFK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopkap[PFK2]),"clicked",GTK_SIGNAL_FUNC(spis_opl_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopkap[PFK2]),(gpointer)PFK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopkap[PFK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopkap[PFK4]=gtk_button_new_with_label(strsql);
tooltips[PFK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK4],data.knopkap[PFK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopkap[PFK4]),"clicked",GTK_SIGNAL_FUNC(spis_opl_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopkap[PFK4]),(gpointer)PFK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopkap[PFK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopkap[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopkap[PFK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopkap[PFK10]),"clicked",GTK_SIGNAL_FUNC(spis_opl_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopkap[PFK10]),(gpointer)PFK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopkap[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  spis_opl_p_radio0(GtkWidget *widget,class spis_opl_p_data *data)
{
//g_print("spis_opl_p_radio0\n");

data->rk->metka_nal=0;

}
void  spis_opl_p_radio1(GtkWidget *widget,class spis_opl_p_data *data)
{
//g_print("spis_opl_p_radio1\n");
data->rk->metka_nal=1;
}
/*********************/
void  spis_opl_p_radio2_0(GtkWidget *widget,class spis_opl_p_data *data)
{
//g_print("spis_opl_p_radio0\n");

data->rk->metka_bnal=0;

}
void  spis_opl_p_radio2_1(GtkWidget *widget,class spis_opl_p_data *data)
{
//g_print("spis_opl_p_radio1\n");
data->rk->metka_bnal=1;
}
/*********************/
void  spis_opl_p_radio3_0(GtkWidget *widget,class spis_opl_p_data *data)
{
//g_print("spis_opl_p_radio0\n");

data->rk->metka_spis=0;

}
void  spis_opl_p_radio3_1(GtkWidget *widget,class spis_opl_p_data *data)
{
//g_print("spis_opl_p_radio1\n");
data->rk->metka_spis=1;
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  spis_opl_v_e_knopka(GtkWidget *widget,class spis_opl_p_data *data)
{
iceb_u_str naim;
iceb_u_str kod;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_DATAN:
    //читаем реквизит
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_DATAN]),"activate");

    if(config_dev.metka_screen == 0) //сенсорного экрана нет
      iceb_calendar(&data->rk->datan,data->window);
    else
      iceb_mous_klav(gettext("Дата начала"),&data->rk->datan,10,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno_toutf());
    return;  

  case E_VREMN:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_VREMN]),"activate");
    iceb_mous_klav(gettext("Время начала"),&data->rk->vremn,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMN]),data->rk->vremn.ravno_toutf());
    return;  

  case E_VREMK:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_VREMK]),"activate");
    iceb_mous_klav(gettext("Время конца"),&data->rk->vremk,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMK]),data->rk->vremk.ravno_toutf());
    return;  

  case E_DATAK:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_DATAK]),"activate");

    if(config_dev.metka_screen == 0) //сенсорного экрана нет
      iceb_calendar(&data->rk->datak,data->window);
    else
      iceb_mous_klav(gettext("Дата конца"),&data->rk->datak,10,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno_toutf());

    return;  

  case E_DATAD:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_DATAD]),"activate");
    if(config_dev.metka_screen == 0) //сенсорного экрана нет
      iceb_calendar(&data->rk->datad,data->window);
    else
      iceb_mous_klav(gettext("Дата документа"),&data->rk->datad,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->rk->datad.ravno_toutf());
    return;  

  case E_NOMDOK:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_NOMDOK]),"activate");
    iceb_mous_klav(gettext("Номер документа"),&data->rk->datak,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk->datak.ravno_toutf());
    return;  


  case E_KODKL:
//    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_KODKL]),"activate");
/***********
    if(config_dev.metka_screen == 0) //сенсорного экрана нет
     {
      if(l_klientr(1,&kod,data->window) == 0)
       data->rk->kodkl.z_plus(kod.ravno());
     }
    else
**************/
      iceb_mous_klav(gettext("Код клиента"),&data->rk->kodkl,10,1,0,1,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKL]),data->rk->kodkl.ravno_toutf());
    return;  


  case E_KOMENT:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_KOMENT]),"activate");
    iceb_mous_klav(gettext("Коментарий"),&data->rk->koment,100,1,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOMENT]),data->rk->koment.ravno_toutf());
    return;  

  case E_PODR:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_PODR]),"activate");
    if(config_dev.metka_screen == 0) //сенсорного экрана нет
     {
      if(rpod_l(1,&kod,&naim,data->window) == 0)
        data->rk->podr.z_plus(kod.ravno());
     }
    else
      iceb_mous_klav(gettext("Подразделение"),&data->rk->podr,100,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno_toutf());
    return;  

  case E_KASSA:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_KASSA]),"activate");
    
    iceb_mous_klav(gettext("Касса"),&data->rk->kassa,100,0,0,0,data->window);
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KASSA]),data->rk->kassa.ravno_toutf());
    return;  

  case E_SUMA:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_SUMA]),"activate");
    iceb_mous_klav(gettext("Сумма"),&data->rk->suma,20,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA]),data->rk->suma.ravno_toutf());
    return;  
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   spis_opl_p_key_press(GtkWidget *widget,GdkEventKey *event,class spis_opl_p_data *data)
{
//char  bros[300];

//printf("spis_opl_p_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopkap[PFK2]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopkap[PFK4]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopkap[PFK10]),"clicked");
    return(TRUE);
//    return(FALSE);

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
void  spis_opl_p_knopka(GtkWidget *widget,class spis_opl_p_data *data)
{
short d,m,g;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case PFK2:
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

    if(data->rk->datan.getdlinna() > 1)
    if(iceb_u_rsdat(&d,&m,&g,data->rk->datan.ravno(),1) != 0)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не верно введена дата начала !"));
      iceb_menu_soob(&repl,data->window);
      return;
     }

    if(data->rk->datak.getdlinna() > 1)
     if(iceb_u_rsdat(&d,&m,&g,data->rk->datak.ravno(),1) != 0)
      {
       iceb_u_str repl;
       repl.plus(gettext("Не верно введена дата конца !"));
       iceb_menu_soob(&repl,data->window);
       return;
      }

    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case PFK4:
    data->rk->clear_data(); 
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");
    return;

  case PFK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    spis_opl_p_vvod(GtkWidget *widget,class spis_opl_p_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("spis_opl_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

  case E_VREMN:
    data->rk->vremn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

  case E_VREMK:
    data->rk->vremk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
     
    break;

  case E_DATAK:
    data->rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAD:
    data->rk->datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


  case E_KODKL:
    data->rk->kodkl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMDOK:
    data->rk->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOMENT:
    data->rk->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_PODR:
    data->rk->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KASSA:
    data->rk->kassa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SUMA:
    data->rk->suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
