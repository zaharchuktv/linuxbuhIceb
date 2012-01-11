/*$Id: rasoskr_m.c,v 1.27 2011-02-21 07:35:56 sasa Exp $*/
/*29.09.2009	22.10.2004	Белых А.И. rasoskr_m.c
Меню для ввода данных для поиска нужных карточек
*/
#include <stdlib.h>
#include "../headers/buhg_g.h"
#include "rasoskr.h"
enum
{
  FK2,
  FK3,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SKLAD,
  E_GRUPA,
  E_SHET,
  E_KODMAT,
  E_DATA_OST,
  E_NAIM,
  E_NDS,
  E_INVNOM,
  E_KON_DAT_IS,
  KOLENTER  
 };

class rasoskr_m_data
 {
  public:
  rasoskr_data  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *radiobutton1[3];
  GtkWidget *radiobutton2[2];
  GtkWidget *radiobutton3[2];
  short kl_shift;
  short voz;      //0-начать расчет 1 нет
  
  rasoskr_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->sklad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SKLAD]))));
    rk->grupa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GRUPA]))));
    rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    rk->kodmat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODMAT]))));
    rk->data_ost.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_OST]))));
    rk->naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM]))));
    rk->nds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NDS]))));
    rk->invnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_INVNOM]))));
    rk->kon_dat_is.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KON_DAT_IS]))));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[0])) == TRUE)
     rk->metka_r=0;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[1])) == TRUE)
     rk->metka_r=1;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[2])) == TRUE)
     rk->metka_r=2;

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[0])) == TRUE)
     rk->metka_ost=0;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[1])) == TRUE)
     rk->metka_ost=1;

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton3[0])) == TRUE)
     rk->metka_innom=0;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton3[1])) == TRUE)
     rk->metka_innom=1;

   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rk->clear_data();
   }
 };



gboolean   rasoskr_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class rasoskr_m_data *data);
void    rasoskr_m_v_vvod(GtkWidget *widget,class rasoskr_m_data *data);
void  rasoskr_m_v_knopka(GtkWidget *widget,class rasoskr_m_data *data);
void   rasoskr_m_rekviz(class rasoskr_m_data *data);
void  rasoskr_v_e_knopka(GtkWidget *widget,class rasoskr_m_data *data);


void rasoskr_sr(class rasoskr_m_data *data);

extern char *name_system;
extern SQL_baza bd;

int   rasoskr_m(class rasoskr_data *rek_ras,GtkWidget *wpredok)
{
rasoskr_m_data data;
char strsql[512];
float pnds=iceb_pnds(wpredok);

data.rk=rek_ras;
if(data.rk->data_ost.getdlinna() <= 1)
 {
  short dt,mt,gt;
  iceb_u_poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%d.%d.%d",dt,mt,gt);
  data.rk->data_ost.new_plus(strsql);  
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать остатки на карточках"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rasoskr_m_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Распечатать остатки на карточках"));

GtkWidget *vbox = gtk_vbox_new (FALSE, 3);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

GtkWidget *separator1=gtk_hseparator_new();
GtkWidget *separator2=gtk_hseparator_new();
GtkWidget *separator3=gtk_hseparator_new();
GtkWidget *separator4=gtk_hseparator_new();


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

sprintf(strsql,gettext("Если НДС не ввели - все карточки\n\
Если НДС = 0      - карточки с нулевым НДС\n\
Если НДС = %2.f     - карточки с %2.f%% НДС\n\
Если НДС = +      - карточки цена в которых вместе с НДС"),pnds,pnds);

label=gtk_label_new(strsql);
gtk_container_add (GTK_CONTAINER (vbox), label);

gtk_container_add (GTK_CONTAINER (vbox), separator1);

//Вставляем радиокнопки

data.radiobutton1[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Сортировать группа материалла-склад"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[0], TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Сортировать счет учета материалла-склад"));
data.radiobutton1[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton1[0]),strsql);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[1], TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Сортировать счет-группа материалла"));
data.radiobutton1[2]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton1[1]),strsql);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[2], TRUE, TRUE, 0);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[data.rk->metka_r]),TRUE); //Устанавливем активной кнопку




