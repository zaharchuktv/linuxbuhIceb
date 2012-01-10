/*$Id: vkartz1.c,v 1.17 2011-02-21 07:35:58 sasa Exp $*/
/*22.02.2010	25.08.2006	Белых А.И.	vkartz1.c
Вторая страница карточки работника
*/
#include <stdlib.h>
#include <unistd.h>
#include "buhg_g.h"
#include "vkartz.h"
enum
 {
  E_ADRES,
  E_NOMER_PASP,
  E_KEM_VIDAN,
  E_DATA_VP,
  E_TELEFON,
  E_HAR_RAB,
  E_GOROD_NI,
  E_IN,
  E_KART_SHET,
  E_TIP_KK,
  E_DATA_ROG,
  E_DATA_PPZ,
  E_KODBANK,
  E_KODSS,
  KOLENTER  
 };

enum
 {
  FK4,
  FK10,
  KOL_FK
 };


class vkartz1_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *radiobutton1[2];
  GtkWidget *label_zag;
  
  short kl_shift;
  short voz;      //0-ввели 1 нет

  
  class vkartz_vs *rk; //реквизиты карточки

  class iceb_u_str kodbank;
  class iceb_u_str kart_shet_old;
  
  vkartz1_data() //Конструктор
   {
    kl_shift=0;
    voz=0;
    kodbank.plus("");
   }

  void read_rek()
   {
    rk->adres.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ADRES]))));
    rk->nomer_pasp.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMER_PASP]))));
    rk->kem_vidan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KEM_VIDAN]))));
    rk->data_vp.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_VP]))));
    rk->telefon.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TELEFON]))));
    rk->har_rab.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_HAR_RAB]))));
    rk->gorod_ni.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GOROD_NI]))));
    rk->in.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_IN]))));
    rk->kart_shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KART_SHET]))));
    rk->tip_kk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TIP_KK]))));
    rk->data_rog.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_ROG]))));
    rk->data_ppz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_PPZ]))));
    rk->kodss.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODSS]))));
    kodbank.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODBANK]))));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[0])) == TRUE)
     rk->pol=0;
    else
     rk->pol=1;

   }
      
 };
void    vkartz1_vvod(GtkWidget *widget,class vkartz1_data *data);
void  vkartz1_knopka(GtkWidget *widget,class vkartz1_data *data);
gboolean   vkartz1_key_press(GtkWidget *widget,GdkEventKey *event,class vkartz1_data *data);
int vkartz1_vihod(class vkartz1_data *data);
void  vkartz1_v_e_knopka(GtkWidget *widget,class vkartz1_data *data);
int l_zarkh(int metka_rr,int tabnom,class iceb_u_str *kodbanka,class iceb_u_str *kartshet,GtkWidget *wpredok);

extern SQL_baza  bd;
extern char      *name_system;

void vkartz1(class vkartz_vs *rk,GtkWidget *wpredok)
{
char strsql[512];
class vkartz1_data data;
class SQLCURSOR cur;
SQL_str row;

data.rk=rk;
data.kart_shet_old.new_plus(data.rk->kart_shet.ravno());

if(data.rk->gorod_ni.getdlinna() > 1)
 {
  SQL_str row;
  class SQLCURSOR cur;
  int kodgr=data.rk->gorod_ni.ravno_atoi();
  //читаем наименовани города налоговой инспекцiї
  sprintf(strsql,"select naik from Gnali where kod=%d",kodgr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    data.rk->gorod_ni.new_plus(kodgr);
    data.rk->gorod_ni.plus(" ");
    data.rk->gorod_ni.plus(row[0]); 
   }
 }

class iceb_u_str kod_naim("");

if(data.rk->kodbank != 0)
 {
  sprintf(strsql,"select naik from Zarsb where kod=%d",data.rk->kodbank);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    kod_naim.plus(data.rk->kodbank);
    kod_naim.plus(" ",row[0]);
   }
 }
class iceb_u_str kod_ni("");
if(data.rk->gorod_ni.getdlinna() > 1)
 {
  sprintf(strsql,"select naik from Gnali where kod=%d",data.rk->gorod_ni.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    kod_ni.plus(data.rk->gorod_ni.ravno());
    kod_ni.plus(" ",row[0]);
   }
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),5);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",data.rk->tabnom.ravno(),data.rk->fio.ravno());

