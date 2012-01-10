/*$Id: l_xrnnv_v.c,v 1.13 2011-09-05 08:18:27 sasa Exp $*/
/*05.11.2010	07.05.2008	Белых А.И.	l_xrnnv_v.c
Ввод и корректировка записи в реестр выданных налоговых накладных
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"

enum
 {
  E_NOM_NAL_NAK,
  E_DATAVNN,
  E_SUMD1,
  E_NDS1,
  E_INN,
  E_NAIM_KONTR,
  E_KOMENT,
  E_KOD_GR,
  E_KOD_OP,
  E_VIDDOK,
  E_DATV,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_xrnnv_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str datapnn_z; /*Дата получения налоговой накладной до корректировки*/

  class iceb_u_str nom_nal_nak;
  class iceb_u_str datavnn;
  class iceb_u_str sumd1;
  class iceb_u_str nds1;
  class iceb_u_str inn;
  class iceb_u_str naim_kontr;
  class iceb_u_str koment;
  class iceb_u_str kod_gr;
  class iceb_u_str kod_op;
  class iceb_u_str datanak;
  class iceb_u_str nom_dok;
  class iceb_u_str viddok;
  class iceb_u_str datv;
  int metka_dok;   
  int nom_zap;
  int metka_pods;
  
  class iceb_u_str datavnn_z;

  l_xrnnv_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    nom_zap=0;
    metka_pods=0;
    clear();
    metka_dok=0;
    datavnn_z.plus("");
   }

  void read_rek()
   {
    nom_nal_nak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOM_NAL_NAK]))));
    datavnn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAVNN]))));
    sumd1.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SUMD1]))));
    nds1.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NDS1]))));
    inn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_INN]))));
    naim_kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM_KONTR]))));
    koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
    kod_gr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_GR]))));
    kod_op.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_OP]))));
    viddok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VIDDOK]))));
    datv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATV]))));
   }
  void clear()
   {
    nom_nal_nak.new_plus("");
    datavnn.new_plus("");
    sumd1.new_plus("");
    nds1.new_plus("");
    inn.new_plus("");
    naim_kontr.new_plus("");
    koment.new_plus("");
    kod_gr.new_plus("");
    kod_op.new_plus("");
    datanak.new_plus("");
    nom_dok.new_plus("");
    viddok.new_plus("");
    datv.new_plus("");
   }
 };


gboolean   l_xrnnv_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_xrnnv_v_data *data);
void  l_xrnnv_v_knopka(GtkWidget *widget,class l_xrnnv_v_data *data);
void    l_xrnnv_v_vvod(GtkWidget *widget,class l_xrnnv_v_data *data);
int l_xrnnv_zap(class l_xrnnv_v_data *data);

void  l_xrnnv_v_e_knopka(GtkWidget *widget,class l_xrnnv_v_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_xrnnv_v(class iceb_u_str *nom_nal_nak,int nom_zap,GtkWidget *wpredok)
{

class l_xrnnv_v_data data;
char strsql[512];
iceb_u_str kikz;

data.nom_zap=nom_zap;

if(nom_zap > 0)
 {
  sprintf(strsql,"select * from Reenn where nz=%d",nom_zap);
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.nom_nal_nak.new_plus(row[2]);
  data.datavnn.new_plus(iceb_u_datzap(row[1]));
  data.datavnn_z.new_plus(iceb_u_datzap(row[1]));

  data.sumd1.new_plus(row[3]);
  data.nds1.new_plus(row[4]);

  data.inn.new_plus(row[6]);
  data.naim_kontr.new_plus(row[7]);
  data.koment.new_plus(row[11]);
  data.kod_gr.new_plus(row[14]);
  data.kod_op.new_plus(row[15]);
  data.datanak.new_plus(iceb_u_datzap(row[9]));
  data.nom_dok.new_plus(row[10]);
  data.metka_pods=atoi(row[8]);  
  data.viddok.new_plus(row[16]);  
  data.datv.new_plus(row[17]);  
  kikz.plus(iceb_kikz(row[12],row[13],wpredok));
  
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(nom_zap == 0)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno_toutf());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_xrnnv_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

GtkWidget *vbox = gtk_vbox_new (FALSE,1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE,1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
//gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_box_pack_start(GTK_BOX (vbox), label, TRUE, TRUE,1);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start(GTK_BOX (vbox), hbox[i], TRUE, TRUE,1);
// gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
//gtk_container_add (GTK_CONTAINER (vbox), hboxknop);
gtk_box_pack_start(GTK_BOX (vbox), hboxknop, TRUE, TRUE,1);

GtkTooltips *tooltips_enter[KOLENTER];

label=gtk_label_new(gettext("Номер налоговой накладной"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_NAL_NAK]), label, FALSE, FALSE,1);

data.entry[E_NOM_NAL_NAK] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_NAL_NAK]), data.entry[E_NOM_NAL_NAK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOM_NAL_NAK]), "activate",GTK_SIGNAL_FUNC(l_xrnnv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOM_NAL_NAK]),data.nom_nal_nak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOM_NAL_NAK]),(gpointer)E_NOM_NAL_NAK);