gtk_container_add (GTK_CONTAINER (vbox), separator2);


data.radiobutton2[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Карточки с нулевыми остатками не печатать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Карточки с нулевыми остатками печатать"));
data.radiobutton2[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[0]),strsql);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk->metka_ost]),TRUE); //Устанавливем активной кнопку


gtk_container_add (GTK_CONTAINER (vbox), separator3);


data.radiobutton3[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Все карточки"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton3[0], TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Только карточки с инвентарными номерами"));
data.radiobutton3[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton3[0]),strsql);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton3[1], TRUE, TRUE, 0);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton3[data.rk->metka_innom]),TRUE); //Устанавливем активной кнопку


gtk_container_add (GTK_CONTAINER (vbox), separator4);



gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s (,,)",gettext("Слад"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SKLAD]),"clicked",GTK_SIGNAL_FUNC(rasoskr_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SKLAD]),(gpointer)E_SKLAD);
tooltips_enter[E_SKLAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SKLAD],data.knopka_enter[E_SKLAD],gettext("Выбор склада"),NULL);

data.entry[E_SKLAD] = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKLAD]), "activate",GTK_SIGNAL_FUNC(rasoskr_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk->sklad.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKLAD]),(gpointer)E_SKLAD);

sprintf(strsql,"%s (,,)",gettext("Группа"));
data.knopka_enter[E_GRUPA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.knopka_enter[E_GRUPA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUPA]),"clicked",GTK_SIGNAL_FUNC(rasoskr_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUPA]),(gpointer)E_GRUPA);
tooltips_enter[E_GRUPA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUPA],data.knopka_enter[E_GRUPA],gettext("Выбор группы"),NULL);

data.entry[E_GRUPA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.entry[E_GRUPA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUPA]), "activate",GTK_SIGNAL_FUNC(rasoskr_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPA]),data.rk->grupa.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUPA]),(gpointer)E_GRUPA);

sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(rasoskr_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(rasoskr_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

sprintf(strsql,"%s (,,)",gettext("Код материалла"));
data.knopka_enter[E_KODMAT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.knopka_enter[E_KODMAT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODMAT]),"clicked",GTK_SIGNAL_FUNC(rasoskr_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODMAT]),(gpointer)E_KODMAT);
tooltips_enter[E_KODMAT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODMAT],data.knopka_enter[E_KODMAT],gettext("Выбор материалла"),NULL);

data.entry[E_KODMAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.entry[E_KODMAT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODMAT]), "activate",GTK_SIGNAL_FUNC(rasoskr_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODMAT]),data.rk->kodmat.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODMAT]),(gpointer)E_KODMAT);

sprintf(strsql,"%s",gettext("Дата остатка"));
data.knopka_enter[E_DATA_OST]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OST]), data.knopka_enter[E_DATA_OST], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_OST]),"clicked",GTK_SIGNAL_FUNC(rasoskr_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_OST]),(gpointer)E_DATA_OST);
tooltips_enter[E_DATA_OST]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_OST],data.knopka_enter[E_DATA_OST],gettext("Выбор даты"),NULL);

data.entry[E_DATA_OST] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OST]), data.entry[E_DATA_OST], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_OST]), "activate",GTK_SIGNAL_FUNC(rasoskr_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_OST]),data.rk->data_ost.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_OST]),(gpointer)E_DATA_OST);

sprintf(strsql,"%s (0,%.2f,+)",gettext("НДС"),pnds);
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NDS] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), data.entry[E_NDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NDS]), "activate",GTK_SIGNAL_FUNC(rasoskr_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NDS]),data.rk->nds.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NDS]),(gpointer)E_NDS);