label=gtk_label_new(iceb_u_toutf(strsql));

sprintf(strsql,"%s %s",name_system,gettext("Просмотр карточки работника."));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vkartz1_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *hbox_main;
 hbox_main = gtk_hbox_new (FALSE, 2);
gtk_container_add (GTK_CONTAINER (data.window), hbox_main);

GtkWidget *vbox_knop = gtk_vbox_new (FALSE, 2);
GtkWidget *vbox_menu = gtk_vbox_new (FALSE, 2);

//GtkWidget *hbox_zagol=gtk_hbox_new (FALSE, 2);

GtkWidget *hbox_menu[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox_menu[i] = gtk_hbox_new (FALSE, 1);



gtk_box_pack_start (GTK_BOX (hbox_main), vbox_knop, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (hbox_main), vbox_menu, TRUE, TRUE, 1);

gtk_container_add (GTK_CONTAINER (vbox_menu), label);

for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox_menu), hbox_menu[i]);

GtkWidget *separator1=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox_menu), separator1);

GtkWidget *hbox_radio = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (vbox_menu), hbox_radio);

sprintf(strsql,"%s:",gettext("Пол"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox_radio),label, TRUE, TRUE, 0);


//Вставляем радиокнопки

data.radiobutton1[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Мужчина"));
data.radiobutton1[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton1[0]),gettext("Женщина"));

gtk_box_pack_start (GTK_BOX (hbox_radio), data.radiobutton1[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (hbox_radio), data.radiobutton1[1], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[data.rk->pol]),TRUE); //Устанавливем активной кнопку


GtkTooltips *tooltips_enter[KOLENTER];

label=gtk_label_new(gettext("Адрес"));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_ADRES]), label, FALSE, FALSE, 0);

data.entry[E_ADRES] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_ADRES]), data.entry[E_ADRES], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ADRES]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRES]),data.rk->adres.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ADRES]),(gpointer)E_ADRES);

label=gtk_label_new(gettext("Номер паспорта"));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_NOMER_PASP]), label, FALSE, FALSE, 0);

data.entry[E_NOMER_PASP] = gtk_entry_new_with_max_length (30);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_NOMER_PASP]), data.entry[E_NOMER_PASP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMER_PASP]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_PASP]),data.rk->nomer_pasp.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMER_PASP]),(gpointer)E_NOMER_PASP);

label=gtk_label_new(gettext("Кем выдано паспорт"));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_KEM_VIDAN]), label, FALSE, FALSE, 0);

data.entry[E_KEM_VIDAN] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_KEM_VIDAN]), data.entry[E_KEM_VIDAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KEM_VIDAN]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KEM_VIDAN]),data.rk->kem_vidan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KEM_VIDAN]),(gpointer)E_KEM_VIDAN);

label=gtk_label_new(gettext("Дата выдачи паспорта"));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_DATA_VP]), label, FALSE, FALSE, 0);

data.entry[E_DATA_VP] = gtk_entry_new_with_max_length (11);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_DATA_VP]), data.entry[E_DATA_VP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_VP]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_VP]),data.rk->data_vp.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_VP]),(gpointer)E_DATA_VP);

label=gtk_label_new(gettext("Телефон"));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_TELEFON]), label, FALSE, FALSE, 0);

data.entry[E_TELEFON] = gtk_entry_new_with_max_length (50);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_TELEFON]), data.entry[E_TELEFON], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TELEFON]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TELEFON]),data.rk->telefon.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TELEFON]),(gpointer)E_TELEFON);

label=gtk_label_new(gettext("Характер работы"));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_HAR_RAB]), label, FALSE, FALSE, 0);

data.entry[E_HAR_RAB] = gtk_entry_new_with_max_length (50);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_HAR_RAB]), data.entry[E_HAR_RAB], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HAR_RAB]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HAR_RAB]),data.rk->har_rab.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HAR_RAB]),(gpointer)E_HAR_RAB);


