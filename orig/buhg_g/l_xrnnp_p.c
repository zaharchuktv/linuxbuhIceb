/*$Id: l_xrnnp_p.c,v 1.9 2011-02-21 07:35:54 sasa Exp $*/
/*03.12.2009	15.04.2008	Белых А.И.	l_xrnnp_p.c
Ввод реквизитов поиска
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "xrnn_poiw.h"
enum
 {
  E_DATAN,
  E_DATAK,
  E_NNN,
  E_NOMDOK,
  E_INN,
  E_NAIM_KONTR,
  E_KOMENT,
  E_KOD_GR,
  E_PODS,
  E_KOD_OP,
  E_DATAVNN,
  E_DATAVNK,
  E_VIDDOK,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_xrnnp_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class xrnn_poiw *rk;
  
  l_xrnnp_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN]))));
    rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK]))));
    rk->nnn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NNN]))));
    rk->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK]))));
    rk->inn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_INN]))));
    rk->naim_kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM_KONTR]))));
    rk->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
    rk->kodgr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_GR]))));
    rk->kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_OP]))));
    rk->podsistema.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PODS]))));
    rk->datavnn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAVNN]))));
    rk->datavnk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAVNK]))));
    rk->viddok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VIDDOK]))));
   }
  void clear()
   {
    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   
   }
 };


gboolean   l_xrnnp_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_xrnnp_p_data *data);
void  l_xrnnp_p_knopka(GtkWidget *widget,class l_xrnnp_p_data *data);
void    l_xrnnp_p_vvod(GtkWidget *widget,class l_xrnnp_p_data *data);

void  l_xrnnp_p_e_knopka(GtkWidget *widget,class l_xrnnp_p_data *data);
int l_xrnnp_vpds(class iceb_u_str *pods,GtkWidget *wpredok);

extern SQL_baza  bd;
extern char      *name_system;

int l_xrnnp_p(class xrnn_poiw *rek_poi,GtkWidget *wpredok)
{

class l_xrnnp_p_data data;
char strsql[512];

data.rk=rek_poi;

 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",name_system,gettext("Поиск"));
label=gtk_label_new(gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_xrnnp_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_vbox_new (FALSE,1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE,1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start(GTK_BOX (vbox), label, TRUE, TRUE,1);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start(GTK_BOX (vbox), hbox[i], TRUE, TRUE,1);
gtk_box_pack_start(GTK_BOX (vbox), hboxknop, TRUE, TRUE,1);

GtkTooltips *tooltips_enter[KOLENTER];


sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(l_xrnnp_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(l_xrnnp_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);

label=gtk_label_new(gettext("Номер налоговой накладной"));
gtk_box_pack_start (GTK_BOX (hbox[E_NNN]), label, FALSE, FALSE,1);

data.entry[E_NNN] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NNN]), data.entry[E_NNN], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NNN]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NNN]),data.rk->nnn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NNN]),(gpointer)E_NNN);

label=gtk_label_new(gettext("Номер документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE,1);

data.entry[E_NOMDOK] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);


label=gtk_label_new(gettext("Индивидуальный налоговый номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), label, FALSE, FALSE,1);

data.entry[E_INN] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.entry[E_INN], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_INN]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INN]),data.rk->inn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_INN]),(gpointer)E_INN);

sprintf(strsql,"%s",gettext("Наименование контрагента"));
data.knopka_enter[E_NAIM_KONTR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_KONTR]), data.knopka_enter[E_NAIM_KONTR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NAIM_KONTR]),"clicked",GTK_SIGNAL_FUNC(l_xrnnp_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NAIM_KONTR]),(gpointer)E_NAIM_KONTR);
tooltips_enter[E_NAIM_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_NAIM_KONTR],data.knopka_enter[E_NAIM_KONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_NAIM_KONTR] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_KONTR]), data.entry[E_NAIM_KONTR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM_KONTR]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM_KONTR]),data.rk->naim_kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM_KONTR]),(gpointer)E_NAIM_KONTR);


label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE,1);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk->koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);


sprintf(strsql,"%s (,,)",gettext("Код группы"));
data.knopka_enter[E_KOD_GR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR]), data.knopka_enter[E_KOD_GR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_GR]),"clicked",GTK_SIGNAL_FUNC(l_xrnnp_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_GR]),(gpointer)E_KOD_GR);
tooltips_enter[E_KOD_GR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_GR],data.knopka_enter[E_KOD_GR],gettext("Выбор группы"),NULL);

data.entry[E_KOD_GR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR]), data.entry[E_KOD_GR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_GR]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_GR]),data.rk->kodgr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_GR]),(gpointer)E_KOD_GR);


sprintf(strsql,"%s (,,)",gettext("Код операции"));
data.knopka_enter[E_KOD_OP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), data.knopka_enter[E_KOD_OP], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_OP]),"clicked",GTK_SIGNAL_FUNC(l_xrnnp_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_OP]),(gpointer)E_KOD_OP);
tooltips_enter[E_KOD_OP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_OP],data.knopka_enter[E_KOD_OP],gettext("Выбор операции"),NULL);

data.entry[E_KOD_OP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), data.entry[E_KOD_OP], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_OP]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_OP]),data.rk->kodop.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_OP]),(gpointer)E_KOD_OP);

sprintf(strsql,"%s (,,)",gettext("Подсистема"));
data.knopka_enter[E_PODS]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODS]), data.knopka_enter[E_PODS], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODS]),"clicked",GTK_SIGNAL_FUNC(l_xrnnp_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODS]),(gpointer)E_PODS);
tooltips_enter[E_PODS]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODS],data.knopka_enter[E_PODS],gettext("Выбор подсистемы"),NULL);

data.entry[E_PODS] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODS]), data.entry[E_PODS], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODS]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODS]),data.rk->podsistema.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODS]),(gpointer)E_PODS);


sprintf(strsql,"%s (%s)",gettext("Дата выписки нал-вой нак-ной начало периода"),gettext("д.м.г"));
data.knopka_enter[E_DATAVNN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAVNN]), data.knopka_enter[E_DATAVNN], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAVNN]),"clicked",GTK_SIGNAL_FUNC(l_xrnnp_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAVNN]),(gpointer)E_DATAVNN);
tooltips_enter[E_DATAVNN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAVNN],data.knopka_enter[E_DATAVNN],gettext("Выбор даты"),NULL);

data.entry[E_DATAVNN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAVNN]), data.entry[E_DATAVNN], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAVNN]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAVNN]),data.rk->datavnn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAVNN]),(gpointer)E_DATAVNN);

sprintf(strsql,"%s (%s)",gettext("Дата выписки нал-вой нак-ной конец периода"),gettext("д.м.г"));
data.knopka_enter[E_DATAVNK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAVNK]), data.knopka_enter[E_DATAVNK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAVNK]),"clicked",GTK_SIGNAL_FUNC(l_xrnnp_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAVNK]),(gpointer)E_DATAVNK);
tooltips_enter[E_DATAVNK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAVNK],data.knopka_enter[E_DATAVNK],gettext("Выбор даты"),NULL);

data.entry[E_DATAVNK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAVNK]), data.entry[E_DATAVNK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAVNK]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAVNK]),data.rk->datavnk.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAVNK]),(gpointer)E_DATAVNK);

sprintf(strsql,"%s",gettext("Вид документа"));
data.knopka_enter[E_VIDDOK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_VIDDOK]), data.knopka_enter[E_VIDDOK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_VIDDOK]),"clicked",GTK_SIGNAL_FUNC(l_xrnnp_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_VIDDOK]),(gpointer)E_VIDDOK);
tooltips_enter[E_VIDDOK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_VIDDOK],data.knopka_enter[E_VIDDOK],gettext("Выбор вида документа"),NULL);

data.entry[E_VIDDOK] = gtk_entry_new_with_max_length (7);
gtk_box_pack_start (GTK_BOX (hbox[E_VIDDOK]), data.entry[E_VIDDOK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VIDDOK]), "activate",GTK_SIGNAL_FUNC(l_xrnnp_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VIDDOK]),data.rk->viddok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VIDDOK]),(gpointer)E_VIDDOK);


GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_xrnnp_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(l_xrnnp_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без поиска нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_xrnnp_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

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
void  l_xrnnp_p_e_knopka(GtkWidget *widget,class l_xrnnp_p_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rk->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rk->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datak.ravno());
      
    return;  


  case E_DATAVNN:

    if(iceb_calendar(&data->rk->datavnn,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAVNN]),data->rk->datavnn.ravno());
      
    return;  

  case E_DATAVNK:

    if(iceb_calendar(&data->rk->datavnk,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAVNN]),data->rk->datavnk.ravno());
      
    return;  

  case E_VIDDOK:
    
    if(rnnvvd(&kod,data->window) == 0)
     data->rk->viddok.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VIDDOK]),data->rk->viddok.ravno_toutf());
    return;  

  case E_NAIM_KONTR:
    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
     data->rk->naim_kontr.new_plus(naim.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NAIM_KONTR]),data->rk->naim_kontr.ravno_toutf());
    return;  

  case E_KOD_GR:
    if(l_xrnngpn(1,&kod,&naim,data->window) == 0)
     data->rk->kodgr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_GR]),data->rk->kodgr.ravno_toutf());
    return;  

  case E_KOD_OP:
    xrnn_kopd(0,&data->rk->kodop,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_OP]),data->rk->kodop.ravno_toutf());
    return;  

  case E_PODS:
    l_xrnnp_vpds(&data->rk->podsistema,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODS]),data->rk->podsistema.ravno_toutf());
    return;  
     
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_xrnnp_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_xrnnp_p_data *data)
{
//char  bros[512];

//printf("l_xrnnp_p_key_press\n");
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
void  l_xrnnp_p_knopka(GtkWidget *widget,class l_xrnnp_p_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_xrnnp_p_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(data->rk->datan.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно ввели дату начала!"),data->window);
      return;
     }
    if(data->rk->datak.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно ввели дату конца!"),data->window);
      return;
     }

    if(data->rk->datavnn.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно ввели дату выписки!"),data->window);
      return;
     }

    if(data->rk->datavnk.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно ввели дату выписки!"),data->window);
      return;
     }
    /*Чтобы вторая дата заполнилась концом месяца если не введена*/
    if(data->rk->datavnn.getdlinna() > 1)
     iceb_rsdatp_str(&data->rk->datavnn,&data->rk->datavnk,data->window);
    
    data->rk->metka_poi=1;
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear();
    return;
    
  case FK10:
    data->rk->metka_poi=0;
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_xrnnp_p_vvod(GtkWidget *widget,class l_xrnnp_p_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_xrnnp_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/********************************/
/*Выбор подсистемы*/
/****************************/
int l_xrnnp_vpds(class iceb_u_str *pods,GtkWidget *wpredok)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

char strsql[512];

sprintf(strsql,"%-3s %s",gettext("МУ"),gettext("Материальный учет"));
punkt_m.plus(strsql);

sprintf(strsql,"%-3s %s",gettext("УСЛ"),gettext("Учет услуг"));
punkt_m.plus(strsql);

sprintf(strsql,"%-3s %s",gettext("УОС"),gettext("Учет основных средств"));
punkt_m.plus(strsql);

sprintf(strsql,"%-3s %s",gettext("УКР"),gettext("Учет командировочных расходов"));
punkt_m.plus(strsql);

sprintf(strsql,"%-3s %s",gettext("ГК"),gettext("Главная книга"));
punkt_m.plus(strsql);


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

switch(nomer)
 {
  case 0:
   pods->z_plus(gettext("МУ"));
   return(0); 

  case 1:
   pods->z_plus(gettext("УСЛ"));
   return(0); 

  case 2:
   pods->z_plus(gettext("УОС"));
   return(0); 

  case 3:
   pods->z_plus(gettext("УКР"));
   return(0); 

  case 4:
   pods->z_plus(gettext("ГК"));
   return(0); 

 }
return(1);
}

