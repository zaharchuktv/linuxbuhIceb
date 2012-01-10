/*$Id: m_poiprov.c,v 1.32 2011-02-21 07:35:55 sasa Exp $*/
/*27.03.2010	06.01.2004	Белых А.И.	m_poiprov.c
Меню для ввода реквизитов поиска проводок
*/

//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include  "prov_poi.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SHET,
  E_SHETK,
  E_KONTR,
  E_KOMENT,
  E_DEBET,
  E_KREDIT,
  E_DATAN,
  E_DATAK,
  E_METKA,
  E_NOMDOK,
  E_KODOP,
  E_GRUPAK,
  E_KEKV,
  E_DATANZ,
  E_DATAKZ,
  KOLENTER  
 };

class poiprov_data
 {
  public:
  
  class prov_poi_data *rk;
  
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  int voz;
  //Конструктор
  poiprov_data()
   {
    voz=1;
    kl_shift=0;
    window=NULL;
   }

  void activate()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }

  void clear_zero()
   {
    rk->clear_zero();
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }

 };

void    poiprov_v_vvod(GtkWidget *widget,class poiprov_data *data);
void  poiprov_v_knopka(GtkWidget *widget,class poiprov_data *data);
gboolean   poiprov_v_key_press(GtkWidget *widget,GdkEventKey *event,class poiprov_data *data);
void      poiprov_rek_metka(class poiprov_data *data);
void  m_poiprov_v_e_knopka(GtkWidget *widget,class poiprov_data *data);

extern SQL_baza	bd;
extern char *name_system;



int m_poiprov(class prov_poi_data *rkp)
{


iceb_u_str stroka;
char strsql[512];
poiprov_data data;


//Делаем ссылку на структуру в которой остались реквизиты от предыдущей работы
data.rk=rkp;

//printf("m_poiprov\n");

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Поиск"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(poiprov_v_key_press),&data);

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

stroka.new_plus(gettext("Поиск проводок"));


stroka.ps_plus(gettext("По невведенным реквизитам меню поиск не выполняется"));

GtkWidget *label=gtk_label_new(stroka.ravno_toutf());

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s (,,)",gettext("Счет"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(m_poiprov_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

sprintf(strsql,"%s (,,)",gettext("Счет корреспондент"));
data.knopka_enter[E_SHETK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHETK]), data.knopka_enter[E_SHETK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHETK]),"clicked",GTK_SIGNAL_FUNC(m_poiprov_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHETK]),(gpointer)E_SHETK);
tooltips_enter[E_SHETK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHETK],data.knopka_enter[E_SHETK],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHETK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETK]), data.entry[E_SHETK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHETK]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETK]),data.rk->shetk.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHETK]),(gpointer)E_SHETK);

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KONTR]),"clicked",GTK_SIGNAL_FUNC(m_poiprov_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KONTR]),(gpointer)E_KONTR);
tooltips_enter[E_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KONTR],data.knopka_enter[E_KONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk->kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);

label=gtk_label_new(gettext("Коментарий"));
data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk->koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);

label=gtk_label_new(gettext("Дебет"));
data.entry[E_DEBET] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_DEBET]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DEBET]), data.entry[E_DEBET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DEBET]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEBET]),data.rk->debet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DEBET]),(gpointer)E_DEBET);


label=gtk_label_new(gettext("Кредит"));
data.entry[E_KREDIT] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KREDIT]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KREDIT]), data.entry[E_KREDIT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KREDIT]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KREDIT]),data.rk->kredit.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KREDIT]),(gpointer)E_KREDIT);


sprintf(strsql,"%s (%s)(%s)",gettext("Дата начала поиска"),gettext("по дате проводки"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(m_poiprov_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

sprintf(strsql,"%s (%s)(%s)",gettext("Дата конца"),gettext("по дате проводки"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(m_poiprov_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);

sprintf(strsql,"%s (,,)",gettext("Метка проводки"));
data.knopka_enter[E_METKA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_METKA]), data.knopka_enter[E_METKA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_METKA]),"clicked",GTK_SIGNAL_FUNC(m_poiprov_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_METKA]),(gpointer)E_METKA);
tooltips_enter[E_METKA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_METKA],data.knopka_enter[E_METKA],gettext("Выбор метки проводки"),NULL);

data.entry[E_METKA] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_METKA]), data.entry[E_METKA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_METKA]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_METKA]),data.rk->metka.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_METKA]),(gpointer)E_METKA);

label=gtk_label_new(gettext("Номер документа"));
data.entry[E_NOMDOK] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);

stroka.new_plus(gettext("Код операции"));
stroka.plus(" (,,)");
label=gtk_label_new(stroka.ravno_toutf());
data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODOP]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODOP]),(gpointer)E_KODOP);


sprintf(strsql,"%s (,,)",gettext("Код группы контрагента"));
data.knopka_enter[E_GRUPAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPAK]), data.knopka_enter[E_GRUPAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUPAK]),"clicked",GTK_SIGNAL_FUNC(m_poiprov_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUPAK]),(gpointer)E_GRUPAK);
tooltips_enter[E_GRUPAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUPAK],data.knopka_enter[E_GRUPAK],gettext("Выбор группы контрагента"),NULL);

data.entry[E_GRUPAK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPAK]), data.entry[E_GRUPAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUPAK]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPAK]),data.rk->grupak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUPAK]),(gpointer)E_GRUPAK);

sprintf(strsql,"%s (,,)",gettext("КЭКЗ"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_KEKV] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KEKV]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KEKV]), data.entry[E_KEKV], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KEKV]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KEKV]),data.rk->kredit.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KEKV]),(gpointer)E_KEKV);