sprintf(strsql,"%s",gettext("Код города нал. инспекции"));
data.knopka_enter[E_GOROD_NI]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_GOROD_NI]), data.knopka_enter[E_GOROD_NI], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GOROD_NI]),"clicked",GTK_SIGNAL_FUNC(vkartz1_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GOROD_NI]),(gpointer)E_GOROD_NI);
tooltips_enter[E_GOROD_NI]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GOROD_NI],data.knopka_enter[E_GOROD_NI],gettext("Выбор города"),NULL);

data.entry[E_GOROD_NI] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox_menu[E_GOROD_NI]), data.entry[E_GOROD_NI], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOROD_NI]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOROD_NI]),kod_ni.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOROD_NI]),(gpointer)E_GOROD_NI);

label=gtk_label_new(gettext("Идентификационный номер"));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_IN]), label, FALSE, FALSE, 0);

data.entry[E_IN] = gtk_entry_new_with_max_length (15);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_IN]), data.entry[E_IN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_IN]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IN]),data.rk->in.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_IN]),(gpointer)E_IN);

label=gtk_label_new(gettext("Номер карт-счёта"));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_KART_SHET]), label, FALSE, FALSE, 0);

data.entry[E_KART_SHET] = gtk_entry_new_with_max_length (30);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_KART_SHET]), data.entry[E_KART_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KART_SHET]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KART_SHET]),data.rk->kart_shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KART_SHET]),(gpointer)E_KART_SHET);

label=gtk_label_new(gettext("Тип кредитной карты"));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_TIP_KK]), label, FALSE, FALSE, 0);

data.entry[E_TIP_KK] = gtk_entry_new_with_max_length (30);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_TIP_KK]), data.entry[E_TIP_KK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TIP_KK]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TIP_KK]),data.rk->tip_kk.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TIP_KK]),(gpointer)E_TIP_KK);

label=gtk_label_new(gettext("Дата рождения"));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_DATA_ROG]), label, FALSE, FALSE, 0);

data.entry[E_DATA_ROG] = gtk_entry_new_with_max_length (11);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_DATA_ROG]), data.entry[E_DATA_ROG], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_ROG]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_ROG]),data.rk->data_rog.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_ROG]),(gpointer)E_DATA_ROG);

label=gtk_label_new(gettext("Дата повышения зарплаты"));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_DATA_PPZ]), label, FALSE, FALSE, 0);

data.entry[E_DATA_PPZ] = gtk_entry_new_with_max_length (11);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_DATA_PPZ]), data.entry[E_DATA_PPZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_PPZ]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_PPZ]),data.rk->data_ppz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_PPZ]),(gpointer)E_DATA_PPZ);

sprintf(strsql,"%s",gettext("Код банка"));
data.knopka_enter[E_KODBANK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_KODBANK]), data.knopka_enter[E_KODBANK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODBANK]),"clicked",GTK_SIGNAL_FUNC(vkartz1_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODBANK]),(gpointer)E_KODBANK);
tooltips_enter[E_KODBANK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODBANK],data.knopka_enter[E_KODBANK],gettext("Выбор банка"),NULL);

data.entry[E_KODBANK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox_menu[E_KODBANK]), data.entry[E_KODBANK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODBANK]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODBANK]),kod_naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODBANK]),(gpointer)E_KODBANK);

label=gtk_label_new(gettext("Код спецстажа"));
gtk_box_pack_start (GTK_BOX (hbox_menu[E_KODSS]), label, FALSE, FALSE, 0);

data.entry[E_KODSS] = gtk_entry_new_with_max_length (64);
gtk_box_pack_start (GTK_BOX (hbox_menu[E_KODSS]), data.entry[E_KODSS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODSS]), "activate",GTK_SIGNAL_FUNC(vkartz1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODSS]),data.rk->kodss.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODSS]),(gpointer)E_KODSS);



GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F4 %s",gettext("Картсчета"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Ввод и корректировка списка картсчетов работника"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(vkartz1_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(vkartz1_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK10], TRUE, TRUE, 0);