sprintf(strsql,"%s (%s)",gettext("Дата выдачи налоговой накладной"),gettext("д.м.г"));
data.knopka_enter[E_DATAVNN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAVNN]), data.knopka_enter[E_DATAVNN], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAVNN]),"clicked",GTK_SIGNAL_FUNC(l_xrnnv_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAVNN]),(gpointer)E_DATAVNN);
tooltips_enter[E_DATAVNN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAVNN],data.knopka_enter[E_DATAVNN],gettext("Выбор даты"),NULL);

data.entry[E_DATAVNN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAVNN]), data.entry[E_DATAVNN], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAVNN]), "activate",GTK_SIGNAL_FUNC(l_xrnnv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAVNN]),data.datavnn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAVNN]),(gpointer)E_DATAVNN);


sprintf(strsql,gettext("Сумма по документу"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMD1]), label, FALSE, FALSE,1);

data.entry[E_SUMD1] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMD1]), data.entry[E_SUMD1], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMD1]), "activate",GTK_SIGNAL_FUNC(l_xrnnv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMD1]),data.sumd1.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMD1]),(gpointer)E_SUMD1);

sprintf(strsql,gettext("Сумма НДС"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NDS1]), label, FALSE, FALSE,1);

data.entry[E_NDS1] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS1]), data.entry[E_NDS1], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NDS1]), "activate",GTK_SIGNAL_FUNC(l_xrnnv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NDS1]),data.nds1.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NDS1]),(gpointer)E_NDS1);



label=gtk_label_new(gettext("Индивидуальный налоговый номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), label, FALSE, FALSE,1);

data.entry[E_INN] = gtk_entry_new_with_max_length (29);
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.entry[E_INN], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_INN]), "activate",GTK_SIGNAL_FUNC(l_xrnnv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INN]),data.inn.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_INN]),(gpointer)E_INN);


sprintf(strsql,"%s",gettext("Наименование контрагента"));
data.knopka_enter[E_NAIM_KONTR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_KONTR]), data.knopka_enter[E_NAIM_KONTR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NAIM_KONTR]),"clicked",GTK_SIGNAL_FUNC(l_xrnnv_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NAIM_KONTR]),(gpointer)E_NAIM_KONTR);
tooltips_enter[E_NAIM_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_NAIM_KONTR],data.knopka_enter[E_NAIM_KONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_NAIM_KONTR] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_KONTR]), data.entry[E_NAIM_KONTR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM_KONTR]), "activate",GTK_SIGNAL_FUNC(l_xrnnv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM_KONTR]),data.naim_kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM_KONTR]),(gpointer)E_NAIM_KONTR);


label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE,1);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_xrnnv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);


sprintf(strsql,"%s",gettext("Код группы"));
data.knopka_enter[E_KOD_GR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR]), data.knopka_enter[E_KOD_GR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_GR]),"clicked",GTK_SIGNAL_FUNC(l_xrnnv_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_GR]),(gpointer)E_KOD_GR);
tooltips_enter[E_KOD_GR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_GR],data.knopka_enter[E_KOD_GR],gettext("Выбор группы"),NULL);