sprintf(strsql,"%s (%s)(%s)",gettext("Дата начала поиска"),gettext("по дате записи"),gettext("д.м.г"));
data.knopka_enter[E_DATANZ]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATANZ]), data.knopka_enter[E_DATANZ], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATANZ]),"clicked",GTK_SIGNAL_FUNC(m_poiprov_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATANZ]),(gpointer)E_DATANZ);
tooltips_enter[E_DATANZ]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATANZ],data.knopka_enter[E_DATANZ],gettext("Выбор даты"),NULL);

data.entry[E_DATANZ] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATANZ]), data.entry[E_DATANZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATANZ]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATANZ]),data.rk->datanz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATANZ]),(gpointer)E_DATANZ);

sprintf(strsql,"%s (%s)(%s)",gettext("Дата конца"),gettext("по дате записи"),gettext("д.м.г"));
data.knopka_enter[E_DATAKZ]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAKZ]), data.knopka_enter[E_DATAKZ], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAKZ]),"clicked",GTK_SIGNAL_FUNC(m_poiprov_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAKZ]),(gpointer)E_DATAKZ);
tooltips_enter[E_DATAKZ]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAKZ],data.knopka_enter[E_DATAKZ],gettext("Выбор даты"),NULL);

data.entry[E_DATAKZ] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAKZ]), data.entry[E_DATAKZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAKZ]), "activate",GTK_SIGNAL_FUNC(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAKZ]),data.rk->datakz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAKZ]),(gpointer)E_DATAKZ);










GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(poiprov_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(poiprov_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(poiprov_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));
//gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));


gtk_main();

return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  m_poiprov_v_e_knopka(GtkWidget *widget,class poiprov_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rk->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
      
    return;  

  case E_DATANZ:

    if(iceb_calendar(&data->rk->datanz,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATANZ]),data->rk->datanz.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rk->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
      
    return;  


  case E_DATAKZ:

    if(iceb_calendar(&data->rk->datakz,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAKZ]),data->rk->datakz.ravno());
      
    return;  

  case E_SHET:
    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno_toutf());
    return;

  case E_SHETK:
    iceb_vibrek(0,"Plansh",&data->rk->shetk,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETK]),data->rk->shetk.ravno_toutf());
    return;

  case E_KONTR:
    iceb_vibrek(0,"Kontragent",&data->rk->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk->kontr.ravno_toutf());
    return;

  case E_GRUPAK:
    iceb_vibrek(0,"Gkont",&data->rk->grupak,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPAK]),data->rk->grupak.ravno_toutf());
    return;

  case E_METKA:
    poiprov_rek_metka(data);
    return;

 }
}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   poiprov_v_key_press(GtkWidget *widget,GdkEventKey *event,class poiprov_data *data)
{
//char  bros[512];
iceb_u_str shet;
iceb_u_str naim;

//printf("poiprov_v_key_press\n");
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
    return(TRUE);

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
void  poiprov_v_knopka(GtkWidget *widget,class poiprov_data *data)
{
//char bros[512];
iceb_u_str shet;
iceb_u_str naim;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->activate();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_zero();
    return;  

  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    poiprov_v_vvod(GtkWidget *widget,class poiprov_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("poiprov_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SHET:
    data->rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHETK:
    data->rk->shetk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KONTR:
    data->rk->kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOMENT:
    data->rk->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DEBET:
    data->rk->debet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KREDIT:
    data->rk->kredit.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KEKV:
    data->rk->kekv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAN:
    data->rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATANZ:
    data->rk->datanz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAKZ:
    data->rk->datakz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_METKA:
    data->rk->metka.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMDOK:
    data->rk->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODOP:
    data->rk->kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUPAK:
    data->rk->grupak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}

/**********************************/
/*Меню выбора меток проводок      */
/**********************************/
void      poiprov_rek_metka(class poiprov_data *data)
{
char bros[512];

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));
zagolovok.plus(gettext("Выберите нужное"));

sprintf(bros,"%s %s",gettext("МУ"),gettext("Материальный учет"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",gettext("ЗП"),gettext("Зароботная плата"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",gettext("УОС"),gettext("Учет основных средств"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",gettext("ПЛТ"),gettext("Платежные поручения"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",gettext("ТРЕ"),gettext("Платежные требования"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",gettext("УСЛ"),gettext("Учет услуг"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",gettext("КО"),gettext("Учет кассовых ордеров"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",gettext("УКР"),gettext("Учет командировочных расходов"));
punkt_m.plus(bros);

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);

switch(nomer)
 {
  case -1:
   return;

  case 0:
   sprintf(bros,"%s",gettext("МУ"));
   break;

  case 1:
   sprintf(bros,"%s",gettext("ЗП"));
   break;

  case 2:
   sprintf(bros,"%s",gettext("УОС"));
   break;

  case 3:
   sprintf(bros,"%s",gettext("ПЛТ"));
   break;

  case 4:
   sprintf(bros,"%s",gettext("ТРЕ"));
   break;

  case 5:
   sprintf(bros,"%s",gettext("УСЛ"));
   break;

  case 6:
   sprintf(bros,"%s",gettext("КО"));
   break;

  case 7:
   sprintf(bros,"%s",gettext("УКР"));
   break;
   
 }

if(data->rk->metka.getdlinna() > 1)
  data->rk->metka.plus(",");
data->rk->metka.plus(bros);

gtk_entry_set_text(GTK_ENTRY(data->entry[E_METKA]),data->rk->metka.ravno_toutf());

}