//gtk_widget_grab_focus(data.entry[0]);
gtk_widget_grab_focus(data.knopka[FK10]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

data.rk->kodbank=data.kodbank.ravno_atoi();

//printf("***data.rk->pol=%d\n",data.rk->pol);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vkartz1_v_e_knopka(GtkWidget *widget,class vkartz1_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_KODBANK:
    if(l_zarsb(1,&kod,&naim,data->window) == 0)
     {
      data->kodbank.new_plus(kod.ravno());
      data->kodbank.plus(" ",naim.ravno());
      sprintf(strsql,"select nks from Zarkh where tn=%d and kb=%d",data->rk->tabnom.ravno_atoi(),kod.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
         gtk_entry_set_text(GTK_ENTRY(data->entry[E_KART_SHET]),iceb_u_toutf(row[0]));
             
     }     
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODBANK]),data->kodbank.ravno_toutf());

    return;  


  case E_GOROD_NI:
    if(l_zargni(1,&kod,&naim,data->window) == 0)
     {
      data->rk->gorod_ni.new_plus(kod.ravno());
      data->rk->gorod_ni.plus(" ",naim.ravno());
     }     
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GOROD_NI]),data->rk->gorod_ni.ravno_toutf());

    return;  



 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vkartz1_key_press(GtkWidget *widget,GdkEventKey *event,class vkartz1_data *data)
{

switch(event->keyval)
 {
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
void  vkartz1_knopka(GtkWidget *widget,class vkartz1_data *data)
{
char strsql[512];
class iceb_u_str kodbanka("");
class iceb_u_str kartshet("");
class iceb_u_str kod_naim("");
SQL_str row;
class SQLCURSOR cur;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vkartz1_knopka knop=%d\n",knop);


switch (knop)
 {

  case FK4:
    if(l_zarkh(1,data->rk->tabnom.ravno_atoi(),&kodbanka,&kartshet,data->window) == 0)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KART_SHET]),kartshet.ravno_toutf());
      kod_naim.new_plus(kodbanka.ravno());
      sprintf(strsql,"select naik from Zarsb where kod=%d",kodbanka.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
        kod_naim.plus(" ",row[0]);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODBANK]),kod_naim.ravno_toutf());
     }
    return;

  case FK10:
    if(vkartz1_vihod(data) != 0)
     return;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vkartz1_vvod(GtkWidget *widget,class vkartz1_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vkartz1_vvod enter=%d\n",enter);

switch (enter)
 {


  case E_IN:
    data->rk->in.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(strlen(data->rk->in.ravno()) != 10)
      iceb_menu_soob(gettext("Количество знаков в индентификационном коде не равно 10 !"),data->window);

    /*Проверяем*/
    sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",data->rk->in.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) >= 1)
     {
      if(data->rk->tabnom.ravno_atoi() != atoi(row[0]))
       {
        class iceb_u_str repl;
        repl.plus(gettext("Такой идентификационный код уже введён !"));
        repl.ps_plus(row[0]);
        repl.plus(" ");
        repl.plus(row[1]);
        iceb_menu_soob(&repl,data->window);
       }
     }

    if(strlen(data->rk->in.ravno()) == 10)
     {
      data->rk->data_rog.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATA_ROG]))));
      if(data->rk->data_rog.getdlinna() <= 1)
       {
        char bros[512];
        sprintf(bros,"%5.5s",data->rk->in.ravno());
        int kold=atoi(bros);
        short dr=0;
        short mr=1;
        short gr=1900;
        iceb_u_denrog(&dr,&mr,&gr,kold);
        sprintf(bros,"%d.%d.%d",dr,mr,gr);
        data->rk->data_rog.new_plus(bros);

        gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_ROG]),data->rk->data_rog.ravno_toutf());

        data->rk->pol=0;
        if(data->rk->in.ravno()[8] == '0' || data->rk->in.ravno()[8] == '2' 
        || data->rk->in.ravno()[8] == '4' || data->rk->in.ravno()[8] == '6' || 
        data->rk->in.ravno()[8] == '8')
          data->rk->pol=1;
        
        if(data->rk->pol == 0)
           gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data->radiobutton1[0]),TRUE); //Устанавливем активной кнопку
        if(data->rk->pol == 1)
           gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data->radiobutton1[1]),TRUE); //Устанавливем активной кнопку
       }                

     }

    break;

  case E_KART_SHET:
    data->rk->kart_shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->rk->kart_shet.ravno_atoi() == 0)
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODBANK]),"");
     
    break;

  case E_KODBANK:
    data->kodbank.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->kodbank.ravno_atoi() > 0)
     {
      sprintf(strsql,"select naik from Zarsb where kod=%d",data->kodbank.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
       {
        iceb_menu_soob(gettext("Нет такого кода банка!"),data->window);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODBANK]),"");
        break;
       }      
      else
       {
        sprintf(strsql,"%s %s",data->kodbank.ravno(),row[0]);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODBANK]),iceb_u_toutf(strsql));
       }
      sprintf(strsql,"select nks from Zarkh where tn=%d and kb=%d",data->rk->tabnom.ravno_atoi(),data->kodbank.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       gtk_entry_set_text(GTK_ENTRY(data->entry[E_KART_SHET]),iceb_u_toutf(row[0]));
     }
    break;

 }