data.entry[E_KOD_GR] = gtk_entry_new_with_max_length (9);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR]), data.entry[E_KOD_GR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_GR]), "activate",GTK_SIGNAL_FUNC(l_xrnnv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_GR]),data.kod_gr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_GR]),(gpointer)E_KOD_GR);


sprintf(strsql,"%s",gettext("Код операции"));
data.knopka_enter[E_KOD_OP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), data.knopka_enter[E_KOD_OP], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_OP]),"clicked",GTK_SIGNAL_FUNC(l_xrnnv_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_OP]),(gpointer)E_KOD_OP);
tooltips_enter[E_KOD_OP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_OP],data.knopka_enter[E_KOD_OP],gettext("Выбор операции"),NULL);

data.entry[E_KOD_OP] = gtk_entry_new_with_max_length (9);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), data.entry[E_KOD_OP], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_OP]), "activate",GTK_SIGNAL_FUNC(l_xrnnv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_OP]),data.kod_op.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_OP]),(gpointer)E_KOD_OP);


sprintf(strsql,"%s",gettext("Вид документа"));
data.knopka_enter[E_VIDDOK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_VIDDOK]), data.knopka_enter[E_VIDDOK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_VIDDOK]),"clicked",GTK_SIGNAL_FUNC(l_xrnnv_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_VIDDOK]),(gpointer)E_VIDDOK);
tooltips_enter[E_VIDDOK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_VIDDOK],data.knopka_enter[E_VIDDOK],gettext("Выбор вида документа"),NULL);

data.entry[E_VIDDOK] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[E_VIDDOK]), data.entry[E_VIDDOK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VIDDOK]), "activate",GTK_SIGNAL_FUNC(l_xrnnv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VIDDOK]),data.viddok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VIDDOK]),(gpointer)E_VIDDOK);

sprintf(strsql,"%s",gettext("Дата выписки налоговой накладной"));
data.knopka_enter[E_DATV]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATV]), data.knopka_enter[E_DATV], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATV]),"clicked",GTK_SIGNAL_FUNC(l_xrnnv_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATV]),(gpointer)E_DATV);
tooltips_enter[E_DATV]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATV],data.knopka_enter[E_DATV],gettext("Выбор даты"),NULL);

data.entry[E_DATV] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATV]), data.entry[E_DATV], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATV]), "activate",GTK_SIGNAL_FUNC(l_xrnnv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATV]),data.datv.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATV]),(gpointer)E_DATV);




GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_xrnnv_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_xrnnv_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 nom_nal_nak->new_plus(data.nom_nal_nak.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_xrnnv_v_e_knopka(GtkWidget *widget,class l_xrnnv_v_data *data)
{
SQL_str row;
class SQLCURSOR cur;
iceb_u_str kod("");
iceb_u_str naim("");
char strsql[512];
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATAVNN:

    if(iceb_calendar(&data->datavnn,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAVNN]),data->datavnn.ravno());
      
    return;  



  case E_NAIM_KONTR:

    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
     {
      data->naim_kontr.new_plus(iceb_get_pnk(kod.ravno(),1,data->window));
      /*читаем налоговый номер*/
      sprintf(strsql,"select innn from Kontragent where kodkon='%s'",kod.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_INN]),row[0]);
        
     }    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NAIM_KONTR]),data->naim_kontr.ravno_toutf());
    return;  

  case E_KOD_GR:
    if(l_xrnngpn(1,&kod,&naim,data->window) == 0)
     data->kod_gr.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_GR]),data->kod_gr.ravno_toutf());
    return;  

  case E_KOD_OP:
    xrnn_kord(1,&data->kod_op,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_OP]),data->kod_op.ravno_toutf());
    return;  

  case E_VIDDOK:
    rnnvvd(&data->viddok,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VIDDOK]),data->viddok.ravno_toutf());
    return;  

  case E_DATV:

    if(iceb_calendar(&data->datv,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATV]),data->datv.ravno());
      
    return;  
     
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_xrnnv_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_xrnnv_v_data *data)
{
//char  bros[512];

//printf("l_xrnnv_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

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
void  l_xrnnv_v_knopka(GtkWidget *widget,class l_xrnnv_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_xrnnv_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_xrnnv_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
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

void    l_xrnnv_v_vvod(GtkWidget *widget,class l_xrnnv_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_xrnnv_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_NOM_NAL_NAK:
    data->nom_nal_nak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATAVNN:
    data->datavnn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_xrnnv_zap(class l_xrnnv_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);


short dd=0,md=0,gd=0;

if(iceb_u_rsdat(&dd,&md,&gd,data->datavnn.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата получения налоговой накладной!"),data->window);
  return(1);
 }
if(iceb_pbpds(md,gd,data->window) != 0)
 return(1);

short dz=0,mz=0,gz=0;
iceb_u_rsdat(&dz,&mz,&gz,data->datavnn_z.ravno(),1);

if(data->nom_zap != 0)
 if(mz != md || gz != gd)
  if(iceb_pbpds(mz,gz,data->window) != 0)
   return(1);


/*************
if(metka_pods == 0 )
 {
  if(VV.data_ravno(16)[0] != '\0')
  if(iceb_u_rsdat(&ddp,&mdp,&gdp,VV.data_ravno(16),1) != 0)
   {
    VVOD SOOB(1);
    sprintf(strsql,gettext("Неправильно введена дата документа !"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;

   }
  sprintf(datdi,"%04d-%02d-%02d",gdp,mdp,ddp);
  strncpy(nomdi,VV.data_ravno(17),sizeof(nomdi)-1);
 }
*****************/


if(data->kod_gr.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Reegrup2 where kod='%s'",data->kod_gr.ravno_filtr());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код группы"),data->kod_gr.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }

if(data->nom_zap == 0) //Ввод новой записи
 {
  /*Проверяем в этом году уже был такой номер*/
  sprintf(strsql,"select datd from Reenn1 where datd >= '%04d-01-01' and datd <= '%04d-12-01' and nnn='%s'",
  gd,gd,data->nom_nal_nak.ravno());    
  SQL_str row;
  class SQLCURSOR cur;    
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)    
   {
    class iceb_u_str repl;
    
    sprintf(strsql,"%s!",gettext("Внимание"));
    repl.plus(strsql);
    
    repl.ps_plus(gettext("Уже есть такой номер налоговой накладной в этом году"));
            
    short d,m,g;
    iceb_u_rsdat(&d,&m,&g,row[0],2);
    sprintf(strsql,"%d.%d.%d",d,m,g);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,data->window);
   }
  //фильтрование символьных переменных внутри функции
  if(zapvreew(dd,md,gd,
   data->nom_nal_nak.ravno(),
   data->sumd1.ravno_atof(),
   data->nds1.ravno_atof(),
   data->metka_dok,
   data->inn.ravno(),
   data->naim_kontr.ravno(),
   data->metka_pods,
   data->datanak.ravno_sqldata(),
   data->nom_dok.ravno(),
   data->koment.ravno(),
   data->kod_gr.ravno(),
   data->kod_op.ravno(),
   data->viddok.ravno(),
   data->datv.ravno_sqldata(),
   data->window) != 0)
    return(1);
 }
else
 {

  sprintf(strsql,"update Reenn set \
datd='%04d-%02d-%02d',\
nnn='%s',\
sumd=%.2f,\
snds=%.2f,\
inn='%s',\
nkontr='%s',\
datdi='%s',\
nomdi='%s',\
koment='%s',\
ktoi=%d,\
vrem=%ld,\
gr='%s',\
ko='%s',\
vd='%s',\
datv='%s',\
nnni=%d \
where nz=%d",
  gd,md,dd,
  data->nom_nal_nak.ravno(),
  data->sumd1.ravno_atof(),
  data->nds1.ravno_atof(),
  data->inn.ravno(),
  data->naim_kontr.ravno_filtr(),
  data->datanak.ravno_sqldata(),
  data->nom_dok.ravno(),
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),
  vrem,
  data->kod_gr.ravno_filtr(),
  data->kod_op.ravno_filtr(),
  data->viddok.ravno_filtr(),
  data->datv.ravno_sqldata(),
  data->nom_nal_nak.ravno_atoi(),
  data->nom_zap);      

  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   return(1);

 }

gtk_widget_hide(data->window);


return(0);

}