sprintf(strsql,"%s (,,)",gettext("Наименование материалла"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NAIM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(rasoskr_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk->naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

sprintf(strsql,"%s (,,)",gettext("Инвентарный номер"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_INVNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_INVNOM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_INVNOM]), data.entry[E_INVNOM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_INVNOM]), "activate",GTK_SIGNAL_FUNC(rasoskr_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INVNOM]),data.rk->invnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_INVNOM]),(gpointer)E_INVNOM);


sprintf(strsql,"%s",gettext("Конечная дата использования"));
data.knopka_enter[E_KON_DAT_IS]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KON_DAT_IS]), data.knopka_enter[E_KON_DAT_IS], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KON_DAT_IS]),"clicked",GTK_SIGNAL_FUNC(rasoskr_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KON_DAT_IS]),(gpointer)E_KON_DAT_IS);
tooltips_enter[E_KON_DAT_IS]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KON_DAT_IS],data.knopka_enter[E_KON_DAT_IS],gettext("Выбор даты"),NULL);

data.entry[E_KON_DAT_IS] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KON_DAT_IS]), data.entry[E_KON_DAT_IS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KON_DAT_IS]), "activate",GTK_SIGNAL_FUNC(rasoskr_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KON_DAT_IS]),data.rk->kon_dat_is.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KON_DAT_IS]),(gpointer)E_KON_DAT_IS);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(rasoskr_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр нужных списков, для заполнения нужными кодами реквизитов меню"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(rasoskr_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(rasoskr_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(rasoskr_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  rasoskr_v_e_knopka(GtkWidget *widget,class rasoskr_m_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATA_OST:

    if(iceb_calendar(&data->rk->data_ost,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_OST]),data->rk->data_ost.ravno());
      
    return;  

  case E_KON_DAT_IS:

    if(iceb_calendar(&data->rk->kon_dat_is,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KON_DAT_IS]),data->rk->kon_dat_is.ravno());
      
    return;  

   
  case E_SHET:

    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno_toutf());

    return;  

  case E_SKLAD:

    iceb_vibrek(0,"Sklad",&data->rk->sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno_toutf());

    return;  

  case E_GRUPA:

    iceb_vibrek(0,"Grup",&data->rk->grupa,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),data->rk->grupa.ravno_toutf());

    return;  

  case E_KODMAT:

    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->rk->kodmat.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),data->rk->kodmat.ravno_toutf());
     }

    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rasoskr_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class rasoskr_m_data *data)
{

//printf("rasoskr_m_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
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
void  rasoskr_m_v_knopka(GtkWidget *widget,class rasoskr_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->rk->data_ost.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Не верно введена дата !"),data->window);
      return;       
     }
    if(data->rk->kon_dat_is.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Не верно введена дата конечного использования!"),data->window);
      return;       
     }
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK3:

    rasoskr_sr(data);
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

void    rasoskr_m_v_vvod(GtkWidget *widget,class rasoskr_m_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("rasoskr_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SKLAD:
    data->rk->sklad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUPA:
    data->rk->grupa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET:
    data->rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODMAT:
    data->rk->kodmat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATA_OST:
    data->rk->data_ost.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NDS:
    data->rk->nds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NAIM:
    data->rk->naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}

/*******************/
/*выбрать вид списка*/
/*********************/

void rasoskr_sr(class rasoskr_m_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Просмотр списка складов"));//0
punkt_m.plus(gettext("Просмотр списка групп"));//1
punkt_m.plus(gettext("Просмотр списка счетов"));//2
punkt_m.plus(gettext("Просмотр списка материалов"));//3


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

switch(nomer)
 {
  case 0:
    if(iceb_vibrek(0,"Sklad",&data->rk->sklad,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno_toutf());
    break;

  case 1:
    if(iceb_vibrek(0,"Grup",&data->rk->grupa,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),data->rk->grupa.ravno_toutf());
    break;

  case 2:
    if(iceb_vibrek(0,"Plansh",&data->rk->shet,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno_toutf());
    break;

  case 3:
    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->rk->kodmat.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),data->rk->kodmat.ravno_toutf());
     }
    break;
 
 }
 


}