enter+=1;
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}


/***************************/
/*Проверка при выходе из меню правильности ввода данных*/
/*******************************************************/

int vkartz1_vihod(class vkartz1_data *data)
{
data->read_rek();

if(data->rk->data_vp.getdlinna() > 1)
 if(data->rk->data_vp.prov_dat() != 0)
  {
   iceb_menu_soob(gettext("Неплавильно введена дата получения паспорта !"),data->window);
   return(1);
  }
if(data->rk->data_rog.getdlinna() > 1)
 if(data->rk->data_rog.prov_dat() != 0)
  {
   iceb_menu_soob(gettext("Неплавильно введена дата рождения !"),data->window);
   return(1);
  }
if(data->rk->data_ppz.getdlinna() > 1)
 if(data->rk->data_ppz.prov_dat() != 0)
  {
   iceb_menu_soob(gettext("Неплавильно введена дата повышения зарплаты !"),data->window);
   return(1);
  }

if(data->rk->in.getdlinna() <= 1)
 return(0);
 
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
/*Проверяем*/

sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",data->rk->in.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) >= 1)
 {
  if(data->rk->tabnom.ravno_atoi() != atoi(row[0]))
   {
    class iceb_u_str repl;
    repl.plus(gettext("Такой идентификационный код уже введён !"));
    repl.ps_plus(row[0]);
    repl.plus(" ");
    repl.plus(row[1]);
    iceb_menu_soob(&repl,data->window);
    return(1);
   }
 }

if(data->kodbank.ravno_atoi() > 0)
 {
  sprintf(strsql,"select kod from Zarsb where kod=%d",data->kodbank.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) <= 0)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код банка"),data->kodbank.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);  
   }
 }
if(data->rk->gorod_ni.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Gnali where kod=%d",data->rk->gorod_ni.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) <= 0)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код города"),data->rk->gorod_ni.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);  
   }
 }

/*проверяем есть ли этот код банка в списке картсчетов*/
if(data->kodbank.ravno_atoi() != 0)
 {
  sprintf(strsql,"select kb from Zarkh where tn=%d and kb=%d",data->rk->tabnom.ravno_atoi(),data->kodbank.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) == 0)
   {
    sprintf(strsql,"insert into Zarkh values(%d,%d,'%s',%d,%ld)",
    data->rk->tabnom.ravno_atoi(),data->kodbank.ravno_atoi(),data->rk->kart_shet.ravno(),iceb_getuid(data->window),time(NULL));
    iceb_sql_zapis(strsql,1,0,data->window);
   }       
  else
   if(iceb_u_SRAV(data->rk->kart_shet.ravno(),data->kart_shet_old.ravno(),0) != 0)
    if(data->rk->kart_shet.getdlinna() > 1)
     {
      sprintf(strsql,"update Zarkh set \
nks='%s',\
ktoz=%d,\
vrem=%ld \
where tn=%d and kb=%d",
      data->rk->kart_shet.ravno(),
      iceb_getuid(data->window),
      time(NULL),
      data->rk->tabnom.ravno_atoi(),
      data->kodbank.ravno_atoi());
      iceb_sql_zapis(strsql,1,0,data->window);
     }
 }

return(0);
}

